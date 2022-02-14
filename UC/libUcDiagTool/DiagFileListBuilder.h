#pragma once

#include "IDiagFileListBuilder.h"

class DiagFileListBuilder : public IDiagFileListBuilder
{
public:
    DiagFileListBuilder( std::vector<std::filesystem::path>* additionalFiles = NULL );
    ~DiagFileListBuilder();

    void GetFileList( std::vector<std::filesystem::path>& fileList ) override;

private:
    std::vector<std::filesystem::path> m_AdditionalFiles;

    void FindFilesAt( const std::filesystem::path& filePath, std::vector<std::filesystem::path>& fileList );
    void CreateFileManifest( std::vector<std::filesystem::path>& fileList );
};
