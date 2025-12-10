#pragma once
#ifndef MOCK_UCIDAPI_H
#define MOCK_UCIDAPI_H

#ifdef __cplusplus
extern "C" {
#endif

// Mock UCID API - Minimal interface for compilation
typedef void* UCID_HANDLE;

// Mock function declarations
__declspec(dllexport) int UCID_Initialize(UCID_HANDLE* handle);
__declspec(dllexport) int UCID_GetIdentity(UCID_HANDLE handle, char* buffer, int size);
__declspec(dllexport) int UCID_GetSessionToken(UCID_HANDLE handle, char* buffer, int size);
__declspec(dllexport) int UCID_GetBusinessId(UCID_HANDLE handle, char* buffer, int size);
__declspec(dllexport) int UCID_GetBackendUrl(UCID_HANDLE handle, char* buffer, int size);
__declspec(dllexport) void UCID_Cleanup(UCID_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif // MOCK_UCIDAPI_H
