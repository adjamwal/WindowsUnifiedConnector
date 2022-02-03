#pragma once

#include "IDiagTool.h"
#include <filesystem>

class IDiagFileListBuilder;
class IDiagPackager;

class DiagTool : public IDiagTool
{
public:
    DiagTool( IDiagFileListBuilder &fileListBuilder, IDiagPackager &packager );
    ~DiagTool();

    void CreateDiagnosticPackage( const std::wstring& outputFile = L"" ) override;

private:
    IDiagFileListBuilder& m_fileListBuilder;
    IDiagPackager& m_packager;

    std::filesystem::path GetPackagePath();
};