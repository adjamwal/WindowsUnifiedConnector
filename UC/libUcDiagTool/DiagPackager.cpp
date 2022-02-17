#include "pch.h"
#include "DiagPackager.h"
#include "IZlibWrapper.h"
#include "IUcLogger.h"
#include <exception>

DiagPackager::DiagPackager( IZlibWrapper& zlibWrapper ) :
    m_zlibWrapper( zlibWrapper )
{

}

DiagPackager::~DiagPackager()
{

}

void DiagPackager::CreatePackage( const std::vector<std::filesystem::path>& fileList, const std::filesystem::path& packagePath )
{
    if ( fileList.empty() || packagePath.empty() ) {
        throw std::runtime_error( __FUNCTION__ ": Invalid Parameters" );
    }

    LOG_DEBUG( "Creating Archive %s", packagePath.generic_u8string().c_str() );

    if ( !m_zlibWrapper.CreateArchiveFile( packagePath.generic_u8string().c_str() ) ) {
        throw std::runtime_error( __FUNCTION__ ": Failed to create archive" );
    }

    for ( auto& file : fileList ) {
        m_zlibWrapper.AddFileToArchive( file.generic_u8string().c_str() );
    }

    m_zlibWrapper.CloseArchiveFile();
}

void DiagPackager::ExtractPackage( const std::filesystem::path& packagePath, const std::filesystem::path& packageDir )
{
    if( packagePath.empty() || packageDir.empty() ) {
        throw std::runtime_error( __FUNCTION__ ": Invalid Parameters" );
    }

    if( !m_zlibWrapper.ExtractArchive( packagePath.generic_u8string().c_str(), packageDir.generic_u8string().c_str() ) ) {
        throw std::runtime_error( __FUNCTION__ ": ExtractArchive failed" );
    }
}