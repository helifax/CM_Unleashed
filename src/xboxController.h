/*
* Copyright (c) Helifax 2019
*/

#ifndef __XBOX_CONTROLLER__H_
#define __XBOX_CONTROLLER__H_

#include "windows.h"
#include "XInput.h"

#define XINPUT_GAMEPAD_DPAD_UP 0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN 0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT 0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT 0x0008
#define XINPUT_GAMEPAD_START 0x0010
#define XINPUT_GAMEPAD_BACK 0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB 0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB 0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER 0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER 0x0200
#define XINPUT_GAMEPAD_A 0x1000
#define XINPUT_GAMEPAD_B 0x2000
#define XINPUT_GAMEPAD_X 0x4000
#define XINPUT_GAMEPAD_Y 0x8000

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