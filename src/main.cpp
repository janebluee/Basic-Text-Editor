#include "Window.h"
#include "TextEditor.h"
#include <windows.h>
#include "resource.h"
#include <string>
#include <memory>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void HandleMenuCommand(HWND hwnd, TextEditor* editor, WPARAM wParam);
void OpenFileDialog(HWND hwnd, TextEditor* editor);
void SaveFileDialog(HWND hwnd, TextEditor* editor);
void FindDialog(HWND hwnd, TextEditor* editor);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    Window window(L"Basic Text Editor", 800, 600);
    TextEditor editor(window.GetHandle());

    SetWindowLongPtr(window.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&editor));
    SetWindowLongPtr(window.GetHandle(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINMENU));
    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(window.GetHandle(), hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    TextEditor* editor = reinterpret_cast<TextEditor*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_COMMAND:
            if (editor) {
                HandleMenuCommand(hwnd, editor, wParam);
            }
            return 0;

        case WM_SIZE: {
            if (editor) {
                RECT rcClient;
                GetClientRect(hwnd, &rcClient);
                SetWindowPos(editor->GetHandle(), nullptr,
                    0, 0,
                    rcClient.right - rcClient.left,
                    rcClient.bottom - rcClient.top,
                    SWP_NOZORDER);
            }
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void HandleMenuCommand(HWND hwnd, TextEditor* editor, WPARAM wParam) {
    switch (LOWORD(wParam)) {
        case ID_FILE_NEW:
            editor->SetText(L"");
            break;

        case ID_FILE_OPEN:
            OpenFileDialog(hwnd, editor);
            break;

        case ID_FILE_SAVE:
            SaveFileDialog(hwnd, editor);
            break;

        case ID_FILE_SAVEAS:
            SaveFileDialog(hwnd, editor);
            break;

        case ID_FILE_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;

        case ID_EDIT_CUT:
            SendMessage(editor->GetHandle(), WM_CUT, 0, 0);
            break;

        case ID_EDIT_COPY:
            SendMessage(editor->GetHandle(), WM_COPY, 0, 0);
            break;

        case ID_EDIT_PASTE:
            SendMessage(editor->GetHandle(), WM_PASTE, 0, 0);
            break;

        case ID_EDIT_FIND:
            FindDialog(hwnd, editor);
            break;
    }
}

void OpenFileDialog(HWND hwnd, TextEditor* editor) {
    wchar_t fileName[MAX_PATH];
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"txt";

    fileName[0] = '\0';

    if (GetOpenFileName(&ofn)) {
        editor->LoadFile(fileName);
    }
}

void SaveFileDialog(HWND hwnd, TextEditor* editor) {
    wchar_t fileName[MAX_PATH];
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"txt";

    fileName[0] = '\0';

    if (GetSaveFileName(&ofn)) {
        editor->SaveFile(fileName);
    }
}

void FindDialog(HWND hwnd, TextEditor* editor) {
    static wchar_t findBuffer[256] = {};
    wchar_t* findText = findBuffer;

    if (DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_FIND_DIALOG), hwnd,
        [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
            switch (msg) {
                case WM_INITDIALOG:
                    SetDlgItemText(hwnd, IDC_FIND_TEXT, findBuffer);
                    return TRUE;

                case WM_COMMAND:
                    switch (LOWORD(wParam)) {
                        case IDOK: {
                            GetDlgItemText(hwnd, IDC_FIND_TEXT, findBuffer, 256);
                            EndDialog(hwnd, IDOK);
                            return TRUE;
                        }
                        case IDCANCEL:
                            EndDialog(hwnd, IDCANCEL);
                            return TRUE;
                    }
                    break;
            }
            return FALSE;
        }, 0) == IDOK) {
        bool matchCase = (IsDlgButtonChecked(hwnd, IDC_MATCH_CASE) == BST_CHECKED);
        editor->Find(findText, matchCase);
    }
}
