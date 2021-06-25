#pragma once
#include "IDiagPackager.h"

class IZlibWrapper;

class DiagPackager : public IDiagPackager
{
public:
    DiagPackager( IZlibWrapper& zlibWrapper );
    ~DiagPackager();

    void CreatePackage( const std::vector<std::filesystem::path>& fileList, const std::filesystem::path& packagePath ) override;

private:
    IZlibWrapper& m_zlibWrapper;
};