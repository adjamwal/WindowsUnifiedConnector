#pragma once

#include <string>
#include <vector>

class ISslUtil
{
public:
    virtual ~ISslUtil() {}

    virtual int32_t DecodeBase64( const std::string& base64Str, std::vector<uint8_t>& output ) = 0;

    virtual bool CalculateSHA256( const std::string filnname, std::string& sha256 ) = 0;
};