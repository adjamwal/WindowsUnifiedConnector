// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::Field;
using ::testing::Values;
using ::testing::Combine;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::InSequence;
using ::testing::TestWithParam;
using ::testing::Matcher;
using ::testing::Values;
using ::testing::Ref;
using ::testing::ReturnRef;
using ::testing::StrEq;
using ::testing::StrNe;
using ::testing::ContainerEq;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::DoAll;
using ::testing::Mock;
using ::testing::ContainsRegex;
using ::testing::An;
using ::testing::Ne;
using ::testing::SetArrayArgument;
using ::testing::HasSubstr;
using ::testing::NotNull;

#endif //PCH_H
