#include "SslUtil.h"
#include "PmLogger.h"
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

SslUtil::SslUtil()
{

}

SslUtil::~SslUtil()
{

}

//see https://en.wikipedia.org/wiki/Base64
//The ratio of output bytes to input bytes is 4:3 ( 33 % overhead ).
//Specifically, given an input of n bytes, the output will be 4 * ciel( n/3 ) bytes long, including padding characters.
#define BASE64_RATIO_NUMBER_DIVIDEND 4
#define BASE64_RATIO_NUMBER_DIVISOR 3
size_t SslUtil::CalculateDecodeLength( const char* b64input )
{
    if( !b64input ) {
        LOG_ERROR( "b64input is NULL" );
        return 0;
    }

    size_t length = strlen( b64input );
    size_t padding = 0;

    if( ( length % BASE64_RATIO_NUMBER_DIVIDEND ) != 0 ) {
        LOG_ERROR( "base64 encoded strings are always divisible by 4" );
        length = 0;
        return length;
    }

    if( b64input[ length - 1 ] == '=' && b64input[ length - 2 ] == '=' ) {
        padding = 2;
    }
    else if( b64input[ length - 1 ] == '=' ) {
        padding = 1;
    }

    length = length * BASE64_RATIO_NUMBER_DIVISOR;

    return ( ( length ) / BASE64_RATIO_NUMBER_DIVIDEND - padding );
}

int32_t SslUtil::DecodeBase64( const std::string& base64Str, std::vector<uint8_t>& output )
{
    int32_t functionStatus = -1;

    if( base64Str.size() == 0 ) {

        LOG_ERROR( "arguments are invalid" );
        return functionStatus;
    }

    BIO* pBioMemory = NULL;
    BIO* pBio64 = NULL;
    
    output.clear();
    unsigned char* pTempOutputBuffer = NULL;
    size_t tempOutputBufferSize = 0u;

    size_t decodeLength = CalculateDecodeLength( base64Str.c_str() );
    if( decodeLength <= 0 ) {
        LOG_ERROR( "CalculateDecodeLength failed to compute decodeLength" );
        goto abort;
    }

    pTempOutputBuffer = ( unsigned char* )malloc( decodeLength + 1 );
    if( !pTempOutputBuffer ) {
        LOG_ERROR( "failed to allocate memory for pTempOutputBuffer" );
        goto abort;
    }
    pTempOutputBuffer[ decodeLength ] = '\0';

    pBioMemory = BIO_new_mem_buf( base64Str.c_str(), -1 );
    if( !pBioMemory ) {
        LOG_ERROR( "failed to allocate memory for pBioMemory" );
        goto abort;
    }

    pBio64 = BIO_new( BIO_f_base64() );
    if( !pBio64 ) {
        LOG_ERROR( "failed to allocate memory for pBio64" );
        goto abort;
    }

    pBioMemory = BIO_push( pBio64, pBioMemory );
    
    BIO_set_flags( pBioMemory, BIO_FLAGS_BASE64_NO_NL );
    tempOutputBufferSize = BIO_read( pBioMemory, pTempOutputBuffer, base64Str.length() );

    if( tempOutputBufferSize != decodeLength ) {
        LOG_ERROR( "bufferSize should equal decode length." );
    }
    else {
        LOG_DEBUG( "Succesfully decoded pInputBuffer into pOutputBuffer" );
        output.resize( tempOutputBufferSize );
        memcpy( output.data(), pTempOutputBuffer, tempOutputBufferSize );
        
        pTempOutputBuffer = NULL;
        functionStatus = 0;
    }

abort:
    if( pBioMemory ) {
        BIO_free_all( pBioMemory );
    }
    if( pBio64 ) {
        BIO_free_all( pBio64 );
    }
    if( pTempOutputBuffer ) {
        free( pTempOutputBuffer );
    }

    return functionStatus;
}

static const int K_READ_BUF_SIZE{ 1024 * 16 };
std::optional<std::string> SslUtil::CalculateSHA256( const std::string filename )
{
    // Initialize openssl
    SHA256_CTX context;
    if ( !SHA256_Init( &context ) )
    {
        return std::nullopt;
    }

    // Read file and update calculated SHA
    char buf[K_READ_BUF_SIZE];
    std::ifstream file( filename, std::ifstream::binary );
    while ( file.good() )
    {
        file.read( buf, sizeof( buf ) );
        if ( !SHA256_Update( &context, buf, file.gcount() ) )
        {
            return std::nullopt;
        }
    }

    // Get Final SHA
    unsigned char result[SHA256_DIGEST_LENGTH];
    if ( !SHA256_Final( result, &context ) )
    {
        return std::nullopt;
    }

    std::stringstream ss;
    for ( int i = 0; i < SHA256_DIGEST_LENGTH; i++ )
    {
        ss << std::hex << std::setw( 2 ) << std::setfill( '0' ) << (int)result[i];
    }

    return ss.str();
}
