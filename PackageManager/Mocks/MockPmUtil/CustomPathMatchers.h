#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <filesystem>

using ::testing::_;

MATCHER_P( PathContains, value, "Path matcher for sub paths" )
{
    bool ret = false;

    for ( const auto& part : arg.relative_path() )
    {
        if ( part.string().find( value ) != std::string::npos ) {
            ret = true;
        }
    }

    return ret;
}