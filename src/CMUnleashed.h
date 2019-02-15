/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

#pragma once

#include <Windows.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <TlHelp32.h>
#include <thread>
#include "Psapi.h"

#define ADDRESS (void *)
#define MAX_ADDR_TO_SCAN 0x7fffffffffffffff

class CMUnleashed
{
public:
    CMUnleashed();
    ~CMUnleashed();

    DWORD getPid(const std::string &_process);

    bool DoPatching(const std::string &gameExeName);
    bool RestoreOriginal(std::string &gameExeName);

    bool CM_SetSeparationFactor(float *factor);
    bool CM_GetSeparationFactor(float *factor);
    bool CM_SetConvergence(float *newValue);
    bool CM_GetConvergence(float *newValue);

    // Update the PID
    void UpdateExePid(DWORD newPid)
    {
        _exePid = newPid;
    }

    DWORD GetExePid(void)
    {
        return _exePid;
    }

private:
    // No copy
    CMUnleashed(const CMUnleashed &);
    CMUnleashed &operator=(const CMUnleashed &);

    // Not actually useful, except to see if Module is in PID
    DWORD64 getModuleBaseAddress(const std::string &moduleName);
    DWORD64 findSignature(const std::string &moduleName, const std::vector<BYTE> &bytes_to_find);

    // 32 bit
    bool injectCodeCaveSeparation32(const DWORD64 baseAddress, DWORD64 const signatureAddress, const std::string &shellcode);
    bool injectCodeCaveConvergence32(const DWORD64 baseAddress, DWORD64 const signatureAddress, const std::string &shellcode);

    // 64 bit
    bool injectCodeCaveSeparation64(const DWORD64 baseAddress, const DWORD64 signatureAddress, const std::string &shellcode);
    bool injectCodeCaveConvergence64(const DWORD64 baseAddress, const DWORD64 signatureAddress, const std::string &shellcode);

    // We don't expose the actual Separation
    // Instead we expose the factor currently applied!
    bool CM_GetActualSeparation(float *newValue);

private:
    // Addresses that we use!!!
    void *newCodeCoveSeparation = nullptr;
    void *newSeparation = nullptr;
    void *prevSeparation = nullptr;
    void *jumpBackSeparation = nullptr;

    void *newCodeCoveConvergence = nullptr;
    void *crtConvergence = nullptr;
    void *newConvergence = nullptr;
    void *prevConvergence = nullptr;

    float _separationFactor = 0.0f;
    float _origSeparation = 0.0f;

    uint8_t _originalSeparationCode[8] = { 0 };
    uint8_t _originalConvergenceCode[8] = { 0 };
    DWORD64 _signatureSeparationAddress = 0;
    DWORD64 _signatureConvergeAddress = 0;

    DWORD _exePid = 0;
    bool _sepPatchDone = false;
    bool _convPatchDone = false;
    bool _is64Bit = false;
};
//-----------------------------------------------------------------------------
