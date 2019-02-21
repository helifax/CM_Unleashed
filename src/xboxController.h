/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

#ifndef __XBOX_CONTROLLER__H_
#define __XBOX_CONTROLLER__H_

#include "windows.h"
#include "XInput.h"

#define CUSTOM_XINPUT_GAMEPAD_DPAD_UP 0xFFF1
#define CUSTOM_XINPUT_GAMEPAD_DPAD_DOWN 0xFFF2
#define CUSTOM_XINPUT_GAMEPAD_DPAD_LEFT 0xFFF4
#define CUSTOM_XINPUT_GAMEPAD_DPAD_RIGHT 0xFFF8
#define CUSTOM_XINPUT_GAMEPAD_START 0xFF1F
#define CUSTOM_XINPUT_GAMEPAD_BACK 0xFF2F
#define CUSTOM_XINPUT_GAMEPAD_LEFT_THUMB 0xFF4F
#define CUSTOM_XINPUT_GAMEPAD_RIGHT_THUMB 0xFF8F
#define CUSTOM_XINPUT_GAMEPAD_LEFT_SHOULDER 0xF1FF
#define CUSTOM_XINPUT_GAMEPAD_RIGHT_SHOULDER 0xF2FF
#define CUSTOM_XINPUT_GAMEPAD_A 0x1FFF
#define CUSTOM_XINPUT_GAMEPAD_B 0x2FFF
#define CUSTOM_XINPUT_GAMEPAD_X 0x4FFF
#define CUSTOM_XINPUT_GAMEPAD_Y 0x8FFF
#define CUSTOM_XINPUT_GAMEPAD_LEFT_TRIGGER 0x1111
#define CUSTOM_XINPUT_GAMEPAD_RIGHT_TRIGGER 0x2222

int32_t GetXInputRealKey(int keyCode)
{
    switch(keyCode)
    {
    case CUSTOM_XINPUT_GAMEPAD_DPAD_UP:
        return 0x0001;
    case CUSTOM_XINPUT_GAMEPAD_DPAD_DOWN:
        return 0x0002;
    case CUSTOM_XINPUT_GAMEPAD_DPAD_LEFT:
        return 0x0004;
    case CUSTOM_XINPUT_GAMEPAD_DPAD_RIGHT:
        return 0x0008;
    case CUSTOM_XINPUT_GAMEPAD_START:
        return 0x0010;
    case CUSTOM_XINPUT_GAMEPAD_BACK:
        return 0x0020;
    case CUSTOM_XINPUT_GAMEPAD_LEFT_THUMB:
        return 0x0040;
    case CUSTOM_XINPUT_GAMEPAD_RIGHT_THUMB:
        return 0x0080;
    case CUSTOM_XINPUT_GAMEPAD_LEFT_SHOULDER:
        return 0x0100;
    case CUSTOM_XINPUT_GAMEPAD_RIGHT_SHOULDER:
        return 0x0200;
    case CUSTOM_XINPUT_GAMEPAD_A:
        return 0x1000;
    case CUSTOM_XINPUT_GAMEPAD_B:
        return 0x2000;
    case CUSTOM_XINPUT_GAMEPAD_X:
        return 0x4000;
    case CUSTOM_XINPUT_GAMEPAD_Y:
        return 0x8000;
    case CUSTOM_XINPUT_GAMEPAD_LEFT_TRIGGER:
        return 0x1111;
    case CUSTOM_XINPUT_GAMEPAD_RIGHT_TRIGGER:
        return 0x2222;
    default:
        return 0x0000;
        break;
    }
}

// XBOX Controller Class Definition
class CXBOXController
{
private:
    XINPUT_STATE _controllerState;
    int _controllerNum;

public:
    CXBOXController(int playerNumber)
    {
        // Set the Controller Number
        _controllerNum = playerNumber - 1;
    }
    //------------------------------------------------------------------------->

    XINPUT_STATE GetState()
    {
        // Zeroise the state
        ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

        // Get the state
        XInputGetState(_controllerNum, &_controllerState);

        return _controllerState;
    }
    //------------------------------------------------------------------------->

    bool IsConnected()
    {
        // Zeroise the state
        ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

        // Get the state
        DWORD Result = XInputGetState(_controllerNum, &_controllerState);

        if(Result == ERROR_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    //------------------------------------------------------------------------->
};

#endif