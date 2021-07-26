#include "MockCodesignVerifier.h"

MockCodesignVerifier::MockCodesignVerifier()
{
}

MockCodesignVerifier::~MockCodesignVerifier()
{
}

void MockCodesignVerifier::MakeVerifyReturn( CodesignStatus value ) 
{
    ON_CALL( *this, Verify( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockCodesignVerifier::ExpectVerifyIsNotCalled()
{
    EXPECT_CALL( *this, Verify( _, _, _ ) ).Times( 0 );
}

void MockCodesignVerifier::MakeVerifyWithKilldateReturn( CodesignStatus value )
{
    ON_CALL( *this, VerifyWithKilldate( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockCodesignVerifier::ExpectVerifyWithKilldateIsNotCalled()
{
    EXPECT_CALL( *this, VerifyWithKilldate( _, _, _, _ ) ).Times( 0 );
}