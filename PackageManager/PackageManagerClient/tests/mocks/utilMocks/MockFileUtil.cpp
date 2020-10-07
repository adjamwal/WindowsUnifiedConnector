#include "MockFileUtil.h"

MockFileUtil::MockFileUtil() :
    m_defaultString( "" )
{
    MakeReadFileReturn( m_defaultString );
}

MockFileUtil::~MockFileUtil()
{

}

void MockFileUtil::MakeReadFileReturn( std::string value )
{
    ON_CALL( *this, ReadFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectReadFileNotCalled()
{
    EXPECT_CALL( *this, ReadFile( _ ) ).Times( 0 );
}