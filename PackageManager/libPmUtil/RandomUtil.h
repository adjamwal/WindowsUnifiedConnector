#pragma once

#include <random>
#include <vector>

class RandomUtil
{
public:
    static std::vector<uint8_t> Bytes( std::size_t count );
    static std::vector<uint16_t> Words( std::size_t count );
    static std::vector<uint32_t> DWords( std::size_t count );
    static std::vector<uint64_t> QWords( std::size_t count );

    static int GetInt( int min, int max );
    static std::string GetString( int minLength = 5, int maxLength = 10 );
    static std::wstring GetWString( int minLength = 5, int maxLength = 10 );
    static uint64_t GetSeed();
    static uint64_t GetNanos();
};
