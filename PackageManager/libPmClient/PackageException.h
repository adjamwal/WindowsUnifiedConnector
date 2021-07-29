#pragma once

#include  <exception>
#include  <string>
#include  "IPmHttp.h"

class PackageException final : public std::exception
{
public:
    PackageException( const char* msg, int errCode, PmHttpExtendedResult eResult = {} ) :
        std::exception( msg ), 
        m_errCode( errCode ),
        m_eResult( eResult ) {}
    PackageException( const std::string msg, int errCode, PmHttpExtendedResult eResult = {} ) :
        PackageException( msg.c_str(), errCode, eResult ) {}

    int whatCode() const { return m_errCode; }
    int whatSubCode() const { return m_eResult.subErrorCode; }
    const std::string& whatSubType() const { return m_eResult.subErrorType; }

private:
    int m_errCode;
    PmHttpExtendedResult m_eResult;
};
