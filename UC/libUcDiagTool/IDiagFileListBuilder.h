#pragma once

#include <vector>
#include <filesystem>

class IDiagFileListBuilder
{
public:
    virtual ~IDiagFileListBuilder() {}

    virtual void GetFileList( std::vector<std::filesystem::path>& fileList ) = 0;
};
