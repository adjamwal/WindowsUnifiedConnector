#pragma once
#include <string>

class IDiagTool
{
public:
    virtual ~IDiagTool() {}

    virtual void CreateDiagnosticPackage( const std::wstring &outputFile = L"" ) = 0;
    virtual void CreateDiagnosticFiles( const std::wstring& outputDir ) = 0;
};
