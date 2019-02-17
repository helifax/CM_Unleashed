/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <windows.h>
#include "ConfigReader.h"
#include "KeyParser.h"

static std::string GetPath()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}
// -----------------------------------------------------------------------------------

ProfileLoader::ProfileLoader()
{
    // grab all profiles
    std::string profileFolder = GetPath() + "\\Profiles\\";
    _allProfiles = get_all_files_names_within_folder(profileFolder);
}
// -----------------------------------------------------------------------------------

std::vector<std::string> ProfileLoader::get_all_files_names_within_folder(std::string folder)
{
    std::vector<std::string> names;
    std::string search_path = folder + "/*.ini";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                names.push_back(fd.cFileName);
            }
        } while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}
// -----------------------------------------------------------------------------------

ConfigReader::ConfigReader(std::string _profileToLoad)
{
    _gameExe = "";
    std::string configFileName = GetPath() + "\\Profiles\\" + _profileToLoad;
    std::ifstream configFile(configFileName);
    std::string configLine;

    //file is opened
    configFile.seekg(0, std::ios::beg);
    if(configFile.is_open())
    {
        // read one line at a time
        while(getline(configFile, configLine))
        {
            //----------------------------------------------
            //[General]
            {
                if((configLine.find("GameExecutable") != std::string::npos) && (configLine.find(";") == std::string::npos))
                {
                    if(configLine.find("=") != std::string::npos)
                    {
                        size_t position = configLine.find("=");
                        // Get rid of the quotes as well
                        std::string gameExe = configLine.substr(position + 2);
                        _gameExe = gameExe.substr(0, gameExe.length() - 1);
                    }
                }
                if((configLine.find("AutoStart") != std::string::npos) && (configLine.find(";") == std::string::npos))
                {
                    if(configLine.find("true") != std::string::npos)
                    {
                        _autoStart = true;
                    }
                    else if(configLine.find("false") != std::string::npos)
                    {
                        _autoStart = false;
                    }
                }
                if((configLine.find("AutoStartDelay") != std::string::npos) && (configLine.find(";") == std::string::npos))
                {
                    size_t position = configLine.find("=");
                    std::string value = configLine.substr(position + 1);
                    uint32_t crtValue = stoi(value) * 1000;
                    _autoStartDelayMs = crtValue > 0 ? crtValue : 10000;
                }

                if((configLine.find("2DDEnableUpdates") != std::string::npos) && (configLine.find(";") == std::string::npos))
                {
                    if(configLine.find("true") != std::string::npos)
                    {
                        _enableNvapiProfileUpdate = true;
                    }
                    else if(configLine.find("false") != std::string::npos)
                    {
                        _enableNvapiProfileUpdate = false;
                    }
                }
                // only if enabled
                if(_enableNvapiProfileUpdate)
                {
                    if((configLine.find("StereoTexture") != std::string::npos) && (configLine.find(";") == std::string::npos))
                    {
                        if(configLine.find("StereoTexture") != std::string::npos)
                        {
                            size_t position = configLine.find("=");
                            std::string value = configLine.substr(position + 1);
                            std::stringstream dwordStream;
                            dwordStream << value;
                            dwordStream >> std::hex >> _stereoTexture;
                        }
                    }

                    if((configLine.find("2DDHUDSettings") != std::string::npos) && (configLine.find(";") == std::string::npos))
                    {
                        if(configLine.find("2DDHUDSettings") != std::string::npos)
                        {
                            size_t position = configLine.find("=");
                            std::string value = configLine.substr(position + 1);
                            std::stringstream dwordStream;
                            dwordStream << value;
                            dwordStream >> std::hex >> _cm_Profile;
                        }
                    }
                    if((configLine.find("2DDConvergence") != std::string::npos) && (configLine.find(";") == std::string::npos))
                    {
                        if(configLine.find("2DDConvergence") != std::string::npos)
                        {
                            union {
                                unsigned int i;
                                float f;
                            } bits;
                            // Read the default convergence
                            size_t position = configLine.find("=");
                            std::string value = configLine.substr(position + 1);
                            bits.f = std::stof(value, 0);
                            _cm_Convergence = bits.i;
                        }
                    }
                    if((configLine.find("2DD_Notes") != std::string::npos) && (configLine.find(";") == std::string::npos))
                    {
                        if(configLine.find("2DD_Notes") != std::string::npos)
                        {
                            size_t position = configLine.find("=");
                            // Get rid of the quotes as well
                            std::string gameExe = configLine.substr(position + 2);
                            _cm_Comments = gameExe.substr(0, gameExe.length() - 1);
                        }
                    }
                }
            }
            //----------------------------------------------
        }
        configFile.close();

        // Read the KEYS
        ReadKeySettings();
    }
}
// -----------------------------------------------------------------------------------

void ConfigReader::ReadKeySettings()
{
    std::string configFileName = GetPath() + "/3DVision_CM_Unleashed.ini";
    std::ifstream configFile(configFileName);
    std::string configLine;

    //file is opened
    configFile.seekg(0, std::ios::beg);
    if(configFile.is_open())
    {
        // read one line at a time
        while(getline(configFile, configLine))
        {
            //[KEY_SETTINGS]
            if(configLine.find("[Key_Settings]") != std::string::npos)
            {
                _altKeys.clear();

                while(configLine.find("[End]") == std::string::npos)
                {
                    getline(configFile, configLine);
                    //----------------------------------------------
                    //get the next line
                    if((configLine.find("NewKey") != std::string::npos) && (configLine.find(";") == std::string::npos))
                    {
                        AddAlternativeKey(configLine);
                    }
                }
            }
        }
        configFile.close();
    }
}
// ---------------------------------------------------------------------------------------------

void ConfigReader::AddAlternativeKey(std::string &configLine)
{
    ALT_KEYS_T newKey;

    // store the injection point
    size_t startPoint = configLine.find("(", 0);
    size_t middlePoint1 = configLine.find(",", startPoint + 1);
    size_t middlePoint2 = configLine.find(",", middlePoint1 + 1);
    size_t middlePoint3 = configLine.find(",", middlePoint2 + 1);
    size_t middlePoint4 = configLine.find(",", middlePoint3 + 1);
    size_t middlePoint5 = configLine.find(",", middlePoint4 + 1);
    size_t middlePoint6 = configLine.find(",", middlePoint5 + 1);
    size_t middlePoint7 = configLine.find(",", middlePoint6 + 1);
    size_t endPoint = configLine.find(")", middlePoint3 + 1);

    // Read the key
    std::string temp = configLine.substr(startPoint + 1, middlePoint1 - startPoint - 1).c_str();
    newKey.altKeyCode = ParseVKey(temp.c_str());

    newKey.altSeparation = (float)atof(configLine.substr(middlePoint1 + 1, middlePoint2 - middlePoint1 - 1).c_str());
    newKey.altConvergence = (float)atof(configLine.substr(middlePoint2 + 1, middlePoint3 - middlePoint2 - 1).c_str());
    temp = (configLine.substr(middlePoint7 + 1, endPoint - middlePoint7 - 1).c_str());

    if(temp.find("once") != std::string::npos)
    {
        newKey.keyMode = ALT_KEYS_MODE_ONE_TIME;
    }
    else if(configLine.find("hold") != std::string::npos)
    {
        newKey.keyMode = ALT_KEYS_MODE_HOLD;
    }
    else if(configLine.find("toggle") != std::string::npos)
    {
        newKey.keyMode = ALT_KEYS_MODE_TOGGLE;
    }

    // Set the initial states
    newKey.keyPrevState = false;
    newKey.keyState = false;

    // Push them
    _altKeys.push_back(newKey);
}
// ---------------------------------------------------------------------------------------------