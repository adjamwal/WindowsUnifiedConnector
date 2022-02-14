#pragma once

#include <memory>
#include <filesystem>
#include <vector>

class IZlibWrapper;
class IDiagFileListBuilder;
class IDiagPackager;
class IDiagTool;

class DiagToolContainer
{
public:
    DiagToolContainer( std::vector<std::filesystem::path>* additionalFiles = NULL );
    ~DiagToolContainer();

    IDiagTool& GetDiagTool();

private:
    std::unique_ptr<IZlibWrapper> m_zlibWrapper;
    std::unique_ptr<IDiagFileListBuilder> m_fileListBuilder;
    std::unique_ptr<IDiagPackager> m_packager;
    std::unique_ptr<IDiagTool> m_diagTool;
};
