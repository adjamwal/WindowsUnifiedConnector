#pragma once

#include  <exception>
#include  <string>
#include  "PmTypes.h"

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
    const PmHttpExtendedResult& whatSubError() const { return m_eResult; }

private:
    int m_errCode;
    PmHttpExtendedResult m_eResult;
};
