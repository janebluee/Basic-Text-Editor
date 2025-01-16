#pragma once
#include <windows.h>
#include <string>

class Window {
public:
    Window(const wchar_t* title, int width, int height);
    ~Window();

    bool ProcessMessages();
    HWND GetHandle() const { return hwnd; }

private:
    HWND hwnd;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
