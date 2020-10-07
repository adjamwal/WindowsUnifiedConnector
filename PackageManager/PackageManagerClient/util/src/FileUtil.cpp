#include "FileUtil.h"
#include "PMLogger.h"
#include <sstream>
#include <fstream>

FileUtil::FileUtil()
{

}

FileUtil::~FileUtil()
{

}

std::string FileUtil::ReadFile( const std::string& filename )
{
    std::stringstream stream;

    if( filename.length() ) {
        std::ifstream file( filename );
        if( file.is_open() ) {
            stream << file.rdbuf();
        }
        else {
            LOG_ERROR( "Failed to open file %s", filename.c_str() );
        }
    }
    else {
        LOG_ERROR( "filename is empty" );
    }

    return stream.str();
}