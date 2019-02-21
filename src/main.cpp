/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

#include <iostream>
#include <Windows.h>
#include <sstream>
#include <conio.h>
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

// Our Profile Manager
ProfileLoader* g_profiles = nullptr;
// Our Reader
ConfigReader* g_reader = nullptr;
// Our Patcher
CMUnleashed* g_cmUnleashed = nullptr;

static bool _keyThreadRunning = true;

static bool _isPatchEnabled = false;
static bool _mainMenu = true;
static bool _infoMenu = false;
static bool _pidMonitorStarted = false;
static bool _autoStartStarted = false;
static bool _manuallyDisabled = false;

// Threads
static bool _exeThreadSingleInstance = false;
static void _ExeMonitorThread();
static bool _AutoThreadSingleInstance = false;
static void _AutoStart();
static void _KeyThread();

// GLOBAL PRINT

static std::string GetPath()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}
//-----------------------------------------------------------------------------------

void __cdecl console_log(const char* fmt, ...)
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

void __cdecl add_to_log(const char* fmt, ...)
{
    const size_t MAX_SIZE = 500000;
    va_list va_alist;
    char logbuf[MAX_SIZE] = "";

    va_start(va_alist, fmt);
    _vsnprintf_s(logbuf + strlen(logbuf), MAX_SIZE, sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
    va_end(va_alist);

    std::string path = GetPath();
    path += "/3DVision_CM_Unleashed.log";

    errno_t err;
    FILE* fp;
    err = fopen_s(&fp, path.c_str(), "ab");
    if(err == 0)
    {
        fprintf(fp, "%s", logbuf);
        fclose(fp);
    }
}
//-----------------------------------------------------------------------------

// use for hotkeys
static bool IsKeyDown(int keyCode);
static bool IsAltKeyToggleKeyDown(int keyCodeIndex);

// Xbox controller support
static CXBOXController* g_xController = new CXBOXController(1);
static bool IsXControllerAltKeyDown(int keyCodeIndex, int* foundKeyCode);
static bool IsXControllerAltKeyToggleKeyDownToggle(int keyCodeIndex);
//-----------------------------------------------------------------------------

static void _Run_Keys_OneTime(size_t keyIndex, size_t& returnIndex)
{
    // Toggled Enabled
    if(IsAltKeyToggleKeyDown((int)keyIndex) || IsXControllerAltKeyDown((int)keyIndex, nullptr))
    {
        // Convergence
        float temp = -1.0f;
        if(g_reader->GetAltConvergence((int)keyIndex, &temp))
        {
            // do it just once
            if(temp != -1.0f)
            {
                // Apply the new convergence
                g_cmUnleashed->CM_GetConvergence(&g_reader->_prevConvergence);
                g_cmUnleashed->CM_SetConvergence(&temp);
                g_reader->_isToggleConvergence = false;
                g_reader->_isPressAndHoldConvergence = false;
            }
        }

        // Separation
        if(g_reader->GetAltSeparation((int)keyIndex, &temp))
        {
            // do it just once
            if(temp != -1.0f)
            {
                g_reader->_prevSeparationFactor = temp;
                g_cmUnleashed->CM_SetSeparationFactor(&temp);
                g_reader->_isToggleSeparation = false;
                g_reader->_isPressAndHoldSeparation = false;
            }
        }

        // Force the index out of size in order to break the for() loop
        returnIndex = g_reader->GetNumberOfKeys();
    }
}
//-----------------------------------------------------------------------------

static void _Run_Keys_Hold(size_t keyIndex, size_t& returnIndex)
{
    /////////////////////////////////////
    // Push to hold settings
    // Only Separation & Convergence are handled
    /////////////////////////////////////
    int currentKeyCode = g_reader->GetKeyNumber((int)keyIndex);
    int prevState = g_reader->GetKeyPrevState((int)keyIndex);

    if(IsKeyDown(currentKeyCode) || IsXControllerAltKeyDown((int)keyIndex, &g_reader->_pressAndHoldKey))
    {
        if(!g_reader->_isPressAndHoldConvergence && !g_reader->_isToggleConvergence)
        {
            g_cmUnleashed->CM_GetConvergence(&g_reader->_prevConvergence);
        }

        // Custom Convergence
        float temp = -1.0f;
        if(g_reader->GetAltConvergence((int)keyIndex, &temp))
        {
            if(temp != -1.0f && g_reader->_prevConvergence != temp)
            {
                // Apply the new convergence
                g_cmUnleashed->CM_SetConvergence(&temp);
                g_reader->_pressAndHoldKey = currentKeyCode;
                g_reader->_isPressAndHoldConvergence = true;
            }
        }
        // Custom Separation
        if(g_reader->GetAltSeparation((int)keyIndex, &temp))
        {
            if(temp != -1.0f && g_reader->_prevSeparationFactor != temp)
            {
                // Apply the new separation
                if(!g_reader->_isPressAndHoldSeparation)
                    g_cmUnleashed->CM_GetSeparationFactor(&g_reader->_prevSeparationFactor);

                g_cmUnleashed->CM_SetSeparationFactor(&temp);
                g_reader->_pressAndHoldKey = currentKeyCode;
                g_reader->_isPressAndHoldSeparation = true;
            }
        }
        returnIndex = g_reader->GetNumberOfKeys();
    }
    else if((g_reader->_isPressAndHoldConvergence || g_reader->_isPressAndHoldSeparation) && g_reader->_pressAndHoldKey == currentKeyCode)
    {
        if(g_reader->_isPressAndHoldConvergence)
        {
            g_cmUnleashed->CM_SetConvergence(&g_reader->_prevConvergence);
            g_reader->_isPressAndHoldConvergence = false;
        }

        if(g_reader->_isPressAndHoldSeparation)
        {
            g_cmUnleashed->CM_SetSeparationFactor(&g_reader->_prevSeparationFactor);
            g_reader->_isPressAndHoldSeparation = false;
        }
        g_reader->SetKeyPrevState(g_reader->GetKeyNumber((int)keyIndex), 0);
        returnIndex = g_reader->GetNumberOfKeys();
    }
}
//-----------------------------------------------------------------------------

static void _Run_Keys_Toggle(size_t keyIndex, size_t& returnIndex)
{
    // Toggled Enabled
    if(IsAltKeyToggleKeyDown((int)keyIndex) || IsXControllerAltKeyToggleKeyDownToggle((int)keyIndex))
    {
        if(!g_reader->_isToggleConvergence && !g_reader->_isPressAndHoldConvergence)
        {
            g_cmUnleashed->CM_GetConvergence(&g_reader->_prevConvergence);
        }

        // Custom Convergence
        float temp = -1.0f;
        if(g_reader->GetAltConvergence((int)keyIndex, &temp))
        {
            if(temp != -1.0f && g_reader->_prevConvergence != temp)
            {
                if(!g_reader->_isToggleConvergence)
                {
                    // Apply the new convergence
                    g_cmUnleashed->CM_SetConvergence(&temp);
                    g_reader->_isToggleConvergence = true;
                }
                else if(g_reader->_isToggleConvergence)
                {
                    g_cmUnleashed->CM_SetConvergence(&g_reader->_prevConvergence);
                    g_reader->_isToggleConvergence = false;
                }
            }
        }

        // Separation
        if(g_reader->GetAltSeparation((int)keyIndex, &temp))
        {
            if(temp != -1.0f && g_reader->_prevSeparationFactor != temp)
            {
                if(!g_reader->_isToggleSeparation)
                {
                    // Apply the new separation
                    g_cmUnleashed->CM_SetSeparationFactor(&temp);
                    g_reader->_isToggleSeparation = true;
                }
                else if(g_reader->_isToggleSeparation)
                {
                    g_cmUnleashed->CM_SetSeparationFactor(&g_reader->_prevSeparationFactor);
                    g_reader->_isToggleSeparation = false;
                }
            }
        }
        // Force the index out of size in order to break the for() loop
        returnIndex = g_reader->GetNumberOfKeys();
    }
}
//-----------------------------------------------------------------------------

static void _ExeMonitorThread()
{
    if(!_exeThreadSingleInstance)
    {
        _exeThreadSingleInstance = true;
        printf("Starting Monitor Thread.\n");

        std::string exeName = g_reader->GetGameExe();
        while(g_cmUnleashed->GetExePid() && _pidMonitorStarted)
        {
            DWORD pid = g_cmUnleashed->getPid(exeName);
            g_cmUnleashed->UpdateExePid(pid);

            // Look every 2 secondS
            if(g_cmUnleashed->GetExePid())
                Sleep(2000);
        }

        // If we get here it means the application died!
        _exeThreadSingleInstance = false;
        g_cmUnleashed->RestoreOriginal(exeName);
        _pidMonitorStarted = false;
        _isPatchEnabled = false;
        printf("Monitor Thread Stopped.\n");

        // Restart the auto-start if enabled
        if(g_reader->AutoStartEnabled() && !_manuallyDisabled)
        {
            _autoStartStarted = true;
            std::thread autoStart(_AutoStart);
            autoStart.detach();
        }
    }
}
//-----------------------------------------------------------------------------

static void _AutoStart()
{
    if(!_AutoThreadSingleInstance)
    {
        _AutoThreadSingleInstance = true;
        bool exeFound = false;
        uint32_t delay = g_reader->GetAutoStartMs();

        if(g_reader->AutoStartEnabled())
            console_log("AutoStart Enabled. Looking for %s ...\n\n", g_reader->GetGameExe().c_str());
        while(!exeFound && _autoStartStarted)
        {
            std::string gameExeName = g_reader->GetGameExe();
            DWORD exePid = g_cmUnleashed->getPid(gameExeName);

            if(exePid != 0)
            {
                // Profile Update
                if(g_reader->UpdateCMProfile())
                {
                    bool profileUpdateOK = NvApi_3DVisionProfileSetup(g_reader->GetGameExe(), g_reader->GetStereoTexture(), g_reader->GetCMProfile(), g_reader->GetCMConvergence(), g_reader->GetCMComments());

                    if(!profileUpdateOK)
                    {
                        console_log("\n-----------------------------------------------------------------------------\n");
                        console_log("!!!Could not update the Nvidia Profile with the Compatibility Mode Values !!!\n");
                        console_log("Are you sure you are in \"RUN AS ADMIN\" Mode?!?!\n");
                        console_log("-----------------------------------------------------------------------------\n\n");
                    }
                    else
                    {
                        console_log("\n-----------------------------------------------------------------------------\n");
                        console_log("Nvidia Profile Updated with the CM flags. (You might need to restart the game!)\n");
                        console_log("-------------------------------------------------------------------------------\n\n");
                    }
                }

                exeFound = true;
                if(g_reader->AutoStartEnabled())
                {
                    console_log("%s found! Attempting to Enable in:", gameExeName.c_str());

                    while(delay)
                    {
                        printf(" %d", delay / 1000);
                        delay -= 1000;
                        Sleep(1000);
                    }
                    console_log("\n");

                    // Enable the Patching
                    _isPatchEnabled = g_cmUnleashed->DoPatching(gameExeName);
                }
                break;
            }
            else
                Sleep(1000);
        }
        _autoStartStarted = false;
        _AutoThreadSingleInstance = false;

        // Start our monitor thread
        _pidMonitorStarted = true;
        std::string exeName = g_reader->GetGameExe();
        DWORD pid = g_cmUnleashed->getPid(exeName);
        g_cmUnleashed->UpdateExePid(pid);
        std::thread pidMonitor(_ExeMonitorThread);
        pidMonitor.detach();
    }
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
                // Profile Update
                if(g_reader->UpdateCMProfile())
                {
                    bool profileUpdateOK = NvApi_3DVisionProfileSetup(g_reader->GetGameExe(), g_reader->GetStereoTexture(), g_reader->GetCMProfile(), g_reader->GetCMConvergence(), g_reader->GetCMComments());

                    if(!profileUpdateOK)
                    {
                        console_log("\n-----------------------------------------------------------------------------\n");
                        console_log("!!!Could not update the Nvidia Profile with the Compatibility Mode Values !!!\n");
                        console_log("Are you sure you are in \"RUN AS ADMIN\" Mode?!?!\n");
                        console_log("-----------------------------------------------------------------------------\n\n");
                    }
                    else
                    {
                        console_log("\n-----------------------------------------------------------------------------\n");
                        console_log("Nvidia Profile Updated with the CM flags. (You might need to restart the game!)\n");
                        console_log("-------------------------------------------------------------------------------\n\n");
                    }
                }

                // Do the patching
                _isPatchEnabled = g_cmUnleashed->DoPatching(gameExeName);

                // Start the monitor thread
                if(_isPatchEnabled)
                {
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
                _manuallyDisabled = true;
                _pidMonitorStarted = false;
                while(_pidMonitorStarted)
                    Sleep(100);
            }
        }
        else if(IsKeyDown(VK_CONTROL) && IsKeyDown(VK_SHIFT) && IsKeyDown(VK_HOME))
        {
            float conv = 0.0f;
            float sep = 0.0f;
            g_cmUnleashed->CM_GetConvergence(&conv);
            g_cmUnleashed->CM_GetSeparationFactor(&sep);

            console_log("-------------------------------------------------------------------\n");
            add_to_log("-------------------------------------------------------------------\n");
            if(sep)
            {
                console_log("Current Separation Percentage: %f\n", sep * 100);
                add_to_log("Current Separation Percentage: %f\n", sep * 100);
            }
            else
            {
                console_log("!!! Can not get current Separation Percentage !!!\n");
                add_to_log("!!! Can not get current Separation Percentage !!!\n");
            }
            if(conv)
            {
                console_log("Current Convergence: %f\n", conv);
                add_to_log("Current Convergence: %f\n", conv);
            }
            else
            {
                console_log("!!! Can not get current Convergence !!!\n");
                add_to_log("!!! Can not get current Convergence !!!\n");
            }
            console_log("-------------------------------------------------------------------\n\n");
            add_to_log("-------------------------------------------------------------------\n\n");

            PlaySound(TEXT("MessageNudge"), NULL, SND_ALIAS | SND_ASYNC);

            Sleep(300);
        }
        else if(IsKeyDown(VK_CONTROL) && IsKeyDown(VK_SHIFT) && IsKeyDown(VK_F10))
        {
            console_log("-------------------------------------------------------------------\n");
            console_log("\"%s\" file successfully read!\n", g_profiles->GetCurrentProfile().c_str());
            console_log("-------------------------------------------------------------------\n\n");

            // Re-trigger the config reader
            if(g_reader)
            {
                _autoStartStarted = false;
                while(_autoStartStarted)
                    Sleep(100);

                // Disable the Patching (if applied)
                _isPatchEnabled = false;
                std::string gameExeName = g_reader->GetGameExe();
                g_cmUnleashed->RestoreOriginal(gameExeName);

                // stop the monitor
                _pidMonitorStarted = false;
                while(_pidMonitorStarted)
                    Sleep(100);

                delete g_reader;
                g_reader = new ConfigReader(g_profiles->GetCurrentProfile());
                _manuallyDisabled = false;
                PlaySound(TEXT("DeviceConnect"), NULL, SND_ALIAS | SND_ASYNC);
                g_reader->SetAutoStartEnabled(true);
                g_reader->SetAutoStartMs(2000);

                _autoStartStarted = true;
                std::thread autoStart(_AutoStart);
                autoStart.detach();
            }

            Sleep(300);
        }
        else if(IsKeyDown(VK_CONTROL) && IsKeyDown(VK_SHIFT) && IsKeyDown(VK_OEM_PLUS))
        {
            g_reader->_prevSeparationFactor += 0.05f;
            g_cmUnleashed->CM_SetSeparationFactor(&g_reader->_prevSeparationFactor);
        }
        else if(IsKeyDown(VK_CONTROL) && IsKeyDown(VK_SHIFT) && IsKeyDown(VK_OEM_MINUS))
        {
            if(g_reader->_prevSeparationFactor - 0.05f > 0.0f)
            {
                g_reader->_prevSeparationFactor -= 0.05f;
                g_cmUnleashed->CM_SetSeparationFactor(&g_reader->_prevSeparationFactor);
            }
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
    return (GetKeyState(keyCode) & 0x0080) != 0;
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
    else if((g_xController->GetState().Gamepad.wButtons != currentKey) && (currentKey != CUSTOM_XINPUT_GAMEPAD_LEFT_TRIGGER) && (currentKey != CUSTOM_XINPUT_GAMEPAD_RIGHT_TRIGGER))
        g_reader->SetKeyPrevState(keyCodeIndex, state);

    return ret;
}
//-----------------------------------------------------------------------------

// XBOX
static bool IsXControllerAltKeyDown(int keyCodeIndex, int* foundKeyCode)
{
    bool state = 0;

    int currentKey = g_reader->GetKeyNumber(keyCodeIndex);
    if(
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_UP) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_DOWN) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_LEFT) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_RIGHT) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_START) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_BACK) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_LEFT_THUMB) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_RIGHT_THUMB) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_LEFT_SHOULDER) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_A) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_B) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_X) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_Y) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_LEFT_TRIGGER) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_RIGHT_TRIGGER))
    {
        if((g_xController->GetState().Gamepad.bLeftTrigger >= 128 && currentKey == CUSTOM_XINPUT_GAMEPAD_LEFT_TRIGGER) ||
            (g_xController->GetState().Gamepad.bRightTrigger >= 128 && currentKey == CUSTOM_XINPUT_GAMEPAD_RIGHT_TRIGGER))
        {
            if(foundKeyCode)
                *foundKeyCode = currentKey;
            return true;
        }
        else if(g_xController->GetState().Gamepad.wButtons == GetXInputRealKey(currentKey))
        {
            if(foundKeyCode)
                *foundKeyCode = currentKey;
            return true;
        }
    }
    return false;
}
//--------------------------------------------------------------------------------------

static bool IsXControllerAltKeyToggleKeyDownToggle(int keyCodeIndex)
{
    bool ret = false;
    int currentKey = g_reader->GetKeyNumber(keyCodeIndex);
    if(
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_UP) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_DOWN) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_LEFT) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_DPAD_RIGHT) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_START) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_BACK) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_LEFT_THUMB) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_RIGHT_THUMB) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_LEFT_SHOULDER) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_A) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_B) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_X) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_Y) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_LEFT_TRIGGER) ||
        (currentKey == CUSTOM_XINPUT_GAMEPAD_RIGHT_TRIGGER))
    {
        //Return if the high byte is true (ie key is down)
        int state = (g_xController->GetState().Gamepad.wButtons & g_reader->GetKeyNumber(keyCodeIndex));
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
    console_log("| Ver: 1.0.19                                                       |\n");
    console_log("| Developed by: Helifax (2019)                                      |\n");
    console_log("| If you would like to donate you can do it at: tavyhome@gmail.com  |\n");
    console_log("|                                                                   |\n");
    console_log("| Supports 32-bits(x86) and 64-bits(x64) Applications!              |\n");
    console_log("| - Windows 7 and above (x64 only) -                                |\n");
    console_log("---------------------------------------------------------------------\n\n");
    console_log("Important:\n");
    console_log("- Always \"RUN AS ADMIN\" !\n");
    console_log("- Don't forget to change the Frustum (CTRL + F11) to un-stretch the image, for BEST RESULTS!\n");
    console_log("- To print this information again, press \"BACKSPACE\".\n");
    console_log("-------------------------------------------------------------------\n\n");
}
//-----------------------------------------------------------------------------

static void showProfileSelection()
{
    std::vector<std::string> allProfiles = g_profiles->GetAllProfiles();
    console_log("Please select one of the following Profiles:\n\n");

    for(auto idx = 0; idx < allProfiles.size(); idx++)
    {
        console_log("%d. %s\n", idx, allProfiles[idx].c_str());
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
    console_log("2. Select a different profile.\n");
    console_log("3. Open \"%s\" file.\n", g_profiles->GetCurrentProfile().c_str());
    console_log("4. Looking for an update? Check here! (opens Web-page)\n");
    console_log("5. Do you want to see screenshots? Or to discuss? Let's do it on GeForce Forums thread! (opens Web-page)\n");
    console_log("6. Quit.\n\n");
    console_log("Awaiting commands :)\n");
}
//-----------------------------------------------------------------------------

static void showInfo()
{
    _mainMenu = false;
    _infoMenu = true;
    system("CLS");
    console_log("-------------------------------------------------------------------\n");
    console_log("1. What does this tool do:\n\n");
    console_log("- Patches in Real-Time(Code Injection) the UDM Nvidia Driver responsible for 3D Vision Compatibility Mode\n  (Stereo Reprojection based on the Depth Buffer).\n");
    console_log("- Removes the limit for Stereo Separation (Depth Override).\n");
    console_log("- Adds key shortcuts to quickly jump from one Separation value to another.\n");
    console_log("- Adds key shortcuts to quickly jump from one Convergence value to another.\n");
    console_log("- DOESN'T MODIFY the EXE of the application/game! (It should be safe, with Anti-Cheat programs)!\n");
    console_log("  (However, I still recommend being cautions, if you want to use it in online multiplayer games!)\n");
    console_log("  Allows updating the 3D Vision Compatibility Profile (for the application/game).\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("\n");

    // Simulates "Press any key to continue..." but without the text
    _getch();

    console_log("-------------------------------------------------------------------\n");
    console_log("2. (Optional) Add or Update 3D Vision Compatibility Mode:\n\n");
    console_log("   - Open \"3DVision_CM_Unleshed.ini\" file.\n");
    console_log("   - Change \"GameExecutable\" for the game EXE you want to play.\n");
    console_log("   - Enable \"2DDEnableUpdates\".\n");
    console_log("   - Set \"StereoTexture\" value.\n");
    console_log("   - Set \"2DDHUDSettings\" value.\n");
    console_log("   - Set \"2DDConvergence\" value, if you wish. This is the default startup value for CM.\n");
    console_log("   - Set \"2DD_Notes\" value, if you wish. It will be displayed in the Nvidia 3D Vision Overlay.\n");
    console_log("   - (Read the comments in the \"3DVision_CM_Unleshed.ini\" file, for more info).\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("\n");

    _getch();

    console_log("-------------------------------------------------------------------\n");
    console_log("3. How to add your Custom Separation and Convergence:\n\n");
    console_log("- Open \"3DVision_CM_Unleshed.ini\" file.\n");
    console_log("- Change \"GameExecutable\" for the game EXE you want to play.\n");
    console_log("  (Make sure there are no spaces and the name is in quotes: GameExecutable=\"re2.exe\").\n\n");
    console_log("   - Optionally: (Set \"AutoStart\" to true, if you want to attempt to automatically start the Patching once the EXE is detected!)\n");
    console_log("   - Optionally: (Set \"AutoStartDelay\" to a value (in seconds), that it takes your game from starting the EXE until it renders.)\n\n");
    console_log("- Under \"[Key_Settings]\" add your shortcut keys using this pattern:\n\n");
    console_log("   - \"NewKey(key_code, separation, convergence, type)\".\n");
    console_log("- (See the examples and documentation in the \"3DVision_CM_Unleshed.ini\" file).\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("\n");

    _getch();

    console_log("-------------------------------------------------------------------\n");
    console_log("4. How to use it:\n\n");
    console_log("- Start this tool and leave it running (in the background).\n");
    console_log("- Start your game and WAIT till 3D Vision Compatibility Mode starts!\n");
    console_log("- Press \"CTRL + SHIFT + T\" to enable the tool!\n");
    console_log("  (This will Patch the driver and allow the keys to work).\n");
    console_log("  (You will hear the \"Plug\" sound for each fix that is applied,\n   followed by the \"TADA\" sound when everything is ready).\n");
    console_log("- Use the keys set (for Convergence and Separation) in the \"3DVision_CM_Unleshed.ini\" file and:\n\n");

    _getch();
    console_log("-------------------------------------------------------------------\n");
    console_log("                    ENJOY \"UNLEASHED\" MODE!\n");
    console_log("-------------------------------------------------------------------\n");
    _getch();

    console_log("\n");
    console_log("- You can modify the Convergence and Separation values, by using the Nvidia Shortcut keys.\n");
    console_log("- Press \"CTRL + SHIFT + PLUS (regular, not numpad)\" to increase Separation factor by 5%%.\n");
    console_log("- Press \"CTRL + SHIFT + MINUS(regular, not numpad)\" to decrease Separation factor by 5%%.\n");
    console_log("- Press \"CTRL + SHIFT + HOME\" to print the current Separation Percentage & Convergence the driver is using.\n");
    console_log("  (Very useful, if you search the Convergence and Separation for a new game)!\n");
    console_log("- You can edit the \"3DVision_CM_Unleshed.ini\" file in real-time.\n");
    console_log("- Press \"CTRL + SHIFT + F10\" to reload the \"3DVision_CM_Unleshed.ini\" file and use the new key shortcuts!\n");

    _getch();

    console_log("\n");
    console_log("- Press \"CTRL + SHIFT + T\" again to disable the tool!\n");
    console_log("  (This will remove the driver modifications and restore it to default).\n");
    console_log("  (You will hear the \"Unplug\" sound if the operation succeeded).\n");
    console_log("-------------------------------------------------------------------\n");
    console_log("\n");

    _getch();

    console_log("\n");
    console_log("Options:\n");
    console_log("1. Go back to Main Menu.\n");
}
//-----------------------------------------------------------------------------

static void profileKeyHandler()
{
    bool stop = false;
    std::vector<std::string> allProfiles = g_profiles->GetAllProfiles();
    int profileNumber;

    while(!stop)
    {
        if(allProfiles.size() == 0)
        {
            console_log("No profiles found in \"Profiles\" folder!\n\n");
            Sleep(1000);
        }

        std::cin >> profileNumber;
        if(profileNumber < allProfiles.size())
        {
            g_profiles->SetCurrentProfileName(allProfiles[profileNumber]);
            stop = true;
            console_log("Loading Profile: %s", g_profiles->GetCurrentProfile().c_str());
            Sleep(1000);
        }
        else
        {
            console_log("%d is not a valid Profile number. Please try again!\n", profileNumber);
        }
    }
}
//-------------------------------------------------------------------------------------------

static void menukeyHandler()
{
    char key;
    do
    {
        key = _getch();
        switch(key)
        {
            // 1 key
        case 0x31:
        {
            if(_mainMenu && !_infoMenu)
                showInfo();
            else
                showMenu();
        }
        break;
        // 2 key
        case 0x32:
        {
            showProfileSelection();
            profileKeyHandler();

            if(g_reader)
                delete g_reader;
            g_reader = new ConfigReader(g_profiles->GetCurrentProfile());

            showMenu();
        }
        break;
        // 3 key
        case 0x33:
        {
            if(_mainMenu)
            {
                std::string iniFile = GetPath() + "\\Profiles\\" + g_profiles->GetCurrentProfile();
                iniFile = "\"" + iniFile + "\"";
                ShellExecute(0, 0, iniFile.c_str(), 0, 0, SW_SHOW);
            }
        }
        break;
            // 4 key
        case 0x34:
        {
            if(_mainMenu)
            {
                ShellExecute(0, 0, "http://3dsurroundgaming.com/CMUnleashed.html", 0, 0, SW_SHOW);
            }
        }
        break;
            // 5 key
        case 0x35:
        {
            if(_mainMenu)
            {
                ShellExecute(0, 0, "https://forums.geforce.com/default/topic/1097032/3d-vision/3d-vision-compatibility-mode-quot-unleashed-quot-/", 0, 0, SW_SHOW);
            }
        }
        break;

        case VK_BACK:
        {
            showMenu();
        }
        break;

        // New Line
        case 0x0d:
            console_log("\n");
            break;

        default:
            //if(key != VK_BACK || key != VK_HOME || key != 'T' || key != VK_F10)
            //console_log("Unknown command?!\n");
            break;
        }
        // 6 key.
    } while((key != 0x36) || _infoMenu);

    printf("\n-------------------\n");
    printf("Have a nice day! :)\n");
    printf("-------------------\n");
    Sleep(2000);
}
//-------------------------------------------------------------------------------------------

int main()
{
    // Console SETUP
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    RECT consoleRect;
    GetWindowRect(GetConsoleWindow(), &consoleRect);

    // Desktop handle and RECT
    HWND desktopHandle = GetDesktopWindow();
    RECT windowRect;
    GetWindowRect(desktopHandle, &windowRect);

    // Position
    consoleRect.right = 960;
    consoleRect.bottom = 640;
    consoleRect.left = windowRect.right / 2 - consoleRect.right / 2 - 150;
    consoleRect.top = windowRect.bottom / 2 - consoleRect.bottom / 2 - 65;
    MoveWindow(GetConsoleWindow(), consoleRect.left, consoleRect.top, consoleRect.right, consoleRect.bottom, TRUE);

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

    // Our splash screen!
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    CSplashWnd splash;
    splash.Show();
    Sleep(4000);
    splash.Hide();
    GdiplusShutdown(gdiplusToken);

    // Show the menu
    showIntroMenu();

    // Show the console
    ShowWindow(GetConsoleWindow(), SW_SHOW);

    // Read all Profiles
    g_profiles = new ProfileLoader();
    // Profile Selection
    showProfileSelection();
    profileKeyHandler();

    // Show the Menu again
    showIntroMenu();

    // Our Reader and Patcher
    g_reader = new ConfigReader(g_profiles->GetCurrentProfile());
    g_cmUnleashed = new CMUnleashed();

    // Start our Key handling thread
    std::thread keyThread(_KeyThread);
    keyThread.detach();

    // Auto-Start & Profile Update
    _autoStartStarted = true;
    std::thread autoStart(_AutoStart);
    autoStart.detach();

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
