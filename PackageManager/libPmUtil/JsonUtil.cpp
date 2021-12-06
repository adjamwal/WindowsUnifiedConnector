#include "JsonUtil.h"
#include "PmLogger.h"

bool JsonUtil::ExtractJsonInt( Json::Value& root, const std::string& attribute, int& dest )
{
    bool rtn = true;

    if( root[ attribute ].isInt() ) {
        dest = root[ attribute ].asInt();
    }
    else {
        rtn = false;
        LOG_ERROR( "Invalid %s", attribute.c_str() );
    }

    return rtn;
}

bool JsonUtil::ExtractJsonString( Json::Value& root, const std::string& attribute, std::string& dest )
{
    bool rtn = true;

    if( root[ attribute ].isString() ) {
        dest = root[ attribute ].asString();
    }
    else {
        rtn = false;
        LOG_ERROR( "Invalid %s", attribute.c_str() );
    }

    return rtn;
}

bool JsonUtil::ExtractJsonStringOptional( Json::Value& root, const std::string& attribute, std::string& dest )
{
    bool rtn = true;

    if ( root[ attribute ].isString() ) {
        dest = root[ attribute ].asString();
    }
    else {
        dest = "";
    }

    return rtn;
}
