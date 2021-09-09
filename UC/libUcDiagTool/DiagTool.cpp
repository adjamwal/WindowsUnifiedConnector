#include "pch.h"
#include "DiagTool.h"
#include "IDiagFileListBuilder.h"
#include "IDiagPackager.h"
#include "IUcLogger.h"
#include "WindowsUtilities.h"
#include <exception>
#include <time.h>

#define DIAG_PACKAGE_PREFIX "CM_Diagnostic_"
DiagTool::DiagTool( IDiagFileListBuilder& fileListBuilder, IDiagPackager& packager ) :
    m_fileListBuilder( fileListBuilder)
    , m_packager( packager )
{

}

DiagTool::~DiagTool()
{

}

std::filesystem::path DiagTool::GetPackagePath()
{
    std::filesystem::path packagePath = WindowsUtilities::ResolveKnownFolderIdForCurrentUser( "FOLDERID_Desktop" );
    packagePath /= DIAG_PACKAGE_PREFIX;
    packagePath += std::to_string( time( NULL ) );
    packagePath += ".zip";

    packagePath.make_preferred();

    LOG_DEBUG( "Diagnostic Package Path: %s", packagePath.generic_u8string().c_str() );

    return packagePath;
}

void DiagTool::CreateDiagnosticPackage()
{
    try {
        std::vector<std::filesystem::path> fileList;
        std::filesystem::path packagePath = GetPackagePath();

        m_fileListBuilder.GetFileList( fileList );

        m_packager.CreatePackage( fileList, packagePath );
    }
    catch ( std::exception& e ) {
        LOG_ERROR( "Exception %s", e.what() );
    }
}