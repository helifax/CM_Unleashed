#pragma once

#include <Windows.h>
#include <sstream>
#include <string>
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

    DWORD getPid(std::string &_process);

    bool DoPatching(std::string &gameExeName);
    bool RestoreOriginal(std::string &gameExeName);

    bool CM_SetSeparationFactor(float *factor);
    bool CM_GetSeparationFactor(float *factor);
    bool CM_SetConvergence(float *newValue);
    bool CM_GetConvergence(float *newValue);

private:
    // No copy
    CMUnleashed(const CMUnleashed &);
    CMUnleashed &operator=(const CMUnleashed &);

    DWORD64 getModuleBaseAddress(std::string &moduleName);
    DWORD64 findSignature(DWORD64 startOffset, std::string &pattern);
    std::string getHexOPCodesFromString(std::string &_input);

    // 32 bit
    bool injectCodeCaveSeparation32(DWORD64 baseAddress, DWORD64 signatureAddress, std::string &shellcode);
    bool injectCodeCaveConvergence32(DWORD64 baseAddress, DWORD64 signatureAddress, std::string &shellcode);

    // 64 bit
    bool injectCodeCaveSeparation64(DWORD64 baseAddress, DWORD64 signatureAddress, std::string &shellcode);
    bool injectCodeCaveConvergence64(DWORD64 baseAddress, DWORD64 signatureAddress, std::string &shellcode);

    // We don't expose the actual Separation
    // Instead we expose the factor currently applied!
    bool CM_GetActualSeparation(float *newValue);

private:
    // Addresses that we use!!!
    void *newCodeCoveSeparation = nullptr;
    void *newSeparation = nullptr;
    void *prevSeparation = nullptr;

    void *newCodeCoveConvergence = nullptr;
    void *crtConvergence = nullptr;
    void *newConvergence = nullptr;
    void *prevConvergence = nullptr;

    float _separationFactor = 0.0f;
    float _origSeparation = 0.0f;

    DWORD64 _max_address_to_scan = 0;
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
