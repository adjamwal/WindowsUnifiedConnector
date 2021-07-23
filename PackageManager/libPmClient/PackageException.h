#pragma once

#include  <exception>
#include  <string>

class PackageException final : public std::exception
{
public:
    PackageException( const char* msg, int errCode, int subErrCode = 0, const char* subErrType = "" ) :
        std::exception( msg ), 
        m_errCode( errCode ),
        m_subErrCode( subErrCode ),
        m_subErrType( subErrType ) {}
    PackageException( const std::string msg, int errCode, int subErrCode = 0, const std::string subErrType = "" ) : 
        PackageException( msg.c_str(), errCode, subErrCode, subErrType.c_str() ) {}

    int whatCode() const { return m_errCode; }
    int whatSubCode() const { return m_subErrCode; }
    const std::string& whatSubType() const { return m_subErrType; }

private:
    int m_errCode;
    int m_subErrCode;
    std::string m_subErrType;
};
