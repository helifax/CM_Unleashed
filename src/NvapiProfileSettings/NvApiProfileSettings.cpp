/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

/*
Author: Helifax
Original Date: 24.04.2015
Updated 12.02.2019

Interface to NVAPI to handle profile modification/creation in order for 3D Vision to render Properly!
*/
// <SettingID>271830721</SettingID> - is Predefined SLI Mode
// <SettingID>271834322</SettingID> - is GPU Count
// <SettingID>278257400</SettingID> - SLi Bits DX9
// 1888336069 - is Convergence

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include "NVAPI_410\nvapi.h"
#include "NVAPI_410\NvApiDriverSettings.h"

extern void __cdecl console_log(const char* fmt, ...);

#define STEREO_TEXTURE 0x70edb381
#define STEREO_PROFILE 0x701EB457
#define CM_2DD_PROFILE 0x709ADADA
#define CM_2DD_CONVERGENCE 0x709ADADB
#define CM_2DD_NOTES 0x709ADADC
#define CM_2DD_DISABLE 0x709ADADD

static std::string m_gameExe = "";
static uint32_t m_stereoTexture = 0;
static uint32_t m_profileValue = 0;
static uint32_t m_convergenceValue = 0;
static std::string m_comments = "";

static char m_profileName[255] = "";
static char m_friendlyName[255] = "";

static bool NvApi_IsProfileFound(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);
static bool NvApi_Enable3DVisionSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);
static bool NvApi_LoadDefaultProfile(NvDRSSessionHandle hSession);
static void NvApi_SetUnicodeString(NvAPI_UnicodeString& nvstr, const char* cstr);
static void _getProfileName(const char* inExeName, char* outProfileName);

static bool NvApi_ApplyProfileSettings(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);

static void PrintError(NvAPI_Status status)
{
    NvAPI_ShortString szDesc = { 0 };
    NvAPI_GetErrorMessage(status, szDesc);
    console_log(" NVAPI error: %s\n", szDesc);
}
///--------------------------------------------------------------------------------------

// Enumerate profiles and find the current profile for the .exe
bool NvApi_3DVisionProfileSetup(std::string gameExe, uint32_t stereoTexture, uint32_t profileValue, uint32_t convergenceValue, std::string comments)
{
    m_gameExe = gameExe;
    m_stereoTexture = stereoTexture;
    m_profileValue = profileValue;
    m_convergenceValue = convergenceValue;
    m_comments = comments;

    bool retValue = false;
    NvAPI_Status status;
    bool profileFound = false;

    // (0) Initialize NVAPI. This must be done first of all
    status = NvAPI_Initialize();
    if(status != NVAPI_OK)
        PrintError(status);

    // (1) Create the session handle to access driver settings
    NvDRSSessionHandle hSession = 0;
    status = NvAPI_DRS_CreateSession(&hSession);
    if(status != NVAPI_OK)
        PrintError(status);

    // (2) load all the system settings into the session
    status = NvAPI_DRS_LoadSettings(hSession);
    if(status != NVAPI_OK)
        PrintError(status);

    NvDRSProfileHandle hProfile = 0;
    uint32_t index = 0;

    while((status = NvAPI_DRS_EnumProfiles(hSession, index, &hProfile)) == NVAPI_OK)
    {
        // (4) hProfile is now a valid Profile handle.
        // retrieve information from the Profile
        profileFound = NvApi_IsProfileFound(hSession, hProfile);

        if(profileFound)
        {
            // Set the magic flag to enable 3D Vision for this profile
            retValue = NvApi_Enable3DVisionSetting(hSession, hProfile);
            break;
        }
        index++;
    }

    if((status == NVAPI_END_ENUMERATION) && (!profileFound))
    {
        // Create a default profile
        retValue = NvApi_LoadDefaultProfile(hSession);
    }
    else if(status != NVAPI_OK)
    {
        PrintError(status);
        retValue = false;
    }

    // (6) We clean up. This is analogous to doing a free()
    NvAPI_DRS_DestroySession(hSession);
    hSession = 0;

    return retValue;
}
///--------------------------------------------------------------------------------------

// If we found a profile for the current .exe
static bool NvApi_IsProfileFound(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile)
{
    // (0) this function assumes that the hSession and hProfile are
    // valid handles obtained from nvapi.
    NvAPI_Status status;

    // (1) First, we retrieve generic profile information
    // The structure will provide us with name, number of applications
    // and number of settings for this profile.
    NVDRS_PROFILE profileInformation = { 0 };
    profileInformation.version = NVDRS_PROFILE_VER;
    status = NvAPI_DRS_GetProfileInfo(hSession, hProfile, &profileInformation);
    if(status != NVAPI_OK)
    {
        PrintError(status);
        return false;
    }
    // (2) Now we enumerate through all the applications on the profile,
    // if there is any
    if(profileInformation.numOfApps > 0)
    {
        NVDRS_APPLICATION* appArray = new NVDRS_APPLICATION[profileInformation.numOfApps];
        NvU32 numAppsRead = profileInformation.numOfApps, i;
        bool profileFound = false;

        // (3) It is possible to enumerate all applications one by one,
        // or all at once on a preallocated array. The numAppsRead
        // represents the number of NVDRS_APPLICATION structures
        // allocated in the array. It will be modified on return of the
        // function contain the number of actual applications that have
        // been filled by NVAPI
        appArray[0].version = NVDRS_APPLICATION_VER;
        status = NvAPI_DRS_EnumApplications(hSession, hProfile, 0, &numAppsRead, appArray);
        if(status != NVAPI_OK)
        {
            PrintError(status);
            delete[] appArray;
            return false;
        }
        for(i = 0; i < numAppsRead; i++)
        {
            wchar_t appName[255];
            size_t origsize = strlen(m_gameExe.c_str()) + 1;
            const size_t newsize = 100;
            size_t convertedChars = 0;
            wchar_t unicodeExeName[newsize];
            std::transform(m_gameExe.begin(), m_gameExe.end(), m_gameExe.begin(), tolower);

            mbstowcs_s(&convertedChars, unicodeExeName, origsize, m_gameExe.c_str(), _TRUNCATE);
            swprintf_s(appName, 255, L"%s", (wchar_t*)appArray[i].appName);

            // Found the profile
            if(wcscmp(appName, unicodeExeName) == 0 && profileInformation.gpuSupport.geforce)
            {
                wchar_t temp[2048];
                swprintf_s(temp, L"%s", (wchar_t*)profileInformation.profileName);
                std::wstring ws(temp);
                // your new String
                std::string str(ws.begin(), ws.end());
                profileFound = true;
                break;
            }
        }
        delete[] appArray;
        return profileFound;
    }
    return false;
}
///--------------------------------------------------------------------------------------

// Set the 3D Vision Profile Magic
static bool NvApi_Enable3DVisionSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile)
{
    NvAPI_Status status;

    // (1) First, we retrieve generic profile information
    // The structure will provide us with name, number of applications
    // and number of settings for this profile.
    NVDRS_PROFILE profileInformation = { 0 };
    profileInformation.version = NVDRS_PROFILE_VER;
    status = NvAPI_DRS_GetProfileInfo(hSession, hProfile, &profileInformation);
    if(status != NVAPI_OK)
    {
        PrintError(status);
        return false;
    }

    // (4) Now we enumerate all the settings on the profile
    if(profileInformation.numOfSettings > 0)
    {
        NVDRS_SETTING* setArray = new NVDRS_SETTING[profileInformation.numOfSettings];
        NvU32 numSetRead = profileInformation.numOfSettings;

        // (5) The function to retrieve the settings in a profile works
        // like the function to retrieve the applications.
        setArray[0].version = NVDRS_SETTING_VER;
        status = NvAPI_DRS_EnumSettings(hSession, hProfile, 0, &numSetRead, setArray);
        if(status != NVAPI_OK)
        {
            PrintError(status);
            return false;
        }

        bool ret = NvApi_ApplyProfileSettings(hSession, hProfile);
        // Delete the array
        delete(setArray);
        return ret;
    }
    else
    {
        NvAPI_Status status;
        status = NvAPI_DRS_DeleteProfile(hSession, hProfile);
        return NvApi_LoadDefaultProfile(hSession);
    }
}
///--------------------------------------------------------------------------------------

// Create Default 3D Vision Profile
static bool NvApi_LoadDefaultProfile(NvDRSSessionHandle hSession)
{
    NVDRS_PROFILE newProfileInfo = { 0 };
    NvDRSProfileHandle returnNewProfile;
    NvAPI_Status status;
    NVDRS_APPLICATION newApplication;

    newProfileInfo.version = NVDRS_PROFILE_VER;
    newProfileInfo.isPredefined = 0;

    // Generate the required Names
    _getProfileName(m_gameExe.c_str(), m_profileName);
    _getProfileName(m_gameExe.c_str(), m_friendlyName);

    // Set Profile Name
    NvApi_SetUnicodeString(newProfileInfo.profileName, m_profileName);

    newApplication.version = NVDRS_APPLICATION_VER;
    newApplication.isPredefined = 0;

    // Set the names
    NvApi_SetUnicodeString(newApplication.appName, m_gameExe.c_str());
    NvApi_SetUnicodeString(newApplication.userFriendlyName, m_friendlyName);
    NvApi_SetUnicodeString(newApplication.launcher, "");
    NvApi_SetUnicodeString(newApplication.fileInFolder, "");

    // Create the profile
    status = NvAPI_DRS_CreateProfile(hSession, &newProfileInfo, &returnNewProfile);
    wchar_t temp[2048];
    swprintf_s(temp, L"%s", (wchar_t*)newProfileInfo.profileName);
    std::wstring ws(temp);
    // your new String
    std::string str(ws.begin(), ws.end());

    if(status == NVAPI_OK)
    {
        // Create the Application and added it to the profile
        status = NvAPI_DRS_CreateApplication(hSession, returnNewProfile, &newApplication);
        if(status == NVAPI_OK)
        {
            bool ret = NvApi_ApplyProfileSettings(hSession, returnNewProfile);
            // Delete the array
            return ret;
        }
        return false;
    }
    return false;
}
//--------------------------------------------------------------------------------------

// Convert from char to unicode chars used by NVAPI
static void NvApi_SetUnicodeString(NvAPI_UnicodeString& nvstr, const char* cstr)
{
    // convert to wchar_t
    size_t origsize = strlen(cstr) + 1;
    const size_t newsize = 255;
    size_t convertedChars = 0;

    wchar_t unicodeName[newsize];
    mbstowcs_s(&convertedChars, unicodeName, origsize, cstr, _TRUNCATE);

    // convert to NvUnicode
    for(int i = 0; i < NVAPI_UNICODE_STRING_MAX; ++i)
    {
        nvstr[i] = 0;
    }

    int i = 0;
    while(unicodeName[i] != 0)
    {
        nvstr[i] = unicodeName[i];
        i++;
    }
}
//--------------------------------------------------------------------------------------

// Generate the Profile Name
static void _getProfileName(const char* inExeName, char* outProfileName)
{
    char profileName[255] = "";
    strncat_s(profileName, inExeName, 255);
    strncat_s(profileName, "-3DVision_CM_Unleashed", 255);
    strcpy_s(outProfileName, 255, profileName);
}
//--------------------------------------------------------------------------------------

static bool NvApi_ApplyProfileSettings(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile)
{
    // We didn't find our value so we need to set it.
    NVDRS_SETTING setNewSettings;
    NvAPI_Status status;

    // Stereo Support!
    setNewSettings.version = NVDRS_SETTING_VER;
    setNewSettings.settingId = STEREO_PROFILE;
    setNewSettings.settingType = NVDRS_DWORD_TYPE;
    setNewSettings.u32CurrentValue = 0x00000001;  // Enable Stereo Support
    status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);

    // Stereo Texture. Disable 3D Vision Automatic Heuristics
    if(m_stereoTexture)
    {
        setNewSettings.version = NVDRS_SETTING_VER;
        setNewSettings.settingId = STEREO_TEXTURE;
        setNewSettings.settingType = NVDRS_DWORD_TYPE;
        setNewSettings.u32CurrentValue = m_stereoTexture;
        status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);
    }

    if(m_profileValue)
    {
        // Enable CM Mode
        setNewSettings.version = NVDRS_SETTING_VER;
        setNewSettings.settingId = CM_2DD_DISABLE;
        setNewSettings.settingType = NVDRS_DWORD_TYPE;
        setNewSettings.u32CurrentValue = 0x00000000;
        status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);

        // Set the default CM settings
        setNewSettings.version = NVDRS_SETTING_VER;
        setNewSettings.settingId = CM_2DD_PROFILE;
        setNewSettings.settingType = NVDRS_DWORD_TYPE;
        setNewSettings.u32CurrentValue = m_profileValue;
        status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);
    }

    if(m_convergenceValue)
    {
        // Set the default CM Convergence
        setNewSettings.version = NVDRS_SETTING_VER;
        setNewSettings.settingId = CM_2DD_CONVERGENCE;
        setNewSettings.settingType = NVDRS_DWORD_TYPE;
        setNewSettings.u32CurrentValue = m_convergenceValue;  // Witcher 3 CM value
        status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);
    }

    // Set the default CM comments
    if(m_comments != "")
    {
        setNewSettings.version = NVDRS_SETTING_VER;
        setNewSettings.settingId = CM_2DD_NOTES;
        setNewSettings.settingType = NVDRS_WSTRING_TYPE;
        setNewSettings.isCurrentPredefined = 1;
        setNewSettings.isPredefinedValid = 1;
        NvApi_SetUnicodeString(setNewSettings.wszCurrentValue, m_comments.c_str());
        NvApi_SetUnicodeString(setNewSettings.wszPredefinedValue, m_comments.c_str());
        status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);
    }

    if(status == NVAPI_OK)
    {
        // Save settings to the driver
        status = NvAPI_DRS_SaveSettings(hSession);
        if(status == NVAPI_OK)
        {
            return true;
        }
        return false;
    }
    return false;
}
//--------------------------------------------------------------------------------------