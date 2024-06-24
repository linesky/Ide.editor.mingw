#include <windows.h>
#include <stdio.h>
#include <string.h>

#define IDC_TEXTAREA 101
#define IDC_FILENAME 102
#define IDC_BTN_NEW 103
#define IDC_BTN_LOAD 104
#define IDC_BTN_SAVE 105
#define IDC_BTN_COMPILE 106
#define IDC_BTN_RUN 107
//gcc -o ide ide.c -mwindows

// Função para substituir a extensão do arquivo
void replace_extension(char *filename, const char *new_ext) {
    char *dot = strrchr(filename, '.');
    if (dot) {
        strcpy(dot, new_ext);
    } else {
        strcat(filename, new_ext);
    }
}

// Função para carregar o arquivo
void load_file(const char *filename, HWND hwndTextArea) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *buffer = (char *)malloc(length + 1);
        fread(buffer, 1, length, file);
        buffer[length] = '\0';
        SetWindowText(hwndTextArea, buffer);
        free(buffer);
        fclose(file);
    } else {
        MessageBox(NULL, "Could not open file.", "Error", MB_OK | MB_ICONERROR);
    }
}

// Função para salvar o arquivo
void save_file(const char *filename, HWND hwndTextArea) {
    FILE *file = fopen(filename, "w");
    if (file) {
        int length = GetWindowTextLength(hwndTextArea);
        char *buffer = (char *)malloc(length + 1);
        GetWindowText(hwndTextArea, buffer, length + 1);
        fwrite(buffer, 1, length, file);
        free(buffer);
        fclose(file);
    } else {
        MessageBox(NULL, "Could not save file.", "Error", MB_OK | MB_ICONERROR);
    }
}

// Função de callback da janela
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndTextArea, hwndFileName, hwndBtnNew, hwndBtnLoad, hwndBtnSave, hwndBtnCompile, hwndBtnRun;
    switch (uMsg) {
        case WM_CREATE: {
            hwndTextArea = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                                         10, 50, 760, 500, hwnd, (HMENU)IDC_TEXTAREA, NULL, NULL);
            hwndFileName = CreateWindowW(L"EDIT", L"new.c", WS_CHILD | WS_VISIBLE | WS_BORDER,
                                         10, 10, 200, 25, hwnd, (HMENU)IDC_FILENAME, NULL, NULL);
            hwndBtnNew = CreateWindowW(L"BUTTON", L"New", WS_CHILD | WS_VISIBLE,
                                       220, 10, 80, 30, hwnd, (HMENU)IDC_BTN_NEW, NULL, NULL);
            hwndBtnLoad = CreateWindowW(L"BUTTON", L"Load", WS_CHILD | WS_VISIBLE,
                                        310, 10, 80, 30, hwnd, (HMENU)IDC_BTN_LOAD, NULL, NULL);
            hwndBtnSave = CreateWindowW(L"BUTTON", L"Save", WS_CHILD | WS_VISIBLE,
                                        400, 10, 80, 30, hwnd, (HMENU)IDC_BTN_SAVE, NULL, NULL);
            hwndBtnCompile = CreateWindowW(L"BUTTON", L"Compile", WS_CHILD | WS_VISIBLE,
                                           490, 10, 80, 30, hwnd, (HMENU)IDC_BTN_COMPILE, NULL, NULL);
            hwndBtnRun = CreateWindowW(L"BUTTON", L"Run", WS_CHILD | WS_VISIBLE,
                                       580, 10, 80, 30, hwnd, (HMENU)IDC_BTN_RUN, NULL, NULL);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDC_BTN_NEW) {
                SetWindowText(hwndTextArea, "");
            } else if (LOWORD(wParam) == IDC_BTN_LOAD) {
                char filename[260];
                GetWindowText(hwndFileName, filename, sizeof(filename));
                load_file(filename, hwndTextArea);
            } else if (LOWORD(wParam) == IDC_BTN_SAVE) {
                char filename[260];
                GetWindowText(hwndFileName, filename, sizeof(filename));
                save_file(filename, hwndTextArea);
            } else if (LOWORD(wParam) == IDC_BTN_COMPILE) {
                char filename[260];
                GetWindowText(hwndFileName, filename, sizeof(filename));
                char command[300];
                sprintf(command, "gcc %s -o ", filename);
                replace_extension(filename, ".exe");
                strcat(command, filename);
                system(command);
            } else if (LOWORD(wParam) == IDC_BTN_RUN) {
                char filename[260];
                GetWindowText(hwndFileName, filename, sizeof(filename));
                replace_extension(filename, ".exe");
                system(filename);
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Função principal
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MyIDEClass";
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 0)); // Fundo amarelo
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, "IDE", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

