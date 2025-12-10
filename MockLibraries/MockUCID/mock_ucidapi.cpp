#include "ucidapi.h"
#include <string.h>
#include <stdio.h>

// Mock implementations for UCID API
// These provide stub functionality for POC builds

int UCID_Initialize(UCID_HANDLE* handle) {
    printf("[MOCK UCID] Initialize called\n");
    // Return success for POC
    *handle = (UCID_HANDLE)0x12345678;
    return 0;
}

int UCID_GetIdentity(UCID_HANDLE handle, char* buffer, int size) {
    const char* mockId = "mock-identity-12345-67890";
    printf("[MOCK UCID] GetIdentity called\n");
    if (buffer && size > 0) {
        strncpy_s(buffer, size, mockId, _TRUNCATE);
        return 0;
    }
    return -1;
}

int UCID_GetSessionToken(UCID_HANDLE handle, char* buffer, int size) {
    const char* mockToken = "mock-session-token-abcdef-123456";
    printf("[MOCK UCID] GetSessionToken called\n");
    if (buffer && size > 0) {
        strncpy_s(buffer, size, mockToken, _TRUNCATE);
        return 0;
    }
    return -1;
}

int UCID_GetBusinessId(UCID_HANDLE handle, char* buffer, int size) {
    const char* mockBizId = "mock-business-id-999";
    printf("[MOCK UCID] GetBusinessId called\n");
    if (buffer && size > 0) {
        strncpy_s(buffer, size, mockBizId, _TRUNCATE);
        return 0;
    }
    return -1;
}

int UCID_GetBackendUrl(UCID_HANDLE handle, char* buffer, int size) {
    const char* mockUrl = "https://mock-backend.example.com";
    printf("[MOCK UCID] GetBackendUrl called\n");
    if (buffer && size > 0) {
        strncpy_s(buffer, size, mockUrl, _TRUNCATE);
        return 0;
    }
    return -1;
}

void UCID_Cleanup(UCID_HANDLE handle) {
    printf("[MOCK UCID] Cleanup called\n");
    // Nothing to clean up in mock
}
