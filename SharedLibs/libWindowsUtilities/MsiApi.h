#pragma once

#include "IMsiApi.h"

class MsiApi : public IMsiApi
{
public:
    MsiApi( IWinApiWrapper& winApiWrapper );

    virtual ~MsiApi();

    int32_t QueryProducts( std::vector<MsiApiProductInfo>& products ) override;

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

    bool VerifyMsiMatchesNameAndPublisher( MsiApiProductInfo& productInfo, 
        std::wstring productName,
        std::wstring productPublisher );

    bool VerifyMsiMatchesNameOnly( MsiApiProductInfo& productInfo, 
        std::wstring productName,
        std::wstring productPublisher );

    bool VerifyMsiMatchesPublisherOnly( MsiApiProductInfo& productInfo, 
        std::wstring productName,
        std::wstring productPublisher );
};
