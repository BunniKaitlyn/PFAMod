#include <windows.h>
#include <detours.h>
#include <string>
#include "util.h"
#include "proxy.h"

namespace MIDI {
    int Parse32Bit(const unsigned char* pcData, int iMaxSize, int* piOut) {
        if (!pcData || !piOut || iMaxSize < 4) {
            return 0;
        }
        *piOut = pcData[0];
        *piOut = (*piOut << 8) | pcData[1];
        *piOut = (*piOut << 8) | pcData[2];
        *piOut = (*piOut << 8) | pcData[3];
        return 4;
    }

    int ParseNChars(const unsigned char* pcData, int iNChars, int iMaxSize, char* pcOut) {
        if (!pcData || !pcOut || iMaxSize <= 0) {
            return 0;
        }
        int iSize = min(iNChars, iMaxSize);
        memcpy(pcOut, pcData, iSize);
        return iSize;
    }
}

// funcs
void (__fastcall* MIDITrack__clear)(void* thisptr) = nullptr;
int (__fastcall* MIDITrack__ParseEvents)(void* thisptr, unsigned char* pcData, int iMaxSize, int iTrack) = nullptr;

// bugfix hook for certain MIDIs crashing PFA.
// ported from pfaviz.
int  (__fastcall* MIDITrack__ParseTrack)(void*, unsigned char*, int, int) = nullptr;
int __fastcall MIDITrack__ParseTrackHook(void* thisptr, unsigned char* pcData, int iMaxSize, int iTrack) {
    char pcBuf[4];
    int iTotal, iTrkSize;

    // Reset first
    MIDITrack__clear(thisptr);

    // Read header
    if (MIDI::ParseNChars(pcData, 4, iMaxSize, pcBuf) != 4) {
        return 0;
    }
    if (MIDI::Parse32Bit(pcData + 4, iMaxSize - 4, &iTrkSize) != 4) {
        return 0;
    }
    iTotal = 8;

    // Check header
    if (strncmp(pcBuf, "MTrk", 4) != 0) {
        return 0;
    }

    MIDITrack__ParseEvents(thisptr, pcData + iTotal, iMaxSize - iTotal, iTrack);
    return iTotal + iTrkSize;
}

void WINAPI Main() {
    util::InitConsole();

    MIDITrack__clear = decltype(MIDITrack__clear)(util::GetBaseAddress() + 0x2B8B0);
    MIDITrack__ParseEvents = decltype(MIDITrack__ParseEvents)(util::GetBaseAddress() + 0x2BA50);
    MIDITrack__ParseTrack = decltype(MIDITrack__ParseEvents)(util::GetBaseAddress() + 0x2B990);

    DetourTransactionBegin();
    DetourAttach(&(PVOID&)MIDITrack__ParseTrack, MIDITrack__ParseTrackHook);
    DetourTransactionCommit();
}

BOOL WINAPI DllInit() {
    std::string sysDir = util::GetSysDir();
    HMODULE hLibrary = LoadLibraryA(util::GetConcatPath(sysDir, "version.dll").c_str());
    if (hLibrary) {
        o_VerFindFileA = (ORIG_VerFindFileA)GetProcAddress(hLibrary, "VerFindFileA");
        o_VerFindFileW = (ORIG_VerFindFileW)GetProcAddress(hLibrary, "VerFindFileW");
        o_VerInstallFileA = (ORIG_VerInstallFileA)GetProcAddress(hLibrary, "VerInstallFileA");
        o_VerInstallFileW = (ORIG_VerInstallFileW)GetProcAddress(hLibrary, "VerInstallFileW");
        o_GetFileVersionInfoA = (ORIG_GetFileVersionInfoA)GetProcAddress(hLibrary, "GetFileVersionInfoA");
        o_GetFileVersionInfoW = (ORIG_GetFileVersionInfoW)GetProcAddress(hLibrary, "GetFileVersionInfoW");
        o_GetFileVersionInfoSizeA = (ORIG_GetFileVersionInfoSizeA)GetProcAddress(hLibrary, "GetFileVersionInfoSizeA");
        o_GetFileVersionInfoSizeW = (ORIG_GetFileVersionInfoSizeW)GetProcAddress(hLibrary, "GetFileVersionInfoSizeW");
        o_GetFileVersionInfoExA = (ORIG_GetFileVersionInfoExA)GetProcAddress(hLibrary, "GetFileVersionInfoExA");
        o_GetFileVersionInfoExW = (ORIG_GetFileVersionInfoExW)GetProcAddress(hLibrary, "GetFileVersionInfoExW");
        o_GetFileVersionInfoSizeExA = (ORIG_GetFileVersionInfoSizeExA)GetProcAddress(hLibrary, "GetFileVersionInfoSizeExA");
        o_GetFileVersionInfoSizeExW = (ORIG_GetFileVersionInfoSizeExW)GetProcAddress(hLibrary, "GetFileVersionInfoSizeExW");
        o_VerLanguageNameA = (ORIG_VerLanguageNameA)GetProcAddress(hLibrary, "VerLanguageNameA");
        o_VerLanguageNameW = (ORIG_VerLanguageNameW)GetProcAddress(hLibrary, "VerLanguageNameW");
        o_VerQueryValueA = (ORIG_VerQueryValueA)GetProcAddress(hLibrary, "VerQueryValueA");
        o_VerQueryValueW = (ORIG_VerQueryValueW)GetProcAddress(hLibrary, "VerQueryValueW");
        o_GetFileVersionInfoByHandle = (ORIG_GetFileVersionInfoByHandle)GetProcAddress(hLibrary, "GetFileVersionInfoByHandle");
        return TRUE;
    }
    return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DllInit();
        Main();
    }
    return TRUE;
}
