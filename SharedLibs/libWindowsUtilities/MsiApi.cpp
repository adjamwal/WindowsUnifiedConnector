#include "pch.h"
#include "MsiApi.h"

MsiApi::MsiApi( IWinApiWrapper& winApiWrapper ) :
    m_winApiWrapper( winApiWrapper )
{

}

MsiApi::~MsiApi()
{
}

std::tuple<int32_t, std::vector<MsiApiProductInfo>> MsiApi::FindProductsByName( std::wstring productName )
{
    return QueryProducts( NULL, productName, L"" );
}

std::tuple<int32_t, std::vector<MsiApiProductInfo>> MsiApi::FindProductsByPublisher( std::wstring productPublisher )
{
    return QueryProducts( NULL, L"", productPublisher );
}

std::tuple<int32_t, std::vector<MsiApiProductInfo>> MsiApi::FindProductsByNameAndPublisher( std::wstring productName, std::wstring productPublisher )
{
    return QueryProducts( NULL, productName, productPublisher );
}

std::tuple<int32_t, std::vector<MsiApiProductInfo>> MsiApi::FindRelatedProducts(std::wstring upgradeCode)
{
    int32_t retValue = 0;
    DWORD dwIndex = 0;
    TCHAR szProductCode[39] = { 0 };
    std::vector<MsiApiProductInfo> products;

    do {
        retValue = m_winApiWrapper.MsiEnumRelatedProductsW(
            upgradeCode.c_str(),
            0,
            dwIndex,
            szProductCode );

        if ( retValue == ERROR_SUCCESS )
        {
            auto[retValue, p] = QueryProducts( szProductCode, L"", L"" );

            products.insert( std::end( products ), std::begin( p ), std::end( p ) );
        }

        dwIndex++;
    } while ( retValue == ERROR_SUCCESS );

    return { (ERROR_NO_MORE_ITEMS == retValue) ? ERROR_SUCCESS : retValue, products };
}

std::tuple<int32_t, std::vector<MsiApiProductInfo>> MsiApi::QueryProducts(
    LPCWSTR productCode,
    std::wstring productName,
    std::wstring productPublisher )
{
    std::vector<MsiApiProductInfo> products;

    int32_t retValue = 0;
    DWORD dwIndex = 0;
    TCHAR szInstalledProductCode[39] = { 0 };
    TCHAR szSid[128] = { 0 };
    DWORD pcchSid = 128;
    MSIINSTALLCONTEXT pdwInstalledContext = {};

    do {
        pcchSid = 128;

        retValue = m_winApiWrapper.MsiEnumProductsExW(
            productCode,
            L"s-1-1-0",
            MSIINSTALLCONTEXT_ALL,
            dwIndex,
            szInstalledProductCode,
            &pdwInstalledContext,
            szSid,
            &pcchSid );

        if ( retValue == ERROR_SUCCESS )
        {
            MsiApiProductInfo currentProduct;

            currentProduct = GetProductInformation(
                szInstalledProductCode,
                pdwInstalledContext,
                szSid,
                pcchSid );

            if ( VerifyMsiMatchesNameAndPublisher( currentProduct, productName, productPublisher ) ||
                    VerifyMsiMatchesNameOnly( currentProduct, productName, productPublisher ) || 
                    VerifyMsiMatchesPublisherOnly( currentProduct, productName, productPublisher ) || 
                    ( productName.empty() && productPublisher.empty() ) ) {
                products.emplace_back( currentProduct );
            }

            dwIndex++;
        }
    } while ( retValue == ERROR_SUCCESS );

    return { (ERROR_NO_MORE_ITEMS == retValue) ? ERROR_SUCCESS : retValue, products };
}

MsiApiProductInfo MsiApi::GetProductInformation(
    WCHAR* szInstalledProductCode, 
    MSIINSTALLCONTEXT pdwInstalledContext, 
    LPWSTR szSid, 
    DWORD pcchSid )
{
    MsiApiProductInfo product;
    std::wstring propertyStr;
    LPCTSTR tempSzUserSid = NULL;

    if ( pcchSid != 0 ) {
        tempSzUserSid = szSid;
        product.UserSid = std::wstring( szSid );
    }

    product.InstalledProductCode = std::wstring( szInstalledProductCode );
    product.MsiInstallContext = pdwInstalledContext;

    //Commented out to improve performance. Might still be valid information in the future.
    //product.InstallState = m_winApiWrapper.MsiQueryProductStateW( szInstalledProductCode );

    MsiApiProductProperties properties;
    properties.InstalledProductName = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLEDPRODUCTNAME );
    properties.Publisher = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_PUBLISHER );
    properties.VersionString = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_VERSIONSTRING );

    //Commented out to improve performance. Might still be valid information in the future.
    //properties.InstallDate = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLDATE );
    //properties.InstallLocation = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLLOCATION );
    //properties.InstallSource = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLSOURCE );
    //properties.LocalPackage = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_LOCALPACKAGE );
    //properties.AssignmentType = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_ASSIGNMENTTYPE );
    
    product.Properties = properties;

    return product;
}

std::wstring MsiApi::QueryProperty(
    LPCTSTR szProductCode, 
    LPCTSTR szUserSid, 
    MSIINSTALLCONTEXT dwContext, 
    LPCTSTR szProperty )
{
    std::wstring retString(L"");
    std::vector<WCHAR> lpValue;

    DWORD pcchValue = 0;
    UINT retValue = m_winApiWrapper.MsiGetProductInfoEx( szProductCode, szUserSid, dwContext, szProperty, NULL, &pcchValue );

    if ( retValue == ERROR_SUCCESS )
    {
        pcchValue++;
        lpValue.resize( pcchValue );
        retValue = m_winApiWrapper.MsiGetProductInfoEx( szProductCode, szUserSid, dwContext, szProperty, lpValue.data(), &pcchValue );
    
        if ( retValue == ERROR_SUCCESS && lpValue.data() != nullptr )
        {
            retString = std::wstring( lpValue.data() );
        }
    }

    return retString;
}

bool MsiApi::VerifyMsiMatchesNameAndPublisher( MsiApiProductInfo& productInfo,
    std::wstring productName,
    std::wstring productPublisher )
{
    bool rtn = false;
    if ( !productName.empty() &&
            !productPublisher.empty() &&
            productName.compare( productInfo.Properties.InstalledProductName ) == 0 &&
            productPublisher.compare( productInfo.Properties.Publisher ) == 0 ) {
        rtn =  true;
    }

    return rtn;
}

bool MsiApi::VerifyMsiMatchesNameOnly( MsiApiProductInfo& productInfo,
    std::wstring productName,
    std::wstring productPublisher )
{
    bool rtn = false;

    if ( !productName.empty() &&
            productPublisher.empty() &&
            productName.compare( productInfo.Properties.InstalledProductName ) == 0 ) {
        rtn = true;
    }

    return rtn;
}

bool MsiApi::VerifyMsiMatchesPublisherOnly( MsiApiProductInfo& productInfo,
    std::wstring productName,
    std::wstring productPublisher )
{
    bool rtn = false;
    if ( productName.empty() &&
        !productPublisher.empty() &&
        productPublisher.compare( productInfo.Properties.Publisher ) == 0 ) {
        rtn = true;
    }

    return rtn;
}