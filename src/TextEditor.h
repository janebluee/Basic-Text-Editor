#pragma once
#include <windows.h>
#include <string>
#include <vector>

class TextEditor {
public:
    TextEditor(HWND parentWindow);
    ~TextEditor();

    void Create();
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    bool SaveFile(const std::wstring& filename);
    bool LoadFile(const std::wstring& filename);
    void Find(const std::wstring& searchText, bool matchCase);
    HWND GetHandle() const { return hwndEdit; }

private:
    HWND hwndEdit;
    HWND hwndParent;
    std::wstring currentFile;
    
    static LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    WNDPROC defaultEditProc;
};
