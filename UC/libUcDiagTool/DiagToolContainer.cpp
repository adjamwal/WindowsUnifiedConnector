#include "pch.h"
#include "DiagToolContainer.h"

#include "ZlibWrapper.h"
#include "DiagFileListBuilder.h"
#include "DiagPackager.h"
#include "DiagTool.h"

DiagToolContainer::DiagToolContainer() :
    m_zlibWrapper( new ZlibWrapper() )
    , m_fileListBuilder( new DiagFileListBuilder() )
    , m_packager( new DiagPackage( *m_zlibWrapper ) )
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