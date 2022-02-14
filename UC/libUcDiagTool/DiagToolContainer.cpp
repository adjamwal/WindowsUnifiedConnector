#include "pch.h"
#include "DiagToolContainer.h"

#include "ZlibWrapper.h"
#include "DiagFileListBuilder.h"
#include "DiagPackager.h"
#include "DiagTool.h"

DiagToolContainer::DiagToolContainer( std::vector<std::filesystem::path>* additionalFiles ) :
    m_zlibWrapper( new ZlibWrapper() )
    , m_fileListBuilder( new DiagFileListBuilder( additionalFiles ) )
    , m_packager( new DiagPackager( *m_zlibWrapper ) )
    , m_diagTool( new DiagTool( *m_fileListBuilder, *m_packager ) )
{

}

DiagToolContainer::~DiagToolContainer()
{

}

IDiagTool& DiagToolContainer::GetDiagTool()
{
    return *m_diagTool;
}