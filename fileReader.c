#include "headers/fileReader.h"

FILE* OpenExplorer(SDL_Window* win, const char * flag){
    OPENFILENAME ofn;
    char filePath[MAX_PATH];

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(win, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = filePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filePath);
    ofn.lpstrFilter = "ROM Files (*.)\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    return fopen(filePath, flag);
}

void GetFileToStr(FILE *f, char **stor){
    unsigned long fileSize;
    fseek(f, 0, SEEK_END);
    fileSize = ftell(f);
    rewind(f);
    *stor = (char *)malloc(fileSize + 1);
    size_t bytes = fread(*stor, sizeof(char), fileSize, f);
    char nullTerm = '\0';
    memcpy(*stor + bytes, &nullTerm, 1);
}