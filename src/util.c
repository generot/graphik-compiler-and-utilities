#include "../include/util.h"

char *read_file(char *filename) {
    HANDLE hFile = CreateFileA(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if(hFile == INVALID_HANDLE_VALUE)
        return NULL;

    DWORD fSize = GetFileSize(hFile, NULL), bytesRead;
    char *buffer = malloc(fSize + 1);

    ReadFile(hFile, (LPVOID)buffer, fSize, &bytesRead, NULL);
    buffer[fSize] = '\0';

    return buffer;
}