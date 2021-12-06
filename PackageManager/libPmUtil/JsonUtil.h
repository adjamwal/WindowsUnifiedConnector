#pragma once

#include "json\json.h"

class JsonUtil
{
public:
    JsonUtil() {}
    virtual ~JsonUtil() {}

    static bool ExtractJsonInt( Json::Value& root, const std::string& attribute, int& dest );
    static bool ExtractJsonString( Json::Value& root, const std::string& attribute, std::string& dest );
    static bool ExtractJsonStringOptional( Json::Value& root, const std::string& attribute, std::string& dest );
};
