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
            HANDLE p = OpenProcess(PROCESS_QUERY_INFORMATION, 0, pid);
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

bool CMUnleashed::findSignature(const std::string _process, const std::string &moduleName, const std::vector<BYTE> &bytes_to_find, DWORD64 *baseAddress, DWORD64 *signatureAddress)
{
    // Initial Setup
    *baseAddress = 0;
    *signatureAddress = 0;
    bool _foundSig = false;

    HANDLE hsnap;
    PROCESSENTRY32 pt;
    hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pt.dwSize = sizeof(PROCESSENTRY32);
    while(Process32Next(hsnap, &pt) && !_foundSig)
    {
        if(!strcmp(pt.szExeFile, _process.c_str()))
        {
            DWORD pid = pt.th32ProcessID;
            BOOL is32 = FALSE;
            HANDLE p = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ, 0, pid);
            if(p)
            {
                char szBuf[MAX_PATH * 100] = { 0 };
                PBYTE pb = NULL;
                MEMORY_BASIC_INFORMATION mbi;

                while(VirtualQueryEx(p, pb, &mbi, sizeof(mbi)) == sizeof(mbi) && !_foundSig)
                {
                    if(mbi.Type)
                    {
                        // Grab the module Name.
                        // (Nice work-around ^_^)
                        char szModName[MAX_PATH]{ 0 };
                        GetMappedFileName(p, pb, szModName, _countof(szModName));
                        std::string temp = std::string(szModName);
                        if(temp.find(m_moduleName64) != std::string::npos)
                        {
                            // Start the memory search!
                            MEMORY_BASIC_INFORMATION mbiModule{};
                            BYTE *address = static_cast<BYTE *>(ADDRESS(mbi.BaseAddress));
                            BYTE *address_high = address + mbi.RegionSize;

                            while(address < address_high && VirtualQuery(address, std::addressof(mbiModule), sizeof(mbiModule)))
                            {
                                BYTE *memoryCopy = new BYTE[mbiModule.RegionSize];
                                // Anti-cheat doesn't allow READ or WRITE -_-
                                ReadProcessMemory(p, address, memoryCopy, mbiModule.RegionSize, nullptr);

                                // The DLL that we are looking for has these properties :)
                                if(mbiModule.State == MEM_FREE && mbiModule.Protect == PAGE_NOACCESS)
                                {
                                    const BYTE *begin = static_cast<const BYTE *>(memoryCopy);
                                    const BYTE *end = begin + mbiModule.RegionSize;

                                    // Search based on our lambda function that accepts wildcards
                                    const BYTE *found = std::search(begin, end, bytes_to_find.begin(), bytes_to_find.end(),
                                        [](char fromText, char fromPattern) { return fromPattern == '?' || fromPattern == fromText; });

                                    if(found != end)
                                    {
                                        // We found the address
                                        DWORD64 offset = (DWORD64)found - (DWORD64)begin;
                                        *baseAddress = (DWORD64)mbiModule.BaseAddress;
                                        *signatureAddress = (DWORD64)mbiModule.BaseAddress + offset;
                                        _foundSig = true;
                                        delete[] memoryCopy;
                                        break;
                                    }
                                }
                                // Delete and increment
                                delete[] memoryCopy;
                                address += mbiModule.RegionSize;
                                mbiModule = {};
                            }
                        }
                    }
                    pb += mbi.RegionSize;
                }
                // end if (p)
                CloseHandle(p);
            }
        }
    }
    CloseHandle(hsnap);
    return _foundSig;
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

                // Now Write the memory address where the code-cave was allocated
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
        console_log("-----------------------------------------------------------------------------\n");
        if(_is64Bit)
            console_log("Game executable %s (x64) found!\n", gameExeName.c_str());
        else
            console_log("Game executable %s (x86) found!\n", gameExeName.c_str());
        console_log("-----------------------------------------------------------------------------\n");
    }

    // Separation Code Cave
    console_log("-----------------------------------------------------------------------------\n");
    console_log("Detecting Entry Point for Separation Patching...\n");
    if(_is64Bit)
        findSignature(gameExeName, m_moduleName64, m_signatureSeparation64, &_moduleBaseAddress, &_signatureSeparationAddress);
    else
        findSignature(gameExeName, m_moduleName32, m_signatureSeparation32, &_moduleBaseAddress, &_signatureSeparationAddress);

    if(_signatureSeparationAddress)
    {
        if(_is64Bit)
            _sepPatchDone = injectCodeCaveSeparation64(_moduleBaseAddress, _signatureSeparationAddress, m_shellcodeSeparation64);
        else
            _sepPatchDone = injectCodeCaveSeparation32(_moduleBaseAddress, _signatureSeparationAddress, m_shellcodeSeparation32);

        if(_sepPatchDone)
        {
            console_log("Separation Patch Applied!\n");
            PlaySound(TEXT("DeviceConnect"), NULL, SND_ALIAS | SND_ASYNC);
        }
        else
        {
            console_log("!!! Could not apply Separation Patch !!!\n");
            PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
        }
    }
    else
    {
        console_log("!!! Could not find Entry Point for Separation Patching !!!\n");
        PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
    }
    console_log("-----------------------------------------------------------------------------\n");

    // Convergence Code Cave
    console_log("-----------------------------------------------------------------------------\n");
    console_log("Detecting Entry Point for Convergence Patching...\n");
    if(_is64Bit)
        findSignature(gameExeName, m_moduleName64, m_signatureConvergence64, &_moduleBaseAddress, &_signatureConvergeAddress);
    else
        findSignature(gameExeName, m_moduleName32, m_signatureConvergence32, &_moduleBaseAddress, &_signatureConvergeAddress);

    if(_signatureConvergeAddress)
    {
        if(_is64Bit)
            _convPatchDone = injectCodeCaveConvergence64(_moduleBaseAddress, _signatureConvergeAddress, m_shellcodeConvergence64);
        else
            _convPatchDone = injectCodeCaveConvergence32(_moduleBaseAddress, _signatureConvergeAddress, m_shellcodeConvergence32);

        if(_convPatchDone)
        {
            console_log("Convergence Patch Applied!\n");
            PlaySound(TEXT("DeviceConnect"), NULL, SND_ALIAS | SND_ASYNC);
        }
        else
        {
            console_log("!!! Could not apply Convergence Patch !!!\n");
            PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
        }
    }
    else
    {
        console_log("!!! Could not find Entry Point for Convergence Patching !!!\n");
        PlaySound(TEXT("CriticalStop"), NULL, SND_ALIAS | SND_ASYNC);
    }
    console_log("-----------------------------------------------------------------------------\n");

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
    HANDLE p = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, 0, _exePid);

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

        // Reset Memory Locations and prepare for a new run!
        ResetMemoryLocations();

        return true;
    }
    else
    {
        console_log("Application has exited! Nothing to restore!\n");
    }
    console_log("-------------------------------------------------------------------\n\n");

    // Reset Memory Locations and prepare for a new run!
    ResetMemoryLocations();

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
        HANDLE p = OpenProcess(PROCESS_VM_READ, 0, _exePid);
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
        HANDLE p = OpenProcess(PROCESS_VM_READ, 0, _exePid);
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
