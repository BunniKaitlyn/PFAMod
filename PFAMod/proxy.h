#pragma once
#include <windows.h>

typedef DWORD (WINAPI* ORIG_VerFindFileA)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT, LPSTR, PUINT);
typedef DWORD (WINAPI* ORIG_VerFindFileW)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
typedef DWORD (WINAPI* ORIG_VerInstallFileA)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT);
typedef DWORD (WINAPI* ORIG_VerInstallFileW)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoSizeW)(LPCWSTR, LPDWORD);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoA)(LPCSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoSizeExA)(DWORD, LPCSTR, LPDWORD);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoSizeExW)(DWORD, LPCWSTR, LPDWORD);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoExA)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI* ORIG_GetFileVersionInfoExW)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI* ORIG_VerLanguageNameA)(DWORD, LPSTR, DWORD);
typedef DWORD (WINAPI* ORIG_VerLanguageNameW)(DWORD, LPWSTR, DWORD);
typedef DWORD (WINAPI* ORIG_VerQueryValueA)(LPCVOID, LPCSTR, LPVOID*, PUINT);
typedef DWORD (WINAPI* ORIG_VerQueryValueW)(LPCVOID, LPCWSTR, LPVOID*, PUINT);
typedef void (WINAPI* ORIG_GetFileVersionInfoByHandle)();

ORIG_VerFindFileA o_VerFindFileA;
ORIG_VerFindFileW o_VerFindFileW;
ORIG_VerInstallFileA o_VerInstallFileA;
ORIG_VerInstallFileW o_VerInstallFileW;
ORIG_GetFileVersionInfoSizeA o_GetFileVersionInfoSizeA;
ORIG_GetFileVersionInfoSizeW o_GetFileVersionInfoSizeW;
ORIG_GetFileVersionInfoA o_GetFileVersionInfoA;
ORIG_GetFileVersionInfoW o_GetFileVersionInfoW;
ORIG_GetFileVersionInfoSizeExA o_GetFileVersionInfoSizeExA;
ORIG_GetFileVersionInfoSizeExW o_GetFileVersionInfoSizeExW;
ORIG_GetFileVersionInfoExA o_GetFileVersionInfoExA;
ORIG_GetFileVersionInfoExW o_GetFileVersionInfoExW;
ORIG_VerLanguageNameA o_VerLanguageNameA;
ORIG_VerLanguageNameW o_VerLanguageNameW;
ORIG_VerQueryValueA o_VerQueryValueA;
ORIG_VerQueryValueW o_VerQueryValueW;
ORIG_GetFileVersionInfoByHandle o_GetFileVersionInfoByHandle;

DWORD WINAPI VerFindFileA(DWORD uFlags, LPCSTR szFileName, LPCSTR szWinDir, LPCSTR szAppDir, LPSTR szCurDir, PUINT puCurDirLen, LPSTR szDestDir, PUINT puDestDirLen) {
    return (o_VerFindFileA)(uFlags, szFileName, szWinDir, szAppDir, szCurDir, puCurDirLen, szDestDir, puDestDirLen);
}

DWORD WINAPI VerFindFileW(DWORD uFlags, LPCWSTR szFileName, LPCWSTR szWinDir, LPCWSTR szAppDir, LPWSTR szCurDir, PUINT puCurDirLen, LPWSTR szDestDir, PUINT puDestDirLen) {
    return (o_VerFindFileW)(uFlags, szFileName, szWinDir, szAppDir, szCurDir, puCurDirLen, szDestDir, puDestDirLen);
}

DWORD WINAPI VerInstallFileA(DWORD uFlags, LPCSTR szSrcFileName, LPCSTR szDestFileName, LPCSTR szSrcDir, LPCSTR szDestDir, LPCSTR szCurDir, LPSTR szTmpFile, PUINT puTmpFileLen) {
    return (o_VerInstallFileA)(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, puTmpFileLen);
}

DWORD WINAPI VerInstallFileW(DWORD uFlags, LPCWSTR szSrcFileName, LPCWSTR szDestFileName, LPCWSTR szSrcDir, LPCWSTR szDestDir, LPCWSTR szCurDir, LPWSTR szTmpFile, PUINT puTmpFileLen) {
    return (o_VerInstallFileW)(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, puTmpFileLen);
}

DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle) {
    return (o_GetFileVersionInfoSizeA)(lptstrFilename, lpdwHandle);
}

DWORD WINAPI GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle) {
    return (o_GetFileVersionInfoSizeW)(lptstrFilename, lpdwHandle);
}

BOOL WINAPI GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    return (o_GetFileVersionInfoA)(lptstrFilename, dwHandle, dwLen, lpData);
}

BOOL WINAPI GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    return (o_GetFileVersionInfoW)(lptstrFilename, dwHandle, dwLen, lpData);
}

DWORD WINAPI GetFileVersionInfoSizeExA(DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle) {
    return (o_GetFileVersionInfoSizeExA)(dwFlags, lpwstrFilename, lpdwHandle);
}

DWORD WINAPI GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
    return (o_GetFileVersionInfoSizeExW)(dwFlags, lpwstrFilename, lpdwHandle);
}

BOOL WINAPI GetFileVersionInfoExA(DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    return (o_GetFileVersionInfoExA)(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

BOOL WINAPI GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    return (o_GetFileVersionInfoExW)(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

DWORD WINAPI VerLanguageNameA(DWORD wLang, LPSTR szLang, DWORD cchLang) {
    return (o_VerLanguageNameA)(wLang, szLang, cchLang);
}

DWORD WINAPI VerLanguageNameW(DWORD wLang, LPWSTR szLang, DWORD cchLang) {
    return (o_VerLanguageNameW)(wLang, szLang, cchLang);
}

BOOL WINAPI VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    return (o_VerQueryValueA)(pBlock, lpSubBlock, lplpBuffer, puLen);
}

BOOL WINAPI VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    return (o_VerQueryValueW)(pBlock, lpSubBlock, lplpBuffer, puLen);
}

void WINAPI GetFileVersionInfoByHandle() {
    (o_GetFileVersionInfoByHandle)();
}
