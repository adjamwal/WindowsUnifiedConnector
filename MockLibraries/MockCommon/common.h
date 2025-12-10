#pragma once
#ifndef MOCK_COMMON_H
#define MOCK_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

// Mock common library functions
// Provides stubs for Cisco common library dependencies

__declspec(dllexport) int Common_Init();
__declspec(dllexport) void Common_Log(const char* message);
__declspec(dllexport) int Common_GetVersion(char* buffer, int size);
__declspec(dllexport) void Common_Shutdown();

#ifdef __cplusplus
}
#endif

#endif // MOCK_COMMON_H
