#include "RandomUtil.h"
#include <numeric>
#include <algorithm>
#include <chrono>

namespace
{
    const std::string availableChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::wstring availableWChars = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
}

std::vector<uint8_t> RandomUtil::Bytes( std::size_t count )
{
    std::vector<uint8_t> data( count * ( 5 + GetNanos() % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), std::mt19937 { std::random_device { }( ) } );
    data.resize( count );
    return data;
}

std::vector<uint16_t> RandomUtil::Words( std::size_t count )
{
    std::vector<uint16_t> data( count * ( 5 + GetNanos() % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), std::mt19937 { std::random_device { }( ) } );
    data.resize( count );
    return data;
}

std::vector<uint32_t> RandomUtil::DWords( std::size_t count )
{
    std::vector<uint32_t> data( count * ( 5 + GetNanos() % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), std::mt19937 { std::random_device { }( ) } );
    data.resize( count );
    return data;
}

std::vector<uint64_t> RandomUtil::QWords( std::size_t count )
{
    std::vector<uint64_t> data( count * ( 5 + GetNanos() % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), std::mt19937 { std::random_device { }( ) } );
    data.resize( count );
    return data;
}

int RandomUtil::GetInt( int min, int max )
{
    std::mt19937 randomNumberGenerator = std::mt19937( GetSeed() );
    std::uniform_int_distribution<int> uni( min, max );

    return uni( randomNumberGenerator );
}

std::string RandomUtil::GetString( int minLength, int maxLength )
{
    int numberOfCharacters = GetInt( minLength, maxLength );

    std::string randomString;
    for( int i = 0; i < numberOfCharacters; i++ ) {
        size_t characterPosition = GetInt( 0, ( int )availableChars.size() - 1 );
        randomString += availableChars[ characterPosition ];
    }

    return randomString;
}

std::wstring RandomUtil::GetWString( int minLength, int maxLength )
{
    int numberOfCharacters = GetInt( minLength, maxLength );

    std::wstring randomString;
    for( int i = 0; i < numberOfCharacters; i++ ) {
        size_t characterPosition = GetInt( 0, ( int )availableWChars.size() - 1 );
        randomString += availableWChars[ characterPosition ];
    }

    return randomString;
}

uint64_t RandomUtil::GetSeed()
{
    unsigned int seed = GetNanos();
    auto seedData = DWords( 5 + seed % 6 );
    for( auto element : seedData ) {
        seed += element;
    }

    return seed;
}

uint64_t RandomUtil::GetNanos()
{
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    std::chrono::system_clock::duration nanos = tp.time_since_epoch();

    return nanos.count();
}
