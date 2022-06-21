#pragma once

#include <cstdint>
#include <tuple>
#include <string>
#include <vector>

#include "IWinApiWrapper.h"
#include <Msi.h>

struct MsiApiProductProperties
{
    std::wstring InstalledProductName;
    std::wstring Publisher;
    std::wstring VersionString;
    //std::wstring InstallDate;
    //std::wstring InstallLocation;
    //std::wstring InstallSource;
    //std::wstring LocalPackage;
    //std::wstring AssignmentType;
};

struct MsiApiProductInfo
{
    INSTALLSTATE InstallState;
    MSIINSTALLCONTEXT MsiInstallContext;
    std::wstring InstalledProductCode;
    std::wstring UserSid;
    MsiApiProductProperties Properties;
};

class IMsiApi
{
public:
    IMsiApi() {}
    virtual ~IMsiApi() {}

    virtual int32_t QueryProducts( std::vector<MsiApiProductInfo>& products ) = 0;

    virtual std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindProductsByName( 
        std::wstring displayName ) = 0;

    virtual std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindProductsByPublisher( 
        std::wstring publisher ) = 0;

    virtual std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindProductsByNameAndPublisher( 
        std::wstring displayName,
        std::wstring publisher ) = 0;

    virtual std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindRelatedProducts( 
        std::wstring upgradeCode ) = 0;

    virtual bool IsMsiServiceReadyforInstall() = 0;
};