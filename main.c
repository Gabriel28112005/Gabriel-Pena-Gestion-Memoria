
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define SIZE 4096

int main() {
    HANDLE hMapFile;
    char *shared_memory;

    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // Use paging file
        NULL,                    // Default security
        PAGE_READWRITE,          // Read/write access
        0,                       // Maximum object size (high-order DWORD)
        SIZE,                    // Maximum object size (low-order DWORD)
        "SharedMemory");         // Name of mapping object

    if (hMapFile == NULL) {
        printf("Could not create file mapping object (%d).\n", GetLastError());
        return 1;
    }

    shared_memory = (char *)MapViewOfFile(hMapFile,   // Handle to map object
                                          FILE_MAP_ALL_ACCESS, // Read/write permission
                                          0,
                                          0,
                                          SIZE);

    if (shared_memory == NULL) {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcess(NULL, "child_process.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // Parent process
        strcpy(shared_memory, "Hello, child process!");
        WaitForSingleObject(pi.hProcess, INFINITE);
        UnmapViewOfFile(shared_memory);
        CloseHandle(hMapFile);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        // Child process
        printf("Child reads: %s\n", shared_memory);
        UnmapViewOfFile(shared_memory);
        CloseHandle(hMapFile);
        exit(0);
    }

    return 0;
}