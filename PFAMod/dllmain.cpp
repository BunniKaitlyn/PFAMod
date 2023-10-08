#include <windows.h>
#include <detours.h>
#include <ppl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "util.h"
#include "proxy.h"

class Renderer {
public:
    enum FontSize { Small, SmallBold, SmallComic, Medium, Large };

    Renderer(void) : m_iBufferWidth(0), m_iBufferHeight(0), m_bLimitFPS(true) {};
    virtual ~Renderer(void) {};

    virtual HRESULT Init(HWND hWnd, bool bLimitFPS) = 0;
    virtual HRESULT ResetDeviceIfNeeded() = 0;
    virtual HRESULT ResetDevice() = 0;
    virtual HRESULT Clear(DWORD color) = 0;
    virtual HRESULT BeginScene() = 0;
    virtual HRESULT EndScene() = 0;
    virtual HRESULT Present() = 0;
    virtual HRESULT BeginText() = 0;
    virtual HRESULT DrawTextW(const WCHAR* sText, FontSize fsFont, LPRECT rcPos, DWORD dwFormat, DWORD dwColor, INT iChars = -1) = 0;
    virtual HRESULT DrawTextA(const CHAR* sText, FontSize fsFont, LPRECT rcPos, DWORD dwFormat, DWORD dwColor, INT iChars = -1) = 0;
    virtual HRESULT EndText() = 0;
    virtual HRESULT DrawRect(float x, float y, float cx, float cy, DWORD color) = 0;
    virtual HRESULT DrawRect(float x, float y, float cx, float cy,
        DWORD c1, DWORD c2, DWORD c3, DWORD c4) = 0;
    virtual HRESULT DrawSkew(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, DWORD color) = 0;
    virtual HRESULT DrawSkew(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,
        DWORD c1, DWORD c2, DWORD c3, DWORD c4) = 0;

    bool GetLimitFPS() const { return m_bLimitFPS; }
    HRESULT SetLimitFPS(bool bLimitFPS);

    int GetBufferWidth() const { return m_iBufferWidth; }
    int GetBufferHeight() const { return m_iBufferHeight; }

protected:
    int m_iBufferWidth, m_iBufferHeight;
    bool m_bLimitFPS;
};

struct MainScreen {
    char m_pad0[0x10];
    Renderer* m_pRenderer;
};

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

int playedNotes = 0;

void (__fastcall* MainScreen__RenderStatus)(MainScreen*, LPRECT) = nullptr;
void __fastcall MainScreen__RenderStatusHook(MainScreen* thisptr, LPRECT prcStatus) {
    TCHAR sNotesCount[128];
    _stprintf_s(sNotesCount, TEXT("%d"), playedNotes);

    RECT customRect;
    CopyRect(&customRect, prcStatus);

    OffsetRect(&customRect, 8, 56 + 1);
    thisptr->m_pRenderer->DrawText(TEXT("Played Notes:"), Renderer::Small, &customRect, 0, 0xFF404040);
    thisptr->m_pRenderer->DrawText(sNotesCount, Renderer::Small, &customRect, DT_RIGHT, 0xFF404040);
    OffsetRect(&customRect, -2, -1);
    thisptr->m_pRenderer->DrawText(TEXT("Played Notes:"), Renderer::Small, &customRect, 0, 0xFFFFFFFF);
    thisptr->m_pRenderer->DrawText(sNotesCount, Renderer::Small, &customRect, DT_RIGHT, 0xFFFFFFFF);

    MainScreen__RenderStatus(thisptr, prcStatus);
}

MMRESULT (__stdcall* o_midiOutShortMsg)(HMIDIOUT, DWORD) = midiOutShortMsg;
MMRESULT __stdcall midiOutShortMsgHook(HMIDIOUT hmo, DWORD dwMsg) {
    MMRESULT result = o_midiOutShortMsg(hmo, dwMsg);

    BYTE bStatus = dwMsg & 0xff;
    if ((bStatus & 0x80) == 0x80 || (bStatus & 0x90) == 0x90) {
        playedNotes++;
    }

    return result;
}

void WINAPI Main() {
    //util::InitConsole();

    MIDITrack__clear = decltype(MIDITrack__clear)(util::GetBaseAddress() + 0x2B8B0);
    MIDITrack__ParseEvents = decltype(MIDITrack__ParseEvents)(util::GetBaseAddress() + 0x2BA50);
    MIDITrack__ParseTrack = decltype(MIDITrack__ParseEvents)(util::GetBaseAddress() + 0x2B990);
    //MIDI__ConnectNotes = decltype(MIDI__ConnectNotes)(util::GetBaseAddress() + 0x2B6B0);
    MainScreen__RenderStatus = decltype(MainScreen__RenderStatus)(util::GetBaseAddress() + 0x1FEA0);

    DetourTransactionBegin();
    DetourAttach(&(PVOID&)MIDITrack__ParseTrack, MIDITrack__ParseTrackHook);
    DetourTransactionCommit();

    //DetourTransactionBegin();
    //DetourAttach(&(PVOID&)MIDI__ConnectNotes, MIDI__ConnectNotesHook);
    //DetourTransactionCommit();

    DetourTransactionBegin();
    DetourAttach(&(PVOID&)MainScreen__RenderStatus, MainScreen__RenderStatusHook);
    DetourTransactionCommit();

    DetourTransactionBegin();
    DetourAttach(&(PVOID&)o_midiOutShortMsg, midiOutShortMsgHook);
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
