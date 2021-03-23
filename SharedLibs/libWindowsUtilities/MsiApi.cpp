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
            auto[ret, p] = QueryProducts( szProductCode, L"", L"" );

            products.insert( std::end( products ), std::begin( p ), std::end( p ) );

            if( ret != ERROR_SUCCESS )
            {
                //TODO log error? not sure how to handle this logic
            }
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
    MSIINSTALLCONTEXT pdwInstalledContext;

    do {
        retValue = m_winApiWrapper.MsiEnumProductsExW(
            productCode,
            L"s-1-1-0",
            MSIINSTALLCONTEXT_USERMANAGED | MSIINSTALLCONTEXT_USERUNMANAGED | MSIINSTALLCONTEXT_MACHINE,
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

            if ( !productName.empty() || !productPublisher.empty() )
            {
                if ( productName.compare( currentProduct.Properties.InstalledProductName ) == 0 ||
                    productPublisher.compare( currentProduct.Properties.Publisher) == 0)
                {
                    products.emplace_back( currentProduct );
                }
            }
            else
            {
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
    product.InstallState = m_winApiWrapper.MsiQueryProductStateW( szInstalledProductCode );

    MsiApiProductProperties properties;
    properties.InstalledProductName = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLEDPRODUCTNAME );
    properties.Publisher = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_PUBLISHER );
    properties.VersionString = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_VERSIONSTRING );
    properties.InstallDate = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLDATE );
    properties.InstallLocation = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLLOCATION );
    properties.InstallSource = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_INSTALLSOURCE );
    properties.LocalPackage = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_LOCALPACKAGE );
    properties.AssignmentType = QueryProperty( szInstalledProductCode, tempSzUserSid, pdwInstalledContext, INSTALLPROPERTY_ASSIGNMENTTYPE );
    
    product.Properties = properties;

    return product;
}

std::wstring MsiApi::QueryProperty(
    LPCTSTR szProductCode, 
    LPCTSTR szUserSid, 
    MSIINSTALLCONTEXT dwContext, 
    LPCTSTR szProperty )
{
    WCHAR* lpValue = new WCHAR[MAX_PATH]{ 0 };

    DWORD pcchValue = 0;
    UINT retValue = m_winApiWrapper.MsiGetProductInfoEx( szProductCode, szUserSid, dwContext, szProperty, NULL, &pcchValue );

    if ( retValue == ERROR_SUCCESS )
    {
        pcchValue++;
        retValue = m_winApiWrapper.MsiGetProductInfoEx( szProductCode, szUserSid, dwContext, szProperty, lpValue, &pcchValue );
    }

    if ( retValue != ERROR_SUCCESS )
    {
        //TODO log error
    }

    return std::wstring( lpValue );
}
