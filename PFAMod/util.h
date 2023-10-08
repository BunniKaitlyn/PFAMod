#pragma once
#include <windows.h>
#include <string>

class util {
public:
    static std::string GetConcatPath(const std::string& first, const std::string& second) {
        std::string temp = first;
        if (first[first.length()] != '\\') {
            temp += '\\';
            return temp + second;
        } else {
            return first + second;
        }
    }

    static std::string GetDirPath(const std::string& file_name) {
        const size_t last = file_name.rfind('\\');
        if (std::string::npos != last) {
            return file_name.substr(0, last);
        }
        return std::string();
    }

    static std::string GetSysDir() {
        char dir[MAX_PATH];
        GetSystemDirectoryA(dir, MAX_PATH);
        return dir;
    }

    static void InitConsole() {
        AllocConsole();

        FILE* pFile;
        freopen_s(&pFile, "CONOUT$", "w", stdout);
    }

    static uintptr_t GetBaseAddress() {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    }
};
