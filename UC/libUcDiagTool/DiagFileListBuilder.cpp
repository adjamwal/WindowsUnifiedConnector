#include "pch.h"
#include "framework.h"

#include "DiagFileListBuilder.h"
#include "WindowsUtilities.h"
#include "IUCLogger.h"

#include <sstream>
DiagFileListBuilder::DiagFileListBuilder()
{

}

DiagFileListBuilder::~DiagFileListBuilder()
{

}

void DiagFileListBuilder::GetFileList( std::vector<std::filesystem::path>& fileList )
{
    std::wstring configPath;

    fileList.clear();

    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureClient\\UnifiedConnector\\config", L"path", configPath ) ) {
        WLOG_ERROR( L"Failed to read config path from registry" );
    }
    else {
        FindFilesAt( configPath, fileList );
    }

    FindFilesAt( WindowsUtilities::GetLogDir(), fileList );
    CreateFileManifest( fileList );
}

void DiagFileListBuilder::FindFilesAt( const std::filesystem::path& filePath, std::vector<std::filesystem::path>& fileList )
{
    if ( !filePath.empty() ) {
        std::filesystem::path searchPath = filePath;
        searchPath /= "*";
        searchPath.make_preferred();
        LOG_DEBUG( "Finding files with at %s", searchPath.generic_u8string().c_str() );
        WindowsUtilities::FileSearchWithWildCard( searchPath, fileList );
    }
    else {
        WLOG_ERROR( L"filePath is empty" );
    }

}

void DiagFileListBuilder::CreateFileManifest( std::vector<std::filesystem::path>& fileList )
{
    std::stringstream manifestStream;
    std::filesystem::path manifestFile = WindowsUtilities::GetLogDir();

    if ( !manifestFile.empty() ) {
        bool manifestAlreadyInList = false;

        manifestFile /= "manifest.txt";
        manifestFile.make_preferred();

        for ( auto& file : fileList ) {
            LOG_DEBUG( "Add %s to manifest", file.generic_u8string().c_str() );
            manifestStream << file.generic_u8string().c_str() << std::endl;
            if ( file == manifestFile ) {
                manifestAlreadyInList = true;
            }
        }

        if ( WindowsUtilities::WriteFileContents(
            manifestFile.native().c_str(),
            ( const uint8_t* )manifestStream.str().c_str(),
            manifestStream.str().length() ) ) {
            if ( !manifestAlreadyInList ) {
                fileList.push_back( manifestFile );
            }
        }
        else {
            WLOG_ERROR( L"Failed to create diag manifest" );
        }
    }
    else {
        WLOG_ERROR( L"File list is empty" );
    }
}