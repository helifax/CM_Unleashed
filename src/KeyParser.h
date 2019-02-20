
/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
* Taken from 3DMigoto : https://github.com/bo3b/3Dmigoto/releases
*/

#pragma once

#include <string>
#include <winnt.h>

template <class T1, class T2>
struct EnumName_t
{
    T1 name;
    T2 val;
};
//-----------------------------------------------------------------------------

// http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
static EnumName_t<const char *, int> VKMappings[] = {
    { "LBUTTON", 0x01 },
    { "RBUTTON", 0x02 },
    { "CANCE", 0x03 },
    { "MBUTTON", 0x04 },
    { "XBUTTON1", 0x05 },
    { "XBUTTON2", 0x06 },
    { "BACK", 0x08 },
    { "BACKSPACE", 0x08 },
    { "BACK_SPACE", 0x08 },
    { "TAB", 0x09 },
    { "CLEAR", 0x0C },
    { "RETURN", 0x0D },
    { "ENTER", 0x0D },
    { "SHIFT", 0x10 },
    { "CONTRO", 0x11 },
    { "CTR", 0x11 },
    { "MENU", 0x12 },
    { "ALT", 0x12 },
    { "PAUSE", 0x13 },
    { "CAPITA", 0x14 },
    { "CAPS", 0x14 },
    { "CAPSLOCK", 0x14 },
    { "CAPS_LOCK", 0x14 },
    { "KANA", 0x15 },
    { "HANGUE", 0x15 },
    { "HANGU", 0x15 },
    { "JUNJA", 0x17 },
    { "FINA", 0x18 },
    { "HANJA", 0x19 },
    { "KANJI", 0x19 },
    { "ESCAPE", 0x1B },
    { "CONVERT", 0x1C },
    { "NONCONVERT", 0x1D },
    { "ACCEPT", 0x1E },
    { "MODECHANGE", 0x1F },
    { "SPACE", 0x20 },
    { "PRIOR", 0x21 },
    { "PGUP", 0x21 },
    { "PAGEUP", 0x21 },
    { "PAGE_UP", 0x21 },
    { "NEXT", 0x22 },
    { "PGDN", 0x22 },
    { "PAGEDOWN", 0x22 },
    { "PAGE_DOWN", 0x22 },
    { "END", 0x23 },
    { "HOME", 0x24 },
    { "LEFT", 0x25 },
    { "UP", 0x26 },
    { "RIGHT", 0x27 },
    { "DOWN", 0x28 },
    { "SELECT", 0x29 },
    { "PRINT", 0x2A },
    { "EXECUTE", 0x2B },
    { "SNAPSHOT", 0x2C },
    { "PRSCR", 0x2C },
    { "PRINTSCREEN", 0x2C },
    { "PRINT_SCREEN", 0x2C },
    { "INSERT", 0x2D },
    { "DELETE", 0x2E },
    { "HELP", 0x2F },
    /* 0-9 & upper case A-Z match ASCII and are checked programatically */
    { "LWIN", 0x5B },
    { "LEFT_WIN", 0x5B },
    { "LEFT_WINDOWS", 0x5B },
    { "RWIN", 0x5C },
    { "RIGHT_WIN", 0x5C },
    { "RIGHT_WINDOWS", 0x5C },
    { "APPS", 0x5D },
    { "SLEEP", 0x5F },
    { "NUMPAD0", 0x60 },
    { "NUMPAD1", 0x61 },
    { "NUMPAD2", 0x62 },
    { "NUMPAD3", 0x63 },
    { "NUMPAD4", 0x64 },
    { "NUMPAD5", 0x65 },
    { "NUMPAD6", 0x66 },
    { "NUMPAD7", 0x67 },
    { "NUMPAD8", 0x68 },
    { "NUMPAD9", 0x69 },
    { "MULTIPLY", 0x6A },
    { "ADD", 0x6B },
    { "SEPARATOR", 0x6C },
    { "SUBTRACT", 0x6D },
    { "DECIMA", 0x6E },
    { "DIVIDE", 0x6F },
    { "F1", 0x70 },
    { "F2", 0x71 },
    { "F3", 0x72 },
    { "F4", 0x73 },
    { "F5", 0x74 },
    { "F6", 0x75 },
    { "F7", 0x76 },
    { "F8", 0x77 },
    { "F9", 0x78 },
    { "F10", 0x79 },
    { "F11", 0x7A },
    { "F12", 0x7B },
    { "F13", 0x7C },
    { "F14", 0x7D },
    { "F15", 0x7E },
    { "F16", 0x7F },
    { "F17", 0x80 },
    { "F18", 0x81 },
    { "F19", 0x82 },
    { "F20", 0x83 },
    { "F21", 0x84 },
    { "F22", 0x85 },
    { "F23", 0x86 },
    { "F24", 0x87 },
    { "NUMLOCK", 0x90 },
    { "SCROL", 0x91 },
    { "LSHIFT", 0xA0 },
    { "LEFT_SHIFT", 0xA0 },
    { "RSHIFT", 0xA1 },
    { "RIGHT_SHIFT", 0xA1 },
    { "LCONTRO", 0xA2 },
    { "LEFT_CONTRO", 0xA2 },
    { "LCTR", 0xA2 },
    { "LEFT_CTR", 0xA2 },
    { "RCONTRO", 0xA3 },
    { "RIGHT_CONTRO", 0xA3 },
    { "RCTR", 0xA3 },
    { "RIGHT_CTR", 0xA3 },
    { "LMENU", 0xA4 },
    { "LEFT_MENU", 0xA4 },
    { "LALT", 0xA4 },
    { "LEFT_ALT", 0xA4 },
    { "RMENU", 0xA5 },
    { "RIGHT_MENU", 0xA5 },
    { "RALT", 0xA5 },
    { "RIGHT_ALT", 0xA5 },
    { "BROWSER_BACK", 0xA6 },
    { "BROWSER_FORWARD", 0xA7 },
    { "BROWSER_REFRESH", 0xA8 },
    { "BROWSER_STOP", 0xA9 },
    { "BROWSER_SEARCH", 0xAA },
    { "BROWSER_FAVORITES", 0xAB },
    { "BROWSER_HOME", 0xAC },
    { "VOLUME_MUTE", 0xAD },
    { "VOLUME_DOWN", 0xAE },
    { "VOLUME_UP", 0xAF },
    { "MEDIA_NEXT_TRACK", 0xB0 },
    { "MEDIA_PREV_TRACK", 0xB1 },
    { "MEDIA_STOP", 0xB2 },
    { "MEDIA_PLAY_PAUSE", 0xB3 },
    { "LAUNCH_MAI", 0xB4 },
    { "LAUNCH_MEDIA_SELECT", 0xB5 },
    { "LAUNCH_APP1", 0xB6 },
    { "LAUNCH_APP2", 0xB7 },
    { "OEM_1", 0xBA },
    { ";", 0xBA },
    { ":", 0xBA },
    { "COLON", 0xBA },
    { "SEMICOLON", 0xBA },
    { "SEMI_COLON", 0xBA },
    { "OEM_PLUS", 0xBB },
    { "=", 0xBB },
    { "PLUS", 0xBB },
    { "EQUALS", 0xBB }, /* "+" alias already used for numpad + */
    { "OEM_COMMA", 0xBC },
    { ",", 0xBC },
    { "<", 0xBC },
    { "COMMA", 0xBC },
    { "OEM_MINUS", 0xBD },
    { "MINUS", 0xBD },
    { "UNDERSCORE", 0xBD },
    { "_", 0xBD }, /* "-" alias already used for numpad - */
    { "OEM_PERIOD", 0xBE },
    { ".", 0xBE },
    { ">", 0xBE },
    { "PERIOD", 0xBE },
    { "OEM_2", 0xBF },
    { "/", 0xBF },
    { "?", 0xBF },
    { "SLASH", 0xBF },
    { "FORWARD_SLASH", 0xBF },
    { "QUESTION", 0xBF },
    { "QUESTION_MARK", 0xBF },
    { "OEM_3", 0xC0 },
    { "`", 0xC0 },
    { "~", 0xC0 },
    { "TILDE", 0xC0 },
    { "GRAVE", 0xC0 },
    { "OEM_4", 0xDB },
    { "[", 0xDB },
    { "{", 0xDB },
    { "OEM_5", 0xDC },
    { "\\", 0xDC },
    { "|", 0xDC },
    { "BACKSLASH", 0xDC },
    { "BACK_SLASH", 0xDC },
    { "PIPE", 0xDC },
    { "VERTICAL_BAR", 0xDC },
    { "OEM_6", 0xDD },
    { "]", 0xDD },
    { "}", 0xDD },
    { "OEM_7", 0xDE },
    { "'", 0xDE },
    { "\"", 0xDE },
    { "QUOTE", 0xDE },
    { "DOUBLE_QUOTE", 0xDE },
    { "OEM_8", 0xDF },
    { "OEM_102", 0xE2 }, /* Either the angle bracket key or the backslash key on the RT 102-key keyboard */
    { "PROCESSKEY", 0xE5 },
    /* {"PACKET", 0xE7}, Would need special handling for unicode characters */
    { "ATTN", 0xF6 },
    { "CRSE", 0xF7 },
    { "EXSE", 0xF8 },
    { "EREOF", 0xF9 },
    { "PLAY", 0xFA },
    { "ZOOM", 0xFB },
    { "NONAME", 0xFC },
    { "PA1", 0xFD },
    { "OEM_CLEAR", 0xFE },

    // XBOX STUFF
    { "XINPUT_GAMEPAD_DPAD_UP", 0x0001 },
    { "XINPUT_GAMEPAD_DPAD_DOWN", 0x0002 },
    { "XINPUT_GAMEPAD_DPAD_LEFT", 0x0004 },
    { "XINPUT_GAMEPAD_DPAD_RIGHT", 0x0008 },
    { "XINPUT_GAMEPAD_START", 0x0010 },
    { "XINPUT_GAMEPAD_BACK", 0x0020 },
    { "XINPUT_GAMEPAD_LEFT_THUMB", 0x0040 },
    { "XINPUT_GAMEPAD_RIGHT_THUMB", 0x0080 },
    { "XINPUT_GAMEPAD_LEFT_SHOULDER", 0x0100 },
    { "XINPUT_GAMEPAD_RIGHT_SHOULDER", 0x0200 },
    { "XINPUT_GAMEPAD_A", 0x1000 },
    { "XINPUT_GAMEPAD_B", 0x2000 },
    { "XINPUT_GAMEPAD_X", 0x4000 },
    { "XINPUT_GAMEPAD_Y", 0x8000 },
    { "XINPUT_GAMEPAD_LEFT_TRIGGER", 0x1111 },
    { "XINPUT_GAMEPAD_RIGHT_TRIGGER", 0x2222 },
};
//-----------------------------------------------------------------------------

static int ParseVKey(const char *name)
{
    int i;

    if(strlen(name) == 1)
    {
        wchar_t c = towupper(name[0]);
        if((c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'Z'))
            return c;
    }

    if(!strncmp(name, "0x", 2))
    {
        unsigned int vkey;
        sscanf_s(name, "%x", &vkey);
        return vkey;
    }

    if(!_strnicmp(name, "VK_", 3))
        name += 3;

    for(i = 0; i < ARRAYSIZE(VKMappings); i++)
    {
        if(!_stricmp(name, VKMappings[i].name))
            return VKMappings[i].val;
    }

    return -1;
}
//-----------------------------------------------------------------------------

// Reverse lookup of key back to string name
static std::string GetKeyName(int key)
{
    for(int i = 0; i < ARRAYSIZE(VKMappings); i++)
    {
        if(VKMappings[i].val == key)
        {
            return (VKMappings[i].name);
        }
    }
    return ("missing");
}
//-----------------------------------------------------------------------------
