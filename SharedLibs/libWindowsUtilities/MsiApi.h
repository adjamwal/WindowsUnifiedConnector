#pragma once

#include "IMsiApi.h"

class MsiApi : public IMsiApi
{
public:
    MsiApi( IWinApiWrapper& winApiWrapper );
    virtual ~MsiApi();

    std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindProductsByName(
        std::wstring productName ) override;

    std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindProductsByPublisher(
        std::wstring productPublisher ) override;

    std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindProductsByNameAndPublisher(
        std::wstring productName,
        std::wstring productPublisher ) override;

    std::tuple<int32_t, std::vector<MsiApiProductInfo>> FindRelatedProducts( 
        std::wstring upgradeCode ) override;
private:
    IWinApiWrapper& m_winApiWrapper;

    std::tuple<int32_t, std::vector<MsiApiProductInfo>> QueryProducts(
        LPCWSTR productCode,
        std::wstring productName,
        std::wstring productPublisher );

    MsiApiProductInfo GetProductInformation(
        WCHAR* szInstalledProductCode,
        MSIINSTALLCONTEXT pdwInstalledContext,
        LPWSTR szSid,
        DWORD pcchSid );

    std::wstring QueryProperty( LPCTSTR szProductCode,
        LPCTSTR szUserSid,
        MSIINSTALLCONTEXT dwContext,
        LPCTSTR szProperty );
};
