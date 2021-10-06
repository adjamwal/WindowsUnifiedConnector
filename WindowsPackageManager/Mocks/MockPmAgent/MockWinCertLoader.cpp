#include "pch.h"
#include "MockWinCertLoader.h"

MockWinCertLoader::MockWinCertLoader()
{

}

MockWinCertLoader::~MockWinCertLoader()
{

}

void MockWinCertLoader::MakeLoadSystemCertsReturn( int value )
{
    ON_CALL( *this, LoadSystemCerts() ).WillByDefault( Return( value ) );
}

void MockWinCertLoader::ExpectLoadSystemCertsIsNotCalled()
{
    EXPECT_CALL( *this, LoadSystemCerts() ).Times( 0 );
}

void MockWinCertLoader::MakeUnloadSystemCertsReturn( int value )
{
    ON_CALL( *this, UnloadSystemCerts() ).WillByDefault( Return( value ) );
}

void MockWinCertLoader::ExpectUnloadSystemCertsIsNotCalled()
{
    EXPECT_CALL( *this, UnloadSystemCerts() ).Times( 0 );
}

void MockWinCertLoader::MakeGetSystemCertsReturn( int value )
{
    ON_CALL( *this, GetSystemCerts( _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinCertLoader::ExpectGetSystemCertsIsNotCalled()
{
    EXPECT_CALL( *this, GetSystemCerts( _, _ ) ).Times( 0 );
}

void MockWinCertLoader::MakeFreeSystemCertsReturn( int value )
{
    ON_CALL( *this, FreeSystemCerts( _, _) ).WillByDefault( Return( value ) );
}

void MockWinCertLoader::ExpectFreeSystemCertsIsNotCalled()
{
    EXPECT_CALL( *this, FreeSystemCerts( _, _ ) ).Times( 0 );
}