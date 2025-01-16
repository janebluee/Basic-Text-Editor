#include "TextEditor.h"
#include <fstream>
#include <sstream>
#include <CommCtrl.h>
#include <Richedit.h>

TextEditor::TextEditor(HWND parentWindow) : hwndParent(parentWindow) {
    Create();
}

TextEditor::~TextEditor() {
    if (hwndEdit) {
        DestroyWindow(hwndEdit);
    }
}

void TextEditor::Create() {
    RECT rc;
    GetClientRect(hwndParent, &rc);

    hwndEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        0, 0,
        rc.right - rc.left,
        rc.bottom - rc.top,
        hwndParent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    defaultEditProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(
        hwndEdit,
        GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(EditProc)
    ));

    SetWindowLongPtr(hwndEdit, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    HFONT hFont = CreateFont(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Consolas"
    );

    SendMessage(hwndEdit, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
}

void TextEditor::SetText(const std::wstring& text) {
    SetWindowText(hwndEdit, text.c_str());
}

std::wstring TextEditor::GetText() const {
    int length = GetWindowTextLength(hwndEdit);
    if (length == 0) return L"";

    std::vector<wchar_t> buffer(length + 1);
    GetWindowText(hwndEdit, buffer.data(), length + 1);
    return std::wstring(buffer.data());
}

bool TextEditor::SaveFile(const std::wstring& filename) {
    std::wofstream file(filename);
    if (!file.is_open()) return false;

    file << GetText();
    file.close();

    currentFile = filename;
    return true;
}

bool TextEditor::LoadFile(const std::wstring& filename) {
    std::wifstream file(filename);
    if (!file.is_open()) return false;

    std::wstringstream buffer;
    buffer << file.rdbuf();
    SetText(buffer.str());

    currentFile = filename;
    return true;
}

void TextEditor::Find(const std::wstring& searchText, bool matchCase) {
    if (searchText.empty()) return;

    DWORD flags = FR_DOWN;
    if (!matchCase) flags |= FR_MATCHCASE;

    int textLength = GetWindowTextLength(hwndEdit);
    if (textLength == 0) return;

    std::vector<wchar_t> buffer(textLength + 1);
    GetWindowText(hwndEdit, buffer.data(), textLength + 1);
    std::wstring content(buffer.data());

    size_t pos = content.find(searchText);
    if (pos != std::wstring::npos) {
        SendMessage(hwndEdit, EM_SETSEL, pos, pos + searchText.length());
        SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0);
    }
}

LRESULT TextEditor::EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TextEditor* editor = reinterpret_cast<TextEditor*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!editor) return DefWindowProc(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_TAB) {
                SendMessage(hwnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L"    "));
                return 0;
            }
            break;
    }

    return CallWindowProc(editor->defaultEditProc, hwnd, msg, wParam, lParam);
}
