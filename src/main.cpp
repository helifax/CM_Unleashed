/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

#include <Windows.h>
#include <sstream>
#include <string>
#include <TlHelp32.h>
#include <thread>
#include <deque>
#include "Psapi.h"
#include "ConfigReader.h"
#include "CMUnleashed.h"
#include "SplashWnd.h"
#include "NvapiProfileSettings/NvApiProfileSettings.h"
#include "xboxController.h"

#pragma comment(lib, "Winmm.lib")

#pragma warning(disable : 4458)
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")

// Our Reader
ConfigReader *g_reader = nullptr;
// Our Patcher
CMUnleashed *g_cmUnleashed = nullptr;

static bool _keyThreadRunning = true;
static bool _isPressAndHold = false;
static std::deque<float> queuedConv;
static std::deque<float> queuedSep;
static bool _isPatchEnabled = false;
static bool _mainMenu = true;
static bool _infoMenu = false;
static bool _profileUpdateOK = false;
static bool _pidMonitorStarted = false;
static bool _autoStartStarted = false;

// GLOBAL PRINT
void __cdecl console_log(const char *fmt, ...)
{
    const size_t MAX_SIZE = 500000;
    va_list va_alist;
    char logbuf[MAX_SIZE] = "";

    va_start(va_alist, fmt);
    _vsnprintf_s(logbuf + strlen(logbuf), MAX_SIZE, sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
    va_end(va_alist);

    printf("%s", logbuf);
    printf(">");
}
//-----------------------------------------------------------------------------

// use for hotkeys
static bool IsKeyDown(int keyCode);
static bool IsAltKeyToggleKeyDown(int keyCodeIndex);

// Xbox controller support
static CXBOXController *g_xController = new CXBOXController(1);
static bool IsXControllerAltKeyToggleKeyDown(int keyCodeIndex);
//-----------------------------------------------------------------------------

static void _Run_Keys_OneTime(size_t keyIndex, size_t &returnIndex)
{
    // Toggled Enabled
    if(IsAltKeyToggleKeyDown((int)keyIndex) || IsXControllerAltKeyToggleKeyDown((int)keyIndex))
    {
        // Convergence
        float temp = -1.0f;
        if(g_reader->GetAltConvergence((int)keyIndex, &temp))
        {
            // do it just once
            if(temp != -1.0f)
            {
                // Apply the new convergence
                g_cmUnleashed->CM_SetConvergence(&temp);
            }
        }

        // Separation
        if(g_reader->GetAltSeparation((int)keyIndex, &temp))
        {
            // do it just once
            if(temp != -1.0f)
            {
                g_cmUnleashed->CM_SetSeparationFactor(&temp);
            }
        }

        // Force the index out of size in order to break the for() loop
        returnIndex = g_reader->GetNumberOfKeys();
    }
}
//-----------------------------------------------------------------------------

static void _Run_Keys_Hold(size_t keyIndex, size_t &returnIndex)
{
    /////////////////////////////////////
    // Push to hold settings
    // Only Separation & Convergence are handled
    /////////////////////////////////////

    static float crtConv = 0;
    static float crtSep = 0;

    int currentKeyCode = g_reader->GetKeyNumber((int)keyIndex);
    if(IsKeyDown(currentKeyCode) || IsXControllerAltKeyToggleKeyDown((int)keyIndex))
    {
        // Custom Convergence
        float temp = -1.0f;
        if(g_reader->GetAltConvergence((int)keyIndex, &temp))
        {
            if(temp != -1.0f && crtConv != temp)
            {
                // Apply the new convergence
                float conv = 0;
                if(g_cmUnleashed->CM_GetConvergence(&conv))
                {
                    crtConv = conv;
                    g_cmUnleashed->CM_SetConvergence(&temp);
                }
            }
        }
        // Custom Separation
        if(g_reader->GetAltSeparation((int)keyIndex, &temp))
        {
            if(temp != -1.0f && crtSep != temp)
            {
                // Apply the new separation
                crtSep = temp;
                g_cmUnleashed->CM_SetSeparationFactor(&temp);
            }
        }
        _isPressAndHold = true;
        returnIndex = g_reader->GetNumberOfKeys();
    }
    else if(_isPressAndHold && (g_reader->GetKeyNumber((int)keyIndex) != g_xController->GetState().Gamepad.wButtons))
    {
        if(crtConv)
        {
            g_cmUnleashed->CM_SetConvergence(&crtConv);
            crtConv = 0;
        }

        if(crtSep)
        {
            float revertFactor = 1.0f;
            g_cmUnleashed->CM_SetSeparationFactor(&revertFactor);
            crtSep = 0;
        }
        g_reader->SetKeyPrevState(g_reader->GetKeyNumber((int)keyIndex), 0);
        _isPressAndHold = false;
    }
}
//-----------------------------------------------------------------------------

static void _Run_Keys_Toggle(size_t keyIndex, size_t &returnIndex)
{
    static float crtSep = 0;
    static float crtConv = 0;

    // Toggled Enabled
    if(IsAltKeyToggleKeyDown((int)keyIndex) || IsXControllerAltKeyToggleKeyDown((int)keyIndex))
    {
        // Convergence
        float temp = -1.0f;
        if(g_reader->GetAltConvergence((int)keyIndex, &temp))
        {
            // First time
            if(!queuedConv.size())
            {
                if(temp != -1.0f && crtConv != temp)
                {
                    float conv = 0;
                    if(g_cmUnleashed->CM_GetConvergence(&conv))
                    {
                        // Apply the new convergence
                        queuedConv.push_back(temp);
                        crtConv = conv;
                        g_cmUnleashed->CM_SetConvergence(&temp);
                    }
                }
            }
            // Queue
            else if(queuedConv[queuedConv.size() - 1] != temp)
            {
                if(temp != -1.0f)
                {
                    queuedConv.push_back(temp);
                    g_cmUnleashed->CM_SetConvergence(&temp);
                }
            }
            // Revert back only on the last convergence!
            else if(queuedConv[queuedConv.size() - 1] == temp)
            {
                // Revert to original
                g_cmUnleashed->CM_SetConvergence(&crtConv);
                queuedConv.clear();
                crtConv = 0;
            }
        }

        // Separation
        if(g_reader->GetAltSeparation((int)keyIndex, &temp))
        {
            // First time
            if(!queuedSep.size())
            {
                if(temp != -1.0f && crtSep != temp)
                {
                    // Apply the new separation
                    queuedSep.push_back(temp);
                    crtSep = temp;
                    g_cmUnleashed->CM_SetSeparationFactor(&temp);
                }
            }
            // Queue
            else if(queuedSep[queuedSep.size() - 1] != temp)
            {
                if(temp != -1.0f)
                {
                    queuedSep.push_back(temp);
                    g_cmUnleashed->CM_SetSeparationFactor(&temp);
                }
            }
            // Revert back only on the last convergence!
            else if(queuedSep[queuedSep.size() - 1] == temp)
            {
                // Revert to original
                float revertFactor = 1.0f;
                g_cmUnleashed->CM_SetSeparationFactor(&revertFactor);
                queuedSep.clear();
                crtSep = 0;
            }
        }

        // Force the index out of size in order to break the for() loop
        returnIndex = g_reader->GetNumberOfKeys();
    }
}
//-----------------------------------------------------------------------------

static void _ExeMonitorThread()
{
    std::string exeName = g_reader->GetGameExe();
    while(g_cmUnleashed->GetExePid() && _pidMonitorStarted)
    {
        _pidMonitorStarted = true;
        g_cmUnleashed->UpdateExePid(g_cmUnleashed->getPid(exeName));

        // Look every 5 secondS
        if(g_cmUnleashed->GetExePid())
            Sleep(5000);
    }

    // If we get here it means the application died!
    g_cmUnleashed->RestoreOriginal(exeName);
    _pidMonitorStarted = false;
    _isPatchEnabled = false;
}
//-----------------------------------------------------------------------------

static void _AutoStart()
{
    bool exeFound = false;
    while(!exeFound && _autoStartStarted)
    {
        std::string gameExeName = g_reader->GetGameExe();
        DWORD exePid = g_cmUnleashed->getPid(gameExeName);

        if(exePid != 0)
        {
            exeFound = true;
            console_log("%s found! Attempting to Enable!\n", gameExeName.c_str());
            // Wait a bit!
            Sleep(10000);
            // Enable the Patching
            _isPatchEnabled = g_cmUnleashed->DoPatching(gameExeName);
            break;
        }
        Sleep(1000);
    }
    _autoStartStarted = false;

    // Start our monitor thread
    _pidMonitorStarted = true;
    std::thread pidMonitor(_ExeMonitorThread);
    pidMonitor.detach();
}
//-----------------------------------------------------------------------------

static void _KeyThread()
{
    while(_keyThreadRunning)
    {
        if(IsKeyDown(VK_CONTROL) && IsKeyDown(VK_SHIFT) && IsKeyDown(0x54))  //T
        {
            // Grab our EXE name
            std::string gameExeName = g_reader->GetGameExe();

            if(!_isPatchEnabled)
            {
                // Do it again, as the ini file could have changed
                if(g_reader->UpdateCMProfile())
                    _profileUpdateOK = NvApi_3DVisionProfileSetup(g_reader->GetGameExe(), g_reader->GetStereoTexture(), g_reader->GetCMProfile(), g_reader->GetCMConvergence(), g_reader->GetCMComments());

                // Do the patching
                _isPatchEnabled = g_cmUnleashed->DoPatching(gameExeName);

                // Start the monitor thread
                if(_isPatchEnabled)
                {
                    // Wait for the current monitor to end.
                    while(_pidMonitorStarted)
                        Sleep(100);

                    // Start or-restart
                    _pidMonitorStarted = true;
                    std::thread pidMonitor(_ExeMonitorThread);
                    pidMonitor.detach();
                }
            }
            else
            {
                // If there is an error here
                // We assume we couldn't clean and the patch is not applied!
                _isPatchEnabled = false;
                g_cmUnleashed->RestoreOriginal(gameExeName);

                // stop the monitor
                _pidMonitorStarted = false;
            }

            Sleep(300);
        }
        else if(IsKeyDown(VK_CONTROL) && IsKeyDown(VK_SHIFT) && IsKeyDown(VK_HOME))
        {
            float conv = 0.0f;
            float sep = 0.0f;
            g_cmUnleashed->CM_GetConvergence(&conv);
            g_cmUnleashed->CM_GetSeparationFactor(&sep);

            console_log("-------------------------------------------------------------------\n");
            if(sep)
                console_log("Current Separation Percentage: %f\n", sep * 100);
            else
                console_log("!!! Can not get current Separation Percentage !!!\n");
            if(conv)
                console_log("Current Convergence: %f\n", conv);
            else
                console_log("!!! Can not get current Convergence !!!\n");
            console_log("-------------------------------------------------------------------\n\n");

            Sleep(300);
        }
        else if(IsKeyDown(VK_CONTROL) && IsKeyDown(VK_SHIFT) && IsKeyDown(VK_F10))
        {
            console_log("-------------------------------------------------------------------\n");
            console_log("\"3DVision_CM_Unleashed.ini\" file successfully read!\n");
            console_log("-------------------------------------------------------------------\n\n");

            // Re-trigger the config reader
            if(g_reader)
            {
                _autoStartStarted = false;
                Sleep(500);

                // Disable the Patching (if applied)
                _isPatchEnabled = false;
                std::string gameExeName = g_reader->GetGameExe();
                g_cmUnleashed->RestoreOriginal(gameExeName);

                // stop the monitor
                _pidMonitorStarted = false;
                Sleep(500);

                delete g_reader;
                g_reader = new ConfigReader();
                PlaySound(TEXT("DeviceConnect"), NULL, SND_ALIAS | SND_ASYNC);

                // Re-try Auto-Start?
                if(g_reader->AutoStartEnabled())
                {
                    console_log("AutoStart Enabled. Looking for %s ...\n\n", g_reader->GetGameExe().c_str());
                    _autoStartStarted = true;
                    std::thread autoStart(_AutoStart);
                    autoStart.detach();
                }
            }

            Sleep(300);
        }

        // Key Handler
        /////////////////////////////////////
        size_t currentKeyIndex;
        for(currentKeyIndex = 0; currentKeyIndex < g_reader->GetNumberOfKeys(); currentKeyIndex++)
        {
            ALT_KEYS_MODE_T keyMode = g_reader->GetKeyMode(currentKeyIndex);
            switch(keyMode)
            {
            case ALT_KEYS_MODE_TOGGLE:
                _Run_Keys_Toggle(currentKeyIndex, currentKeyIndex);
                break;

            case ALT_KEYS_MODE_ONE_TIME:
                _Run_Keys_OneTime(currentKeyIndex, currentKeyIndex);
                break;

            case ALT_KEYS_MODE_HOLD:
                _Run_Keys_Hold(currentKeyIndex, currentKeyIndex);
                break;

            case ALT_KEYS_MODE_NONE:
            default:
                break;
            }
        }

        Sleep(60);
    }
}
//-----------------------------------------------------------------------------

static bool IsKeyDown(int keyCode)
{
    return (GetKeyState(keyCode) & 0x80) != 0;
}
//-----------------------------------------------------------------------------

static bool IsAltKeyToggleKeyDown(int keyCodeIndex)
{
    bool ret = false;
    int currentKey = g_reader->GetKeyNumber(keyCodeIndex);
    int state = GetKeyState(g_reader->GetKeyNumber(keyCodeIndex)) & 0x8000;

    //Return if the high byte is true (ie key is down)
    int prevState = g_reader->GetKeyPrevState(keyCodeIndex);

    if((state) && (prevState != state))
    {
        g_reader->SetKeyPrevState(keyCodeIndex, state);
        ret = true;
    }
    // Don't overwrite the XBOX states!
    else if(g_xController->GetState().Gamepad.wButtons != currentKey)
        g_reader->SetKeyPrevState(keyCodeIndex, state);

    return ret;
}
//-----------------------------------------------------------------------------

// XBOX
static bool IsXControllerAltKeyToggleKeyDown(int keyCodeIndex)
{
    bool ret = false;
    int currentKey = g_reader->GetKeyNumber(keyCodeIndex);
    if(
        (currentKey == XINPUT_GAMEPAD_DPAD_UP) ||
        (currentKey == XINPUT_GAMEPAD_DPAD_DOWN) ||
        (currentKey == XINPUT_GAMEPAD_DPAD_LEFT) ||
        (currentKey == XINPUT_GAMEPAD_DPAD_RIGHT) ||
        (currentKey == XINPUT_GAMEPAD_START) ||
        (currentKey == XINPUT_GAMEPAD_BACK) ||
        (currentKey == XINPUT_GAMEPAD_LEFT_THUMB) ||
        (currentKey == XINPUT_GAMEPAD_RIGHT_THUMB) ||
        (currentKey == XINPUT_GAMEPAD_LEFT_SHOULDER) ||
        (currentKey == XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
        (currentKey == XINPUT_GAMEPAD_A) ||
        (currentKey == XINPUT_GAMEPAD_B) ||
        (currentKey == XINPUT_GAMEPAD_X) ||
        (currentKey == XINPUT_GAMEPAD_Y))
    {
        //Return if the high byte is true (ie key is down)
        int state = g_xController->GetState().Gamepad.wButtons & g_reader->GetKeyNumber(keyCodeIndex);
        int prevState = g_reader->GetKeyPrevState(keyCodeIndex);
        if((prevState != state))
        {
            g_reader->SetKeyPrevState(keyCodeIndex, state);
            ret = true;
        }
    }
    return ret;
}
//--------------------------------------------------------------------------------------

static void showIntroMenu()
{
    system("CLS");
    console_log("\n");
    console_log("---------------------------------------------------------------------\n");
    console_log("| Welcome to 3D Vision Compatibility Mode \"Unleashed\"!              |\n");
    console_log("| Ver: 1.0.0                                                        |\n");
    console_log("| Developed by: Helifax (2019)                                      |\n");
    console_log("| If you would like to donate you can do it at: tavyhome@gmail.com  |\n");
    console_log("|                                                                   |\n");
    console_log("| Supports 32-bits(x86) and 64-bits(x64) Applications!              |\n");
    console_log("| - Windows 7 and above (x64 only) -                                |\n");
    console_log("--------------------------------------------------------------------\n");
    console_log("(Always \"RUN AS ADMIN\" !)\n");
    console_log("(Don't forget to change the Frustum (CTRL + F11) to un-stretch the image, in CM, for BEST RESULTS!)\n");
    console_log("(To refresh this Console Window press \"SPACE\" followed by \"ENTER\".)\n\n");

    if(g_reader->UpdateCMProfile())
    {
        if(!_profileUpdateOK)
        {
            console_log("-----------------------------------------------------------------------------\n");
            console_log("!!!Could not update the Nvidia Profile with the Compatibility Mode Values !!!\n");
            console_log("-----------------------------------------------------------------------------\n\n");
        }
        else
        {
            console_log("-------------------------------------------------------------------------\n");
            console_log("Nvidia Profile Updated with the Compatibility Mode values, as instructed!\n");
            console_log("-------------------------------------------------------------------------\n\n");
        }
    }
}
//-----------------------------------------------------------------------------

static void showMenu()
{
    _mainMenu = true;
    _infoMenu = false;
    showIntroMenu();
    console_log("Options:\n");
    console_log("1. In-depth Instructions on how to use it.\n");
    console_log("2. Open \"3DVision_CM_Unleshed.ini\" file.\n");
    console_log("3. Do you want to see screenshots? Or to discuss? Let's do it on GeForce Forums thread! (opens Web-page)\n");
    console_log("4. Looking for an update? Check here! (opens Web-page)\n");
    console_log("5. Quit.\n\n");
    if(g_reader->AutoStartEnabled())
    {
        console_log("AutoStart Enabled. Looking for %s ...\n\n", g_reader->GetGameExe().c_str());
    }
    console_log("Awaiting commands :)\n");
}
//-----------------------------------------------------------------------------

static void showInfo()
{
    _mainMenu = false;
    _infoMenu = true;
    system("CLS");
    console_log("\n");
    console_log("-------------------------------------------------------------------\n");
    console_log(" !!! If your game doesn't render in 3D Vision Compatibility Mode,\n");
    console_log(" check the \"[General]\" section in \"3DVision_CM_Unleased.ini\" file,\n");
    console_log(" on how to enable it !!!\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("1. What does this tool do:\n\n");
    console_log("- It Patches in Real-Time(Code Injection) the UDM Nvidia Driver responsible for 3D Vision Compatibility Mode\n  (Stereo Reprojection based on the Depth Buffer).\n");
    console_log("- It removes the limit for Stereo Separation (Depth Override).\n");
    console_log("- It adds key shortcuts to quickly jump from one Separation value to another.\n");
    console_log("- It adds key shortcuts to quickly jump from one Convergence value to another.\n");
    console_log("- It DOESN'T MODIFY the EXE of the application/game! (It should be safe, with Anti-Cheat programs)!\n");
    console_log("  (However, I still recommend being cautions, if you want to use it in online multiplayer games!)\n");
    console_log("  It allows updating the 3D Vision Compatibility Profile (for the application/game).\n");
    console_log("-------------------------------------------------------------------");
    console_log("\n\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("2. How to configure it:\n\n");
    console_log("- Open \"3DVision_CM_Unleshed.ini\" file.\n");
    console_log("- Change \"GameExecutable\" for the game EXE you want to play.\n");
    console_log("  (Make sure there are no spaces and the name is in quotes: GameExecutable=\"re2.exe\").\n");
    console_log("- Set \"AutoStart\" to true, if you want to attempt to automatically start the Patching once the EXE is detected!\n");
    console_log("  (This can fail! If the game doesn't load and 3DVision doesn't start in a 15 seconds frame, then we abort!\n   However, the tool can still be enabled manually!)\n");
    console_log("- Under \"[Key_Settings]\" add your shortcut keys.\n");
    console_log("- (Read the examples in the \"3DVision_CM_Unleshed.ini\" file).\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("3. How to use it:\n\n");
    console_log("- Start this tool and leave it running (in the background).\n");
    console_log("- Start your game and WAIT till 3D Vision Compability Mode starts!\n");
    console_log("- Press \"CTRL + SHIFT + T\" to enable the tool!\n");
    console_log("  (This will Patch the driver and allow the keys to work).\n");
    console_log("  (You will hear the \"Plug\" sound for each fix that is applied,\n   followed by the \"TADA\" sound when everything is ready).\n");
    console_log("- Use the keys set (for Convergence and Separation) in the \"3DVision_CM_Unleshed.ini\" file and:\n\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("                    ENJOY \"UNLEASHED\" MODE!\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("\n");
    console_log("- You can modify the Convergence and Separation values, by using the Nvidia Shortcut keys.\n");
    console_log("- Press \"CTRL + SHIFT + HOME\" to print the current Separation Percentage & Convergence the driver is using.\n");
    console_log("  (Very useful, if you search the Convergence and Separation for a new game)!\n");
    console_log("- You can edit the \"3DVision_CM_Unleshed.ini\" file in real-time.\n");
    console_log("- Press \"CTRL + SHIFT + F10\" to reload the \"3DVision_CM_Unleshed.ini\" file and use the new key shortcuts!\n");
    console_log("\n");
    console_log("- Press \"CTRL + SHIFT + T\" again to disable the tool!\n");
    console_log("  (This will remove the driver modifications and restore it to default).\n");
    console_log("  (You will hear the \"Unplug\" sound if the operation succeeded).\n");
    console_log("-------------------------------------------------------------------\n\n");
    console_log("Options:\n");
    console_log("1. Go back to Main Menu.\n");
}
//-----------------------------------------------------------------------------

static void menukeyHandler()
{
    char key;
    static bool ignoreReturn = false;
    do
    {
        key = getchar();
        switch(key)
        {
            // 1 key
        case 0x31:
        {
            if(_mainMenu && !_infoMenu)
                showInfo();
            else
                showMenu();
            ignoreReturn = true;
        }
        break;
            // 2 key
        case 0x32:
        {
            system("start 3DVision_CM_Unleased.ini");
            console_log("");
            ignoreReturn = true;
        }
        break;
            // 3 key
        case 0x33:
        {
            ShellExecute(0, 0, "https://forums.geforce.com/default/topic/1097032/3d-vision/3d-vision-compatibility-mode-quot-unleashed-quot-/", 0, 0, SW_SHOW);
            console_log("");
            ignoreReturn = true;
        }
        break;
            // 4 key
        case 0x34:
        {
            ShellExecute(0, 0, "http://3dsurroundgaming.com/", 0, 0, SW_SHOW);
            console_log("");
            ignoreReturn = true;
        }
        break;

        case VK_SPACE:
        {
            showMenu();
            ignoreReturn = true;
        }
        break;

        default:
            if((key != VK_SPACE) && (key == 10) && !ignoreReturn)
                console_log("Unknown command?!\n");
            ignoreReturn = false;
            break;
        }
        // 5 key.
    } while((key != 0x35));

    printf("Have a nice day! :)\n");
    Sleep(2000);
}
///-------------------------------------------------------------------------------------------

int main()
{
    // Console SETUP
    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_HIDE);
    RECT consoleRect;
    GetWindowRect(console, &consoleRect);

    // Desktop handle and RECT
    HWND desktopHandle = GetDesktopWindow();
    RECT windowRect;
    GetWindowRect(desktopHandle, &windowRect);

    // Position
    consoleRect.right = 960;
    consoleRect.bottom = 640;
    consoleRect.left = windowRect.right / 2 - consoleRect.right / 2 - 150;
    consoleRect.top = windowRect.bottom / 2 - consoleRect.bottom / 2 - 65;
    MoveWindow(console, consoleRect.left, consoleRect.top, consoleRect.right, consoleRect.bottom, TRUE);

    // Console Font Size
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 16;  // Height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");  // Choose your font
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    // Console Colour
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);

    // Our Reader and Patcher
    g_reader = new ConfigReader();
    g_cmUnleashed = new CMUnleashed();

    // Our splash screen!
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    CSplashWnd splash;
    splash.Show();
    //Sleep(4000);
    splash.Hide();
    GdiplusShutdown(gdiplusToken);

    // Setup the profile if requireds
    if(g_reader->UpdateCMProfile())
    {
        _profileUpdateOK = NvApi_3DVisionProfileSetup(g_reader->GetGameExe(), g_reader->GetStereoTexture(), g_reader->GetCMProfile(), g_reader->GetCMConvergence(), g_reader->GetCMComments());
        if(!_profileUpdateOK)
        {
            PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
        }
    }

    // Show the menu
    showIntroMenu();

    // Show the console
    ShowWindow(console, SW_SHOW);

    // Start our Key handling thread
    std::thread keyThread(_KeyThread);
    keyThread.detach();

    // Auto-Start
    if(g_reader->AutoStartEnabled())
    {
        _autoStartStarted = true;
        std::thread autoStart(_AutoStart);
        autoStart.detach();
    }

    showMenu();
    menukeyHandler();

    // Cleanup
    if(_isPatchEnabled)
    {
        std::string gameExe = g_reader->GetGameExe();
        g_cmUnleashed->RestoreOriginal(gameExe);
    }

    if(g_xController)
        delete g_xController;

    if(g_reader)
        delete g_reader;

    if(g_cmUnleashed)
        delete g_cmUnleashed;
}
//-----------------------------------------------------------------------------
