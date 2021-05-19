#pragma once

#include "IDiagFileListBuilder.h"

class DiagFileListBuilder : public IDiagFileListBuilder
{
public:
    DiagFileListBuilder();
    ~DiagFileListBuilder();

    void GetFileList( std::vector<std::filesystem::path>& fileList ) override;

private:
    void FindFilesAt( const std::filesystem::path& filePath, std::vector<std::filesystem::path>& fileList );
    void CreateFileManifest( std::vector<std::filesystem::path>& fileList );
};
