#pragma once
#include "IDiagPackager.h"

class IZlibWrapper;

class DiagPackage : public IDiagPackager
{
public:
    DiagPackage( IZlibWrapper& zlibWrapper );
    ~DiagPackage();

    void CreatePackage( const std::vector<std::filesystem::path>& fileList, const std::filesystem::path& packagePath ) override;

private:
    IZlibWrapper& m_zlibWrapper;
};