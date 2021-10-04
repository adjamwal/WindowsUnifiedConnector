#pragma once

#include "cmidapi.h"
#include <cstdio>
#include <Windows.h>
#include <string>
#include "PmTypes.h"

class ICodesignVerifier;

class UCIDApiDll
{
public:
    UCIDApiDll( ICodesignVerifier& codeSignVerifier );
    virtual ~UCIDApiDll();

    int32_t GetId( std::string& id );
    int32_t GetToken(std::string& token );
    int32_t RefreshToken();
    int32_t GetUrls( PmUrlList& urls );

    bool LoadApi();
    void UnloadApi();

private:
    typedef cmid_result_t( *GetIdFunc )(IN OUT char*, IN OUT int* );
    typedef cmid_result_t( *GetTokenFunc )(IN OUT char*, IN OUT int* );
    typedef cmid_result_t( *RefreshTokenFunc )();
    typedef cmid_result_t( *GetUrlFunc)( IN cmid_url_type_t, IN OUT char*, IN OUT int* );
    bool LoadDll(const std::wstring dllPath);
    void UnloadDll();
    cmid_result_t GetUrl( cmid_url_type_t urlType, std::string &url );

    ICodesignVerifier& m_codeSignVerifier;
    HMODULE m_api;
    GetIdFunc m_getIdFunc;
    GetTokenFunc m_getTokenFunc;
    RefreshTokenFunc m_refreshTokenFunc;
    GetUrlFunc m_getUrlFunc;
    std::wstring m_loadedDllName;
    bool m_isModuleLoaded;
};
