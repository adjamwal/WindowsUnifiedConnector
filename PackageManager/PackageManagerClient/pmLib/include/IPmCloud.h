#pragma once

#include <string>

class IPmCloud
{
public:
    IPmCloud() {}
    virtual ~IPmCloud() {}

    virtual void SetUri( const std::string& uri ) = 0;
    virtual void SetToken( const std::string& token ) = 0;
    virtual int32_t Checkin( const std::string& payload, std::string& response ) = 0;
    virtual int32_t SendEvent( const std::string& payload ) = 0;
    virtual int32_t DownloadFile( const std::string& uri, const std::string filename ) = 0;
};