#include "common.h"
#include <stdio.h>
#include <string.h>

// Mock implementations for common library functions

int Common_Init() {
    printf("[MOCK COMMON] Init called\n");
    return 0;  // Success
}

void Common_Log(const char* message) {
    if (message) {
        printf("[MOCK COMMON LOG] %s\n", message);
    }
}

int Common_GetVersion(char* buffer, int size) {
    const char* version = "1.0.0-mock-common";
    printf("[MOCK COMMON] GetVersion called\n");
    if (buffer && size > 0) {
        strncpy_s(buffer, size, version, _TRUNCATE);
        return 0;
    }
    return -1;
}

void Common_Shutdown() {
    printf("[MOCK COMMON] Shutdown called\n");
}
