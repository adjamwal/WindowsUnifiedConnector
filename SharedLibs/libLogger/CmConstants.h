#pragma once

#define CMID_MODULE_INTERFACE_VERSION   1u
#define WREG_CMID                       L"Software\\Cisco\\SecureClient\\Cloud Management\\CMID"
#define WREG_CMID_X64                   L"Software\\Cisco\\SecureClient\\Cloud Management\\CMID\\x64"
#define WREG_CMID_X86                   L"Software\\Cisco\\SecureClient\\Cloud Management\\CMID\\x86"
#define WREG_CM_CONFIG                  L"Software\\Cisco\\SecureClient\\Cloud Management\\config"
#define WREG_CMPM                       L"Software\\Cisco\\SecureClient\\Cloud Management\\CMPM"
#define WREG_CM_SERVICE                 L"Software\\Cisco\\SecureClient\\Cloud Management\\CMSERVICE"
#define WREG_CM                         L"Software\\Cisco\\SecureClient\\Cloud Management"

#define CMID_API_GET_ID_FUNCTION_NAME           "cmid_get_id"
#define CMID_API_GET_TOKEN_FUNCTION_NAME        "cmid_get_token"
#define CMID_API_REFRESH_TOKEN_FUNCTION_NAME    "cmid_refresh_token"
#define CMID_API_GET_URL_FUNCTION_NAME          "cmid_get_url"

#if defined(_WIN64)
#define CMID_API_DLL_KEY    WREG_CMID_X64
#else
#define CMID_API_DLL_KEY    WREG_CMID_X86
#endif

