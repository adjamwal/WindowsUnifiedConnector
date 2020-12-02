#pragma once

#include "ucidapi.h"
#include <cstdio>
#include <Windows.h>
#include <string>

class ICodesignVerifier;

class UCIDApiDll
{
public:
    UCIDApiDll( ICodesignVerifier& codeSignVerifier );
    virtual ~UCIDApiDll();

    int32_t GetId( std::string& id );
    int32_t GetToken(std::string& token );
    int32_t RefreshToken();

    bool LoadApi();
    void UnloadApi();

private:
    typedef ucid_result_t( *GetIdFunc )(IN OUT char* p_id, IN OUT int* p_buflen);
    typedef ucid_result_t( *GetTokenFunc )(IN OUT char* p_token, IN OUT int* p_buflen);
    typedef ucid_result_t( *RefreshTokenFunc )();

    bool LoadDll(const std::wstring dllPath);
    void UnloadDll();

    ICodesignVerifier& m_codeSignVerifier;
    HMODULE m_api;
    GetIdFunc m_getIdFunc;
    GetTokenFunc m_getTokenFunc;
    RefreshTokenFunc m_refreshTokenFunc;
    std::wstring m_loadedDllName;
    bool m_isModuleLoaded;
};
