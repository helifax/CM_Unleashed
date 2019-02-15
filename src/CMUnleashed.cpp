/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

#include "CMUnleashed.h"
#include <math.h>

extern void __cdecl console_log(const char *fmt, ...);

//////////////////////////////////////////////////////////////////////////
// INTERNAL STUFF
//////////////////////////////////////////////////////////////////////////

const std::string m_jumpToCodeCave = std::string("\xE9\xAA\xAA\xAA\xAA\x90\x90\x90", 8);

//32 bit variant
const std::string m_moduleName32 = "nvwgf2um.dll";
const std::vector<BYTE> m_signatureSeparation32{ 0xF3, 0x0F, 0x10, 0x80, '?', '?', '?', '?', 0x0F, 0x2E, 0x46 };
const std::vector<BYTE> m_signatureConvergence32{ 0xF3, 0x0F, 0x10, 0x86, '?', '?', '?', '?', 0x0F, 0x2E, 0x46 };
const std::string m_shellcodeSeparation32 = std::string(
    "\x53\x51\x8B\x1D\xAA\xAA\xAA\xAA\x83\xFB\x00\x0F\x84\x14\x00\x00\x00\x8B\x0D\xBB\xBB\xBB\xBB\x39\xCB\x0F\x85\x2E\x00\x00\x00\x0F\x84\x19\x00\x00\x00\x52\x8B\x90\xDD\xDD\xDD\xDD\x89\x15\xAA\xAA\xAA\xAA\x89\x15\xBB\xBB\xBB\xBB\x5A\xE9\x00\x00\x00\x00\x59\x5B\xF3\x0F\x10\x80\xDD\xDD\xDD\xDD\xE9\xCC\xCC\xCC\xCC\x59\x5B\x53\x8B\x1D\xAA\xAA\xAA\xAA\x89\x1D\xBB\xBB\xBB\xBB\x89\x98\xDD\xDD\xDD\xDD\x5B\xF3\x0F\x10\x80\xDD\xDD\xDD\xDD",
    107);
const std::string m_shellcodeConvergence32 = std::string(
    "\x50\x51\xA1\xAA\xAA\xAA\xAA\x83\xF8\x00\x0F\x84\x28\x00\x00\x00\x8B\x0D\xBB\xBB\xBB\xBB\x39\xC8\x0F\x85\x36\x00\x00\x00\x0F\x84\x14\x00\x00\x00\x52\x8B\x96\xDD\xDD\xDD\xDD\x89\x15\xAA\xAA\xAA\xAA\x89\x15\xBB\xBB\xBB\xBB\x5A\x59\x58\xF3\x0F\x10\x86\xDD\xDD\xDD\xDD\x50\x8B\x86\xDD\xDD\xDD\xDD\xA3\xCC\xCC\xCC\xCC\x58\xE9\xEE\xEE\xEE\xEE\x59\x58\x50\xA1\xAA\xAA\xAA\xAA\xA3\xBB\xBB\xBB\xBB\x89\x86\xDD\xDD\xDD\xDD\x58\xF3\x0F\x10\x86\xDD\xDD\xDD\xDD",
    112);

// 64 bit variant
const std::string m_moduleName64 = "nvwgf2umx.dll";
const std::vector<BYTE> m_signatureSeparation64{ 0xF3, 0x0F, 0x10, 0x80, '?', '?', '?', '?', 0x0F, 0x2E, 0x43 };
const std::vector<BYTE> m_signatureConvergence64{ 0xF3, 0x0F, 0x10, 0x83, '?', '?', '?', '?', 0x0F, 0x2E, 0x43 };
std::string m_shellcodeSeparation64 = std::string(
    "\x53\x51\x48\x8B\x1D\xAA\xAA\xAA\xAA\x48\x83\xFB\x00\x74\x1A\x90\x90\x90\x90\x48\x8B\x0D\xBB\xBB\xBB\xBB\x48\x39\xCB\x75\x47\x90\x90\x90\x90\x74\x29\x90\x90\x90\x90\x52\x48\x8B\x90\xCC\xCC\xCC\xCC\x48\x89\x15\xAA\xAA\xAA\xAA\x48\x89\x15\xBB\xBB\xBB\xBB\x5A\xEB\x0C\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x59\x5B\xF3\x0F\x10\x80\xCC\xCC\xCC\xCC\xE9\x99\x99\x99\x99\x90\x90\x90\x90\x90\x90\x90\x90\x90\x59\x5B\x53\x48\x8B\x1D\xAA\xAA\xAA\xAA\x48\x89\x1D\xBB\xBB\xBB\xBB\x48\x89\x98\xCC\xCC\xCC\xCC\x5B\xF3\x0F\x10\x80\xCC\xCC\xCC\xCC",
    135);
std::string m_shellcodeConvergence64 = std::string(
    "\x50\x51\x48\xA1\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x48\x83\xF8\x00\x74\x31\x90\x90\x90\x90\x48\x8B\x0D\xBB\xBB\xBB\xBB\x48\x39\xC8\x75\x4A\x90\x90\x90\x90\x74\x1B\x90\x90\x90\x90\x52\x48\x8B\x93\xFF\xFF\xFF\xFF\x48\x89\x15\xAA\xAA\xAA\xAA\x48\x89\x15\xBB\xBB\xBB\xBB\x5A\x59\x58\xF3\x0F\x10\x83\xFF\xFF\xFF\xFF\x50\x8B\x83\xFF\xFF\xFF\xFF\xA3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x58\xE9\xDD\xDD\xDD\xDD\x90\x90\x90\x90\x90\x90\x90\x90\x90\x59\x58\x50\x48\xA1\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x48\xA3\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x48\x89\x83\xFF\xFF\xFF\xFF\x58\xF3\x0F\x10\x83\xFF\xFF\xFF\xFF",
    146);
//////////////////////////////////////////////////////////////////////////

CMUnleashed::CMUnleashed()
{
}
//-----------------------------------------------------------------------------

CMUnleashed::~CMUnleashed()
{
}
//-----------------------------------------------------------------------------

DWORD CMUnleashed::getPid(const std::string &_process)
{
    HANDLE hsnap;
    PROCESSENTRY32 pt;
    hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pt.dwSize = sizeof(PROCESSENTRY32);
    while(Process32Next(hsnap, &pt))
    {
        if(!strcmp(pt.szExeFile, _process.c_str()))
        {
            DWORD pid = pt.th32ProcessID;

            BOOL is32 = FALSE;
            HANDLE p = p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, pid);
            if(IsWow64Process(p, &is32))
            {
                // What type of process this is!
                _is64Bit = !is32;
            }
            CloseHandle(p);
            CloseHandle(hsnap);
            return pid;
        }
    }
    CloseHandle(hsnap);
    return 0;
}
//-----------------------------------------------------------------------------

DWORD64 CMUnleashed::getModuleBaseAddress(const std::string &moduleName)
{
    HMODULE *hModules;
    char szBuf[50];
    DWORD cModules = 4096;
    DWORD64 dwBase = -1;
    DWORD MODULE = 0;

    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, _exePid);
    //------
    if(_is64Bit)
    {
        MODULE = LIST_MODULES_64BIT;
    }
    else
    {
        MODULE = LIST_MODULES_32BIT;
    }

    EnumProcessModulesEx(p, NULL, 0, &cModules, MODULE);
    hModules = new HMODULE[cModules];

    if(EnumProcessModulesEx(p, hModules, cModules, &cModules, MODULE))
    {
        for(unsigned int i = 0; i < cModules / sizeof(HMODULE); i++)
        {
            if(GetModuleBaseName(p, hModules[i], szBuf, sizeof(szBuf)) != 0)
            {
                if(moduleName.compare(szBuf) == 0)
                {
                    dwBase = (DWORD64)hModules[i];
                    MODULEINFO info = { 0 };
                    GetModuleInformation(p, hModules[i], &info, sizeof(MODULEINFO));
                    break;
                }
            }
        }
    }
    delete[] hModules;
    CloseHandle(p);
    return dwBase;
}
//-----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// THIS IS A VERY SLOW FUNCTION.
// It's very pedantic, but correctness first!
// KEPT FOR REFERENCES!
/*
DWORD64 CMUnleashed::findSignature(DWORD64 startOffset, std::string &pattern)
{
    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_ALL_ACCESS, 0, _exePid);

    if(p)
    {
        uint64_t timestamp = GetTickCount64();

        DWORD patternLength = (DWORD)strlen(pattern.c_str());
        //Check for the signature
        for(DWORD offset = 0; offset < (_max_address_to_scan - patternLength); offset++)
        {
            bool found = true;
            for(DWORD j = 0; j < patternLength; j++)
            {
                char value = 0;
                // Read from the current address
                ReadProcessMemory(p, (LPCVOID)(startOffset + offset + j), &value, 1, 0);
                //Compare with the wildcard or our value
                found &= pattern[j] == '?' || pattern[j] == value;
                // If we don't find an opcode we break immediately as there is no point on scanning further
                if(!found)
                {
                    break;
                }
            }
            // We found the address we return it:)
            if(found)
            {
                CloseHandle(p);
                return startOffset + offset;
            }
            uint64_t delta = GetTickCount64() - timestamp;
            if(delta > 30000)
            {
                // abort took too long
                CloseHandle(p);
                return 0x00;
            }
        }
        // Signature not found; return 0x00
        CloseHandle(p);
    }
    return 0x00;
}
//-----------------------------------------------------------------------------

std::string CMUnleashed::getHexOPCodesFromString(std::string &_input)
{
    std::string output;
    std::string separator = " ";

    size_t pos = 0;
    size_t oldPos = 0;
    while(pos < _input.length())
    {
        pos = _input.find(separator, pos + 1);
        if(pos)
        {
            if(oldPos)
                oldPos += 1;

            std::string temp = std::string(_input.substr(oldPos, 2));

            unsigned long long value;
            std::stringstream hexvalue;
            hexvalue << temp;
            // Wildcard "?" or not ?
            if(temp != "??")
            {
                //convert to hex and store in value
                hexvalue >> std::hex >> value;
                // add it to our signature array
                output += (char)value;
            }
            else
            {
                // if there is a wildcard defined we simply store it
                output += "?";
            }
            oldPos = pos;
        }
    }
    return output;
}
//-----------------------------------------------------------------------------
*/

DWORD64 CMUnleashed::findSignature(const std::string &moduleName, const std::vector<BYTE> &bytes_to_find)
{
    HMODULE *hModules;
    char szBuf[50];
    DWORD cModules = 4096;
    DWORD64 dwBase = -1;
    DWORD MODULE = 0;
    DWORD64 address_found = 0x0;
    bool isFound = false;
    bool isModuleFound = false;

    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_ALL_ACCESS, 0, _exePid);
    //------
    if(_is64Bit)
    {
        MODULE = LIST_MODULES_64BIT;
    }
    else
    {
        MODULE = LIST_MODULES_32BIT;
    }

    EnumProcessModulesEx(p, NULL, 0, &cModules, MODULE);
    hModules = new HMODULE[cModules];

    if(EnumProcessModulesEx(p, hModules, cModules, &cModules, MODULE))
    {
        for(unsigned int i = 0; i < cModules / sizeof(HMODULE); i++)
        {
            // Did we find the address of the module (and couldn't find the addres??)
            if(isFound || (isModuleFound && !isFound))
                break;

            if(GetModuleBaseName(p, hModules[i], szBuf, sizeof(szBuf)) != 0)
            {
                if(moduleName.compare(szBuf) == 0)
                {
                    dwBase = (DWORD64)hModules[i];
                    MODULEINFO info = { 0 };
                    GetModuleInformation(p, hModules[i], &info, sizeof(MODULEINFO));

                    // Start the memory search!
                    MEMORY_BASIC_INFORMATION mbi{};
                    BYTE *address = static_cast<BYTE *>(ADDRESS(dwBase));
                    BYTE *address_high = address + info.SizeOfImage;

                    while(address < address_high && VirtualQuery(address, std::addressof(mbi), sizeof(mbi)))
                    {
                        BYTE *memoryCopy = new BYTE[mbi.RegionSize];
                        ReadProcessMemory(p, address, memoryCopy, mbi.RegionSize, nullptr);

                        // The DLL that we are looking for has these properties :)
                        if(mbi.State == MEM_FREE && mbi.Protect == PAGE_NOACCESS)
                        {
                            const BYTE *begin = static_cast<const BYTE *>(memoryCopy);
                            const BYTE *end = begin + mbi.RegionSize;

                            // Search based on our lambda function that accepts wildcards
                            const BYTE *found = std::search(begin, end, bytes_to_find.begin(), bytes_to_find.end(),
                                [](char fromText, char fromPattern) { return fromPattern == '?' || fromPattern == fromText; });

                            if(found != end)
                            {
                                // We found the address
                                DWORD64 offset = (DWORD64)found - (DWORD64)begin;
                                address_found = (DWORD64)mbi.BaseAddress + offset;
                                isFound = true;
                                delete[] memoryCopy;
                                break;
                            }
                            delete[] memoryCopy;
                        }
                        address += mbi.RegionSize;
                        mbi = {};
                    }

                    // We found the module but not the address if we get here!
                    isModuleFound = true;
                }
            }
        }
    }
    delete[] hModules;
    CloseHandle(p);
    return address_found;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::injectCodeCaveSeparation32(const DWORD64 baseAddress, const DWORD64 signatureAddress, const std::string &shellcode)
{
    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 0, _exePid);

    if(p)
    {
        // Get our number of bytes to write
        unsigned long shsize = (unsigned long)shellcode.length();

        // Find our block of free space to allocate
        // so we can still do 32bit long jump in code-caves
        MEMORY_BASIC_INFORMATION mBI = { 0 };
        DWORD64 memoryRegionStart = baseAddress;
        DWORD64 memoryRegionEnd = MAX_ADDR_TO_SCAN;

        while(memoryRegionStart < memoryRegionEnd)
        {
            memset(&mBI, 0, sizeof(MEMORY_BASIC_INFORMATION));
            VirtualQueryEx(p, ADDRESS(memoryRegionStart), &mBI, sizeof(MEMORY_BASIC_INFORMATION));

            if(mBI.State == MEM_FREE)
            {
                if(!newSeparation)
                {
                    newSeparation = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }
                if(!prevSeparation)
                {
                    prevSeparation = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }

                newCodeCoveSeparation = VirtualAllocEx(p, ADDRESS(memoryRegionStart), shsize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
                if(newCodeCoveSeparation)
                {
                    break;
                }
            }
            memoryRegionStart = (DWORD64)mBI.BaseAddress + mBI.RegionSize;
        }

        if(newCodeCoveSeparation)
        {
            // Store the original code that we will inject -> for reverting!
            ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress)), &_originalSeparationCode, 8, NULL);

            // Write our CodeCave in our allocated memory space
            WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation)), shellcode.c_str(), shsize, NULL);

            // Write our registry offset inside the Code Cave!
            {
                // Read the currently running registries so we update our code cave with it!
                char originalRegistryHoldingValue[4] = "";
                ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress + 0x4)), originalRegistryHoldingValue, 4, NULL);

                // Write it in our Code Cave!
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x28), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x44), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x5E), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x67), &originalRegistryHoldingValue, 4, NULL);
            }

            // Write the Separation addresses
            {
                // Write the new Separation memory locations
                // Offset Addr =    Actual Addr         Current Position in the code    length of the addr (4 bytes always)
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x04), &newSeparation, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x2E), &newSeparation, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x52), &newSeparation, 4, NULL);

                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x13), &prevSeparation, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x34), &prevSeparation, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x58), &prevSeparation, 4, NULL);
            }

            // Write all our jumps back to the original code
            {
                // Now Write the return memory address
                // 1nd JUMP
                DWORD64 jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveSeparation) + 0x48 + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x48 + 0x1), &jumpBackAddress, 4, NULL);

                // Now Write the return memory address
                // 2nd JUMP
                // Write the jump in the code cave
                std::string jmpCC = "\xE9\x90\x90\x90\x90";
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + shsize), jmpCC.c_str(), jmpCC.length(), NULL);

                jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveSeparation) + shsize + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + shsize + 0x01), &jumpBackAddress, 4, NULL);
            }

            // Write the JUMP to our CODE CAVE and thus enabling the code cave!
            {
                void *tempAddress = VirtualAllocEx(p, NULL, m_jumpToCodeCave.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                /// Write the JUMP to our address
                WriteProcessMemory(p, ADDRESS(tempAddress), m_jumpToCodeCave.c_str(), m_jumpToCodeCave.length(), NULL);

                /// Now Write the memory address where the code-cave was allocated
                DWORD64 jumpAddress = DWORD64(newCodeCoveSeparation) - signatureAddress - (0x05);
                WriteProcessMemory(p, ADDRESS((DWORD64)tempAddress + 1), &jumpAddress, 4, NULL);

                // Copy the whole thing to the new location
                void *jmpBack = malloc(m_jumpToCodeCave.length());
                ReadProcessMemory(p, ADDRESS((DWORD64)(tempAddress)), jmpBack, m_jumpToCodeCave.length(), NULL);
                WriteProcessMemory(p, ADDRESS(signatureAddress), jmpBack, m_jumpToCodeCave.length(), NULL);

                free(jmpBack);
                VirtualFreeEx(p, tempAddress, m_jumpToCodeCave.length(), MEM_DECOMMIT);
            }
            CloseHandle(p);
            return true;
        }
    }
    CloseHandle(p);
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::injectCodeCaveConvergence32(const DWORD64 baseAddress, const DWORD64 signatureAddress, const std::string &shellcode)
{
    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 0, _exePid);

    if(p)
    {
        // Get our number of bytes to write
        unsigned long shsize = (unsigned long)shellcode.length();

        // Find our block of free space to allocate
        // so we can still do 32bit long jump in code-caves
        MEMORY_BASIC_INFORMATION mBI = { 0 };
        DWORD64 memoryRegionStart = baseAddress;
        DWORD64 memoryRegionEnd = MAX_ADDR_TO_SCAN;

        while(memoryRegionStart < memoryRegionEnd)
        {
            memset(&mBI, 0, sizeof(MEMORY_BASIC_INFORMATION));
            VirtualQueryEx(p, ADDRESS(memoryRegionStart), &mBI, sizeof(MEMORY_BASIC_INFORMATION));

            if(mBI.State == MEM_FREE)
            {
                if(!crtConvergence)
                {
                    crtConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }
                if(!newConvergence)
                {
                    newConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }
                if(!prevConvergence)
                {
                    prevConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }

                newCodeCoveConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), shsize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
                if(newCodeCoveConvergence)
                {
                    break;
                }
            }
            memoryRegionStart = (DWORD64)mBI.BaseAddress + mBI.RegionSize;
        }

        if(newCodeCoveConvergence)
        {
            // Store the original code that we will inject -> for reverting!
            ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress)), &_originalConvergenceCode, 8, NULL);

            // Write our CodeCave in our allocated memory space
            WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence)), shellcode.c_str(), shsize, NULL);

            // Write our registry offset inside the Code Cave!
            {
                // Read the currently running registries so we update our code cave with it!
                char originalRegistryHoldingValue[4] = "";
                ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress + 0x4)), originalRegistryHoldingValue, 4, NULL);

                // Write it in our Code Cave!
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x27), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x3E), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x45), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x63), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x6C), &originalRegistryHoldingValue, 4, NULL);
            }

            // Write the Convergence addresses
            {
                // Write the new Separation memory locations
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x03), &newConvergence, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x12), &prevConvergence, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x2D), &newConvergence, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x33), &prevConvergence, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x4A), &crtConvergence, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x58), &newConvergence, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x5D), &prevConvergence, 4, NULL);
            }

            // Write all our jumps back to the original code
            {
                // Now Write the return memory address
                // 1nd JUMP
                DWORD64 jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveConvergence) + 0x4F + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x4F + 0x1), &jumpBackAddress, 4, NULL);

                // Now Write the return memory address
                // 2nd JUMP
                // Write the jump in the code cave
                std::string jmpCC = "\xE9\x90\x90\x90\x90";
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + shsize), jmpCC.c_str(), jmpCC.length(), NULL);

                jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveConvergence) + shsize + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + shsize + 0x01), &jumpBackAddress, 4, NULL);
            }

            // Write the JUMP to our CODE CAVE and thus enabling the code cave!
            {
                void *tempAddress = VirtualAllocEx(p, NULL, m_jumpToCodeCave.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                /// Write the JUMP to our address
                WriteProcessMemory(p, ADDRESS(tempAddress), m_jumpToCodeCave.c_str(), m_jumpToCodeCave.length(), NULL);

                /// Now Write the memory address where the code-cave was allocated
                DWORD64 jumpAddress = DWORD64(newCodeCoveConvergence) - signatureAddress - (0x05);
                WriteProcessMemory(p, ADDRESS((DWORD64)tempAddress + 1), &jumpAddress, 4, NULL);

                // Copy the whole thing to the new location
                void *jmpBack = malloc(m_jumpToCodeCave.length());
                ReadProcessMemory(p, ADDRESS((DWORD64)(tempAddress)), jmpBack, m_jumpToCodeCave.length(), NULL);
                WriteProcessMemory(p, ADDRESS(signatureAddress), jmpBack, m_jumpToCodeCave.length(), NULL);

                free(jmpBack);
                VirtualFreeEx(p, tempAddress, m_jumpToCodeCave.length(), MEM_DECOMMIT);
            }
            CloseHandle(p);
            return true;
        }
    }
    CloseHandle(p);
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::injectCodeCaveSeparation64(const DWORD64 baseAddress, const DWORD64 signatureAddress, const std::string &shellcode)
{
    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 0, _exePid);

    if(p)
    {
        // Get our number of bytes to write
        unsigned long shsize = (unsigned long)shellcode.length();

        // Find our block of free space to allocate
        // so we can still do 32bit long jump in code-caves
        MEMORY_BASIC_INFORMATION mBI = { 0 };
        DWORD64 memoryRegionStart = baseAddress;
        DWORD64 memoryRegionEnd = MAX_ADDR_TO_SCAN;

        while(memoryRegionStart < memoryRegionEnd)
        {
            memset(&mBI, 0, sizeof(MEMORY_BASIC_INFORMATION));
            VirtualQueryEx(p, ADDRESS(memoryRegionStart), &mBI, sizeof(MEMORY_BASIC_INFORMATION));

            if(mBI.State == MEM_FREE)
            {
                if(!newSeparation)
                {
                    newSeparation = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }
                if(!prevSeparation)
                {
                    prevSeparation = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }

                newCodeCoveSeparation = VirtualAllocEx(p, ADDRESS(memoryRegionStart), shsize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
                if(newCodeCoveSeparation)
                {
                    break;
                }
            }
            // WHY?!?!
            memoryRegionStart += 4096;
            // This doesn't work! It seems we allocate WAAAAY to far (outside the 32-bit jump)
            //memoryRegionStart = (DWORD64)mBI.BaseAddress + mBI.RegionSize;
        }

        if(newCodeCoveSeparation)
        {
            // Store the original code that we will inject -> for reverting!
            ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress)), &_originalSeparationCode, 8, NULL);

            // Write our CodeCave in our allocated memory space
            WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation)), shellcode.c_str(), shsize, NULL);

            // Write our registry offset inside the Code Cave!
            {
                // Read the currently running registries so we update our code cave with it!
                char originalRegistryHoldingValue[4] = "";
                ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress + 0x4)), originalRegistryHoldingValue, 4, NULL);

                // Write it in our Code Cave!
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x2D), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x54), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x7A), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x83), &originalRegistryHoldingValue, 4, NULL);
            }

            // Write the Separation addresses
            {
                // Write the new Separation memory locations
                // Offset Addr =    Actual Addr         Current Position in the code    length of the addr (4 bytes always)
                DWORD64 temp = (DWORD64)newSeparation - ((DWORD64)newCodeCoveSeparation + 0x05 + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x05), &temp, 4, NULL);

                temp = (DWORD64)prevSeparation - ((DWORD64)newCodeCoveSeparation + 0x16 + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x16), &temp, 4, NULL);

                temp = (DWORD64)newSeparation - ((DWORD64)newCodeCoveSeparation + 0x34 + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x34), &temp, 4, NULL);

                temp = (DWORD64)prevSeparation - ((DWORD64)newCodeCoveSeparation + 0x3B + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x3B), &temp, 4, NULL);

                temp = (DWORD64)newSeparation - ((DWORD64)newCodeCoveSeparation + 0x6C + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x6C), &temp, 4, NULL);

                temp = (DWORD64)prevSeparation - ((DWORD64)newCodeCoveSeparation + 0x73 + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x73), &temp, 4, NULL);
            }

            // Write all our jumps back to the original code
            {
                // Now Write the return memory address
                // 1nd JUMP
                DWORD64 jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveSeparation) + 0x58 + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + 0x58 + 0x1), &jumpBackAddress, 4, NULL);

                // Now Write the return memory address
                // 2nd JUMP
                // Write the jump in the code cave
                std::string jmpCC = "\xE9\x90\x90\x90\x90";
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + shsize), jmpCC.c_str(), jmpCC.length(), NULL);

                jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveSeparation) + shsize + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveSeparation) + shsize + 0x01), &jumpBackAddress, 4, NULL);
            }

            // Write the JUMP to our CODE CAVE and thus enabling the code cave!
            {
                void *tempAddress = VirtualAllocEx(p, NULL, m_jumpToCodeCave.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                /// Write the JUMP to our address
                WriteProcessMemory(p, ADDRESS(tempAddress), m_jumpToCodeCave.c_str(), m_jumpToCodeCave.length(), NULL);

                /// Now Write the memory address where the code-cave was allocated
                //DWORD64 jumpAddress = DWORD64(newCodeCoveSeparation) - signatureAddress - (/*A jump is always 5bytes */ 0x05);
                DWORD64 jumpAddress = DWORD64(newCodeCoveSeparation) - signatureAddress - (/*A jump is always 5bytes */ 0x05);
                WriteProcessMemory(p, ADDRESS((DWORD64)tempAddress + 1), &jumpAddress, 4, NULL);

                // Copy the whole thing to the new location
                void *jmpBack = malloc(m_jumpToCodeCave.length());
                ReadProcessMemory(p, ADDRESS((DWORD64)(tempAddress)), jmpBack, m_jumpToCodeCave.length(), NULL);
                WriteProcessMemory(p, ADDRESS(signatureAddress), jmpBack, m_jumpToCodeCave.length(), NULL);

                free(jmpBack);
                VirtualFreeEx(p, tempAddress, m_jumpToCodeCave.length(), MEM_DECOMMIT);
            }
            CloseHandle(p);
            return true;
        }
    }
    CloseHandle(p);
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::injectCodeCaveConvergence64(const DWORD64 baseAddress, const DWORD64 signatureAddress, const std::string &shellcode)
{
    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 0, _exePid);

    if(p)
    {
        // Get our number of bytes to write
        unsigned long shsize = (unsigned long)shellcode.length();

        // Find our block of free space to allocate
        // so we can still do 32bit long jump in code-caves
        MEMORY_BASIC_INFORMATION mBI = { 0 };
        DWORD64 memoryRegionStart = baseAddress;
        DWORD64 memoryRegionEnd = MAX_ADDR_TO_SCAN;

        while(memoryRegionStart < memoryRegionEnd)
        {
            memset(&mBI, 0, sizeof(MEMORY_BASIC_INFORMATION));
            VirtualQueryEx(p, ADDRESS(memoryRegionStart), &mBI, sizeof(MEMORY_BASIC_INFORMATION));

            if(mBI.State == MEM_FREE)
            {
                if(!crtConvergence)
                {
                    crtConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }
                if(!newConvergence)
                {
                    newConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }
                if(!prevConvergence)
                {
                    prevConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }

                newCodeCoveConvergence = VirtualAllocEx(p, ADDRESS(memoryRegionStart), shsize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
                if(newCodeCoveConvergence)
                {
                    break;
                }
            }
            // WHY?!?!
            memoryRegionStart += 4096;
            // This doesn't work! It seems we allocate WAAAAY to far (outside the 32-bit jump)
            //memoryRegionStart = (DWORD64)mBI.BaseAddress + mBI.RegionSize;
        }

        if(newCodeCoveConvergence)
        {
            // Store the original code that we will inject -> for reverting!
            ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress)), &_originalConvergenceCode, 8, NULL);

            // Write our CodeCave in our allocated memory space
            WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence)), shellcode.c_str(), shsize, NULL);

            // Write our registry offset inside the Code Cave!
            {
                // Read the currently running registries so we update our code cave with it!
                char originalRegistryHoldingValue[4] = "";
                ReadProcessMemory(p, ADDRESS((DWORD64)(signatureAddress + 0x4)), originalRegistryHoldingValue, 4, NULL);

                // Write it in our Code Cave!
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x2F), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x48), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x4F), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x85), &originalRegistryHoldingValue, 4, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x8E), &originalRegistryHoldingValue, 4, NULL);
            }

            // Write the Convergence addresses
            {
                // Write the new Separation memory locations
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x04), &newConvergence, 8, NULL);

                // Offset Addr =    Actual Addr         Current Position in the code    length of the addr (4 bytes always)
                DWORD64 temp = (DWORD64)prevConvergence - ((DWORD64)newCodeCoveConvergence + 0x18 + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x18), &temp, 4, NULL);

                temp = (DWORD64)newConvergence - ((DWORD64)newCodeCoveConvergence + 0x36 + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x36), &temp, 4, NULL);

                temp = (DWORD64)prevConvergence - ((DWORD64)newCodeCoveConvergence + 0x3D + 0x04);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x3D), &temp, 4, NULL);

                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x54), &crtConvergence, 8, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x70), &newConvergence, 8, NULL);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x7A), &prevConvergence, 8, NULL);
            }

            // Write all our jumps back to the original code
            {
                // Now Write the return memory address
                // 1nd JUMP
                DWORD64 jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveConvergence) + 0x5D + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + 0x5D + 0x1), &jumpBackAddress, 4, NULL);

                // Now Write the return memory address
                // 2nd JUMP
                // Write the jump in the code cave
                std::string jmpCC = "\xE9\x90\x90\x90\x90";
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + shsize), jmpCC.c_str(), jmpCC.length(), NULL);

                jumpBackAddress = (signatureAddress + 0x05) - (DWORD64(newCodeCoveConvergence) + shsize + 0x05);
                WriteProcessMemory(p, ADDRESS(DWORD64(newCodeCoveConvergence) + shsize + 0x01), &jumpBackAddress, 4, NULL);
            }

            // Write the JUMP to our CODE CAVE and thus enabling the code cave!
            {
                void *tempAddress = VirtualAllocEx(p, NULL, m_jumpToCodeCave.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                /// Write the JUMP to our address
                WriteProcessMemory(p, ADDRESS(tempAddress), m_jumpToCodeCave.c_str(), m_jumpToCodeCave.length(), NULL);

                /// Now Write the memory address where the code-cave was allocated
                DWORD64 jumpAddress = DWORD64(newCodeCoveConvergence) - signatureAddress - (/*A jump is always 5bytes */ 0x05);
                WriteProcessMemory(p, ADDRESS((DWORD64)tempAddress + 1), &jumpAddress, 4, NULL);

                // Copy the whole thing to the new location
                void *jmpBack = malloc(m_jumpToCodeCave.length());
                ReadProcessMemory(p, ADDRESS((DWORD64)(tempAddress)), jmpBack, m_jumpToCodeCave.length(), NULL);
                WriteProcessMemory(p, ADDRESS(signatureAddress), jmpBack, m_jumpToCodeCave.length(), NULL);

                free(jmpBack);
                VirtualFreeEx(p, tempAddress, m_jumpToCodeCave.length(), MEM_DECOMMIT);
            }
            CloseHandle(p);
            return true;
        }
    }
    CloseHandle(p);
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::DoPatching(const std::string &gameExeName)
{
    // Get the module address from the EXE
    _exePid = getPid(gameExeName);

    if(_exePid)
    {
        if(_is64Bit)
            console_log("Game executable %s (x64) found!\n", gameExeName.c_str());
        else
            console_log("Game executable %s (x86) found!\n", gameExeName.c_str());
    }

    DWORD64 baseAddress = -1;

    if(_is64Bit)
        baseAddress = getModuleBaseAddress(m_moduleName64);
    else
        baseAddress = getModuleBaseAddress(m_moduleName32);

    if(baseAddress == -1)
    {
        console_log("!!! Could not open the specified EXE !!!\n");
        console_log("Make sure the EXE name is correctly specified in \"3DVision_CM_Unleashed.ini\"!\n");
        console_log("Make sure to run this tool as Administrator (RUN AS ADMIN)!\n\n");
        PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
        return false;
    }

    // Separation Code Cave
    console_log("Starting Signature Scanning for Separation Patch... (this might take a while)\n");
    if(_is64Bit)
        _signatureSeparationAddress = findSignature(m_moduleName64, m_signatureSeparation64);
    else
        _signatureSeparationAddress = findSignature(m_moduleName32, m_signatureSeparation32);

    if(_signatureSeparationAddress)
    {
        if(_is64Bit)
            _sepPatchDone = injectCodeCaveSeparation64(baseAddress, _signatureSeparationAddress, m_shellcodeSeparation64);
        else
            _sepPatchDone = injectCodeCaveSeparation32(baseAddress, _signatureSeparationAddress, m_shellcodeSeparation32);

        if(_sepPatchDone)
        {
            console_log("Separation Patch Applied!\n");
            PlaySound(TEXT("DeviceConnect"), NULL, SND_ALIAS | SND_ASYNC);
        }
        else
        {
            console_log("!!! Could not apply the Separation Patch !!!\n");
            PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
        }
    }
    else
    {
        console_log("!!! Could not find the Signature for the Separation Patch !!!\n");
        PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
    }

    // Convergence Code Cave
    console_log("Starting Signature Scanning for Convergence Patch... (this might take a while)\n");
    if(_is64Bit)
        _signatureConvergeAddress = findSignature(m_moduleName64, m_signatureConvergence64);
    else
        _signatureConvergeAddress = findSignature(m_moduleName32, m_signatureConvergence32);

    if(_signatureConvergeAddress)
    {
        if(_is64Bit)
            _convPatchDone = injectCodeCaveConvergence64(baseAddress, _signatureConvergeAddress, m_shellcodeConvergence64);
        else
            _convPatchDone = injectCodeCaveConvergence32(baseAddress, _signatureConvergeAddress, m_shellcodeConvergence32);

        if(_convPatchDone)
        {
            console_log("Convergence Patch Applied!\n");
            PlaySound(TEXT("DeviceConnect"), NULL, SND_ALIAS);
        }
        else
        {
            console_log("!!! Could not apply the Convergence Patch !!!\n");
            PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS);
        }
    }
    else
    {
        console_log("!!! Could not find the Signature for the Convergence Patch !!!\n");
        PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS);
    }

    if(_sepPatchDone && _convPatchDone)
    {
        console_log("!!! Both Separation & Convergence Patches Applied !!!\n");
        console_log("-------------------------------------------------------------------\n");
        console_log("You can now use the custom Separation and Convergence!\n");
        console_log("-------------------------------------------------------------------\n\n");
        PlaySound("C:\\Windows\\media\\tada.wav", NULL, SND_ASYNC);
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed ::RestoreOriginal(std::string &gameExeName)
{
    console_log("-------------------------------------------------------------------\n");
    console_log("Starting Cleanup...\n");

    float defaultSep = 1.0f;
    CM_SetSeparationFactor(&defaultSep);
    // Give some time to actually be used!
    Sleep(100);

    // Get our process handle
    HANDLE p = OpenProcess(PROCESS_ALL_ACCESS, 0, _exePid);

    if(p)
    {
        // Restore the stuff
        if(_signatureSeparationAddress)
        {
            if(WriteProcessMemory(p, ADDRESS(DWORD64(_signatureSeparationAddress)), _originalSeparationCode, 8, NULL))
            {
                _signatureSeparationAddress = 0;
                console_log("Restored Original Separation Code\n");
                PlaySound(TEXT("DeviceDisconnect"), NULL, SND_ALIAS | SND_ASYNC);
            }
            else
            {
                console_log("!!! Could not restore Original Separation Code !!!\n");
                PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
            }
        }
        else
            console_log("Nothing to Restore! Separation Patch is not applied !\n");

        if(_signatureConvergeAddress)
        {
            if(WriteProcessMemory(p, ADDRESS(DWORD64(_signatureConvergeAddress)), _originalConvergenceCode, 8, NULL))
            {
                _signatureConvergeAddress = 0;
                console_log("Restored Original Convergence Code\n");
                PlaySound(TEXT("DeviceDisconnect"), NULL, SND_ALIAS | SND_ASYNC);
            }
            else
            {
                console_log("!!! Could not restore Original Convergence Code !!!\n");
                PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
            }
        }
        else
            console_log("Nothing to Restore! Convergence Patch is not applied !\n");

        // Free our CodeCave code
        if(newCodeCoveSeparation)
        {
            VirtualFreeEx(p, ADDRESS(DWORD64(newCodeCoveSeparation)), (unsigned long)m_shellcodeSeparation64.length(), MEM_DECOMMIT);
            newCodeCoveSeparation = 0;
        }

        if(newCodeCoveConvergence)
        {
            VirtualFreeEx(p, ADDRESS(DWORD64(newCodeCoveConvergence)), (unsigned long)m_shellcodeConvergence64.length(), MEM_DECOMMIT);
            newCodeCoveConvergence = 0;
        }

        if(newSeparation)
        {
            VirtualFreeEx(p, ADDRESS(DWORD64(newSeparation)), sizeof(float), MEM_DECOMMIT);
            newSeparation = 0;
        }

        if(prevSeparation)
        {
            VirtualFreeEx(p, ADDRESS(DWORD64(prevSeparation)), sizeof(float), MEM_DECOMMIT);
            prevSeparation = 0;
        }

        if(crtConvergence)
        {
            VirtualFreeEx(p, ADDRESS(DWORD64(crtConvergence)), sizeof(float), MEM_DECOMMIT);
            crtConvergence = 0;
        }

        if(newConvergence)
        {
            VirtualFreeEx(p, ADDRESS(DWORD64(newConvergence)), sizeof(float), MEM_DECOMMIT);
            newConvergence = 0;
        }

        if(prevConvergence)
        {
            VirtualFreeEx(p, ADDRESS(DWORD64(prevConvergence)), sizeof(float), MEM_DECOMMIT);
            prevConvergence = 0;
        }

        console_log("Finished freeing up all the resources !!!\n");
        console_log("-------------------------------------------------------------------\n\n");
        CloseHandle(p);
        return true;
    }
    else
    {
        console_log("Application has exited! Nothing to restore!\n");
    }
    console_log("-------------------------------------------------------------------\n\n");
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::CM_SetConvergence(float *newValue)
{
    if(_convPatchDone)
    {
        HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, 0, _exePid);
        BOOL ok = WriteProcessMemory(p, ADDRESS((DWORD64)newConvergence), newValue, sizeof(float), NULL);
        CloseHandle(p);

        if(ok)
            return true;
    }
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::CM_GetConvergence(float *newValue)
{
    if(_convPatchDone)
    {
        HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ, 0, _exePid);
        BOOL ok = ReadProcessMemory(p, ADDRESS((DWORD64)(crtConvergence)), newValue, sizeof(float), NULL);
        CloseHandle(p);
        if(ok)
            return true;
    }
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::CM_SetSeparationFactor(float *factor)
{
    if(_sepPatchDone)
    {
        if(_separationFactor == 0.0f)
        {
            // Store the original Separation Value!
            if(!CM_GetActualSeparation(&_origSeparation))
                return false;
        }

        // Update the separation
        _separationFactor = *factor;
        float newSep = _origSeparation * _separationFactor;
        HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, 0, _exePid);
        BOOL ok = WriteProcessMemory(p, ADDRESS((DWORD64)newSeparation), &newSep, sizeof(float), NULL);
        CloseHandle(p);

        if(ok)
            return true;
    }
    return false;
}
//-----------------------------------------------------------------------------

bool CMUnleashed::CM_GetSeparationFactor(float *factor)
{
    if(_sepPatchDone)
    {
        if(_separationFactor != 0.0f)
        {
            *factor = _separationFactor;
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------

// PRIVATE FUNCTION
bool CMUnleashed::CM_GetActualSeparation(float *newValue)
{
    if(_sepPatchDone)
    {
        float crtSep = 0;
        HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ, 0, _exePid);
        BOOL ok = ReadProcessMemory(p, ADDRESS((DWORD64)(newSeparation)), &crtSep, sizeof(float), NULL);
        CloseHandle(p);

        if(ok)
        {
            // Apply the inverse
            *newValue = crtSep;
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
