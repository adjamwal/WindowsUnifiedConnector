#pragma once

#include "IFileUtil.h"

class FileUtil : public IFileUtil
{
public:
    FileUtil();
    ~FileUtil();

    std::string ReadFile( const std::string& filename ) override;
};