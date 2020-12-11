#include "RandomUtil.h"
#include <random>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <atomic>

namespace
{
    const std::string availableChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::wstring availableWChars = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static std::mt19937 generator( std::random_device {}( ) );
    static std::uniform_int_distribution<int> distribution { std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max() };
    static std::atomic_int sequence = distribution( generator );
}

std::vector<uint8_t> RandomUtil::Bytes( std::size_t count )
{
    std::vector<uint8_t> data( count * ( 2 + ( sequence + GetNanos() ) % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), generator );
    data.resize( count );
    return data;
}

std::vector<uint16_t> RandomUtil::Words( std::size_t count )
{
    std::vector<uint16_t> data( count * ( 2 + ( sequence + GetNanos() ) % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), generator );
    data.resize( count );
    return data;
}

std::vector<uint32_t> RandomUtil::DWords( std::size_t count )
{
    std::vector<uint32_t> data( count * ( 2 + ( sequence + GetNanos() ) % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), generator );
    data.resize( count );
    return data;
}

std::vector<uint64_t> RandomUtil::QWords( std::size_t count )
{
    std::vector<uint64_t> data( count * ( 2 + ( sequence + GetNanos() ) % 6 ) );
    std::iota( data.begin(), data.end(), 0 );
    std::shuffle( data.begin(), data.end(), generator );
    data.resize( count );
    return data;
}

int RandomUtil::GetInt( int min, int max )
{
    std::uniform_int_distribution<int> range( min, max );
    return range( generator );
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
    uint64_t seed = sequence + GetNanos();
    auto seedData = QWords( 10 + seed % 6 );
    for( auto element : seedData ) {
        seed += element;
    }

    return seed;
}

uint64_t RandomUtil::GetNanos()
{
    auto origin = std::chrono::system_clock::from_time_t( time_t( -12219292800 ) );
    auto tspan = std::chrono::system_clock::now() - origin;
    auto nanos = std::chrono::duration_cast< std::chrono::nanoseconds >( tspan ).count();

    return nanos;
}
