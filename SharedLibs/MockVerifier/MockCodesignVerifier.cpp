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

void MockCodesignVerifier::MakeVerifyWithKilldateReturn( CodesignStatus value )
{
    ON_CALL( *this, VerifyWithKilldate( _, _, _, _ ) ).WillByDefault( Return( value ) );
}