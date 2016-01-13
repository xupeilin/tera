// Copyright (c) 2015, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utils/string_util.h"

#include <gtest/gtest.h>

namespace tera {

TEST(StringUtilTest, IsValidName) {
    ASSERT_FALSE(IsValidName(""));
    ASSERT_FALSE(IsValidName(std::string("\0", 1)));
    ASSERT_FALSE(IsValidName("\1"));

    ASSERT_FALSE(IsValidName(std::string(kNameLenMin - 1, 'a')));
    ASSERT_TRUE(IsValidName(std::string(kNameLenMin, 'a')));
    ASSERT_TRUE(IsValidName(std::string(kNameLenMin + 1, 'a')));

    ASSERT_TRUE(IsValidName(std::string(kNameLenMax - 1, 'a')));
    ASSERT_TRUE(IsValidName(std::string(kNameLenMax, 'a')));
    ASSERT_FALSE(IsValidName(std::string(kNameLenMax + 1, 'a')));

    ASSERT_FALSE(IsValidName("1abc"));
    ASSERT_FALSE(IsValidName("_1abc"));

    ASSERT_TRUE(IsValidName("a"));
    ASSERT_TRUE(IsValidName("A"));
    ASSERT_TRUE(IsValidName("abcDEFGz123_233000_"));

    ASSERT_FALSE(IsValidName("abcDEFGz123_233\1bac"));
    ASSERT_FALSE(IsValidName("a~`!@#$%^&*()_=+"));
    ASSERT_FALSE(IsValidName("a[{;:'\",<>/?\"'}]"));
}

TEST(StringUtilTest, IsValidCfName) {
    ASSERT_TRUE(IsValidColumnFamilyName(""));
    ASSERT_TRUE(IsValidColumnFamilyName(std::string(64 * 1024 - 1, 'a')));
    ASSERT_FALSE(IsValidColumnFamilyName(std::string(64 * 1024, 'a')));

    ASSERT_TRUE(IsValidColumnFamilyName("1"));
    ASSERT_TRUE(IsValidColumnFamilyName("cf0"));
    ASSERT_TRUE(IsValidColumnFamilyName("_1234567890-abcdefghijklmnopqrstuvwxyz:."));

    ASSERT_FALSE(IsValidColumnFamilyName("cf0\1"));
    ASSERT_FALSE(IsValidColumnFamilyName("cf0\2"));
}

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
