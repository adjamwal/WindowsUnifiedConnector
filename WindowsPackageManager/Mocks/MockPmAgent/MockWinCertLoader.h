#pragma once

#include <gmock/gmock.h>
#include "IWinCertLoader.h"

class MockWinCertLoader : public IWinCertLoader
{
public:
    MockWinCertLoader();
    ~MockWinCertLoader();

    MOCK_METHOD0( LoadSystemCerts, int() );
    void MakeLoadSystemCertsReturn( int value );
    void ExpectLoadSystemCertsIsNotCalled();

    MOCK_METHOD0( UnloadSystemCerts, int() );
    void MakeUnloadSystemCertsReturn( int value );
    void ExpectUnloadSystemCertsIsNotCalled();
    
    MOCK_METHOD2( GetSystemCerts, int( X509***, size_t& ) );
    void MakeGetSystemCertsReturn( int value );
    void ExpectGetSystemCertsIsNotCalled();

    MOCK_METHOD2( FreeSystemCerts, int( X509**, size_t ) );
    void MakeFreeSystemCertsReturn( int value );
    void ExpectFreeSystemCertsIsNotCalled();
};