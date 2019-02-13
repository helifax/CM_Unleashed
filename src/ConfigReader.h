/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

#pragma once

#include <vector>

typedef enum
{
    ALT_KEYS_MODE_ONE_TIME = 10,
    ALT_KEYS_MODE_HOLD = 20,
    ALT_KEYS_MODE_TOGGLE = 30,
    ALT_KEYS_MODE_NONE = 0
} ALT_KEYS_MODE_T;

typedef struct
{
    int altKeyCode;
    float altConvergence;
    float altSeparation;
    int keyState;
    int keyPrevState;
    ALT_KEYS_MODE_T keyMode;
} ALT_KEYS_T;

class ConfigReader
{
public:
    //ctor
    ConfigReader();
    ~ConfigReader()
    {
    }
    // ---------------------------------------------------------------------------------------------

    int GetKeyState(unsigned int index)
    {
        if(index < _altKeys.size())
            return _altKeys[index].keyState;
        else
            return 0;
    }
    // ---------------------------------------------------------------------------------------------

    void SetKeyState(unsigned int index, int state)
    {
        if(index < _altKeys.size())
            _altKeys[index].keyState = state;
    }
    // ---------------------------------------------------------------------------------------------

    int GetKeyPrevState(unsigned int index)
    {
        if(index < _altKeys.size())
            return _altKeys[index].keyPrevState;
        else
            return 0;
    }
    // ---------------------------------------------------------------------------------------------

    void SetKeyPrevState(unsigned int index, int state)
    {
        if(index < _altKeys.size())
            _altKeys[index].keyPrevState = state;
    }
    // ---------------------------------------------------------------------------------------------

    ALT_KEYS_MODE_T GetKeyMode(size_t index)
    {
        if(index < _altKeys.size())
            return _altKeys[index].keyMode;
        else
            return ALT_KEYS_MODE_NONE;
    }
    // ---------------------------------------------------------------------------------------------

    size_t GetNumberOfKeys()
    {
        return _altKeys.size();
    }
    // ---------------------------------------------------------------------------------------------

    int GetKeyNumber(unsigned int index)
    {
        if(index < _altKeys.size())
            return _altKeys[index].altKeyCode;
        else
            return -1;
    }
    // ---------------------------------------------------------------------------------------------

    bool GetAltConvergence(unsigned int index, float *retValue)
    {
        if(index < _altKeys.size())
        {
            *retValue = _altKeys[index].altConvergence;
            return true;
        }
        else
            return false;
    }
    // ---------------------------------------------------------------------------------------------

    bool GetAltSeparation(unsigned int index, float *retValue)
    {
        if(index < _altKeys.size())
        {
            *retValue = _altKeys[index].altSeparation;
            return true;
        }
        else
            return false;
    }
    // ---------------------------------------------------------------------------------------------

    std::string GetGameExe()
    {
        return _gameExe;
    }
    // ---------------------------------------------------------------------------------------------

    bool AutoStartEnabled()
    {
        return _autoStart;
    }
    // ---------------------------------------------------------------------------------------------

    uint32_t GetAutoStartMs()
    {
        return _autoStartDelayMs;
    }
    // ---------------------------------------------------------------------------------------------

    bool UpdateCMProfile()
    {
        return _enableNvapiProfileUpdate;
    }
    // ---------------------------------------------------------------------------------------------

    uint32_t GetStereoTexture()
    {
        return _stereoTexture;
    }
    // ---------------------------------------------------------------------------------------------

    uint32_t GetCMProfile()
    {
        return _cm_Profile;
    }
    // ---------------------------------------------------------------------------------------------

    uint32_t GetCMConvergence()
    {
        return _cm_Convergence;
    }
    // ---------------------------------------------------------------------------------------------

    std::string GetCMComments()
    {
        return _cm_Comments;
    }
    // ---------------------------------------------------------------------------------------------

private:
    void ReadKeySettings();
    void AddAlternativeKey(std::string &configLine);

private:
    std::vector<ALT_KEYS_T> _altKeys;
    std::string _gameExe = "";
    bool _autoStart = false;
    uint32_t _autoStartDelayMs = 10000;

    bool _enableNvapiProfileUpdate = false;
    uint32_t _stereoTexture = 1;
    uint32_t _cm_Profile = 0;
    uint32_t _cm_Convergence = 0;
    std::string _cm_Comments = "";
};