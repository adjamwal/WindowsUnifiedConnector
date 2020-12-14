#pragma once

#include  <exception>
#include  <string>

class PackageException final : public std::exception
{
public:
    PackageException( const char* msg, int errCode ) : std::exception( msg ), m_errCode( errCode ) {}
    PackageException( const std::string msg, int errCode ) : PackageException( msg.c_str(), errCode ) {}

    int whatCode() const { return m_errCode; }

private:
    int m_errCode;
};
