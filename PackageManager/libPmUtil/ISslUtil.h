#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

class ISslUtil
{
public:
    virtual ~ISslUtil() {}

    virtual int32_t DecodeBase64( const std::string& base64Str, std::vector<uint8_t>& output ) = 0;

    virtual std::optional<std::string> CalculateSHA256( const std::filesystem::path& filnname ) = 0;
};