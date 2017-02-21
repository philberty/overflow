// -*-c++-*-
// Copyright (c) 2017 Philip Herron.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <gtest/gtest.h>

#include "Helpers.h"

#define CHECK_OFFSETS(_LNS, LN, _O, _L)           \
    do {                                          \
        std::pair<int,int>& _l = _LNS->at(LN);    \
        ASSERT_EQ(_l.first, _O);                  \
        ASSERT_EQ(_l.second, _L);                 \
    } while(0);                                   \


TEST(HELPERS, PARSE0) {
    const std::string input_buffer = "xyz..123..45..";

    const unsigned char *ibuf = (const unsigned char*)input_buffer.c_str();
    const size_t length = input_buffer.size();
    const std::string delim = "..";

    std::vector<std::pair<int,int>> *lines = Overflow::Helper::SplitBufferByDelimiter(ibuf, length, delim);

    ASSERT_NE(lines, nullptr);
    ASSERT_EQ(lines->size(), 3);

    CHECK_OFFSETS(lines, 0, 0, 5);
    CHECK_OFFSETS(lines, 1, 5, 5);
    CHECK_OFFSETS(lines, 2, 10, 4);

    delete lines;
}

TEST(HELPERS, PARSE1) {
    const std::string input_buffer = "x..12..345..";

    const unsigned char *ibuf = (const unsigned char*)input_buffer.c_str();
    const size_t length = input_buffer.size();
    const std::string delim = "..";

    std::vector<std::pair<int,int>> *lines = Overflow::Helper::SplitBufferByDelimiter(ibuf, length, delim);

    ASSERT_NE(lines, nullptr);
    ASSERT_EQ(lines->size(), 3);

    CHECK_OFFSETS(lines, 0, 0, 3);
    CHECK_OFFSETS(lines, 1, 3, 4);
    CHECK_OFFSETS(lines, 2, 7, 5);

    delete lines;
}

TEST(HELPERS, PARSE2) {
    const std::string input_buffer = "x.y.z";

    const unsigned char *ibuf = (const unsigned char*)input_buffer.c_str();
    const size_t length = input_buffer.size();
    const std::string delim = ".";

    std::vector<std::pair<int,int>> *lines = Overflow::Helper::SplitBufferByDelimiter(ibuf, length, delim);

    ASSERT_NE(lines, nullptr);
    ASSERT_EQ(lines->size(), 3);

    CHECK_OFFSETS(lines, 0, 0, 2);
    CHECK_OFFSETS(lines, 1, 2, 2);
    CHECK_OFFSETS(lines, 2, 4, 1);

    delete lines;
}

TEST(HELPERS, PARSE3) {
    const std::string input_buffer = "RTSP/1.0 200\r\nheader: value\r\ncontent";

    const unsigned char *ibuf = (const unsigned char*)input_buffer.c_str();
    const size_t length = input_buffer.size();
    const std::string delim = "\r\n";

    std::vector<std::pair<int,int>> *lines = Overflow::Helper::SplitBufferByDelimiter(ibuf, length, delim);

    ASSERT_NE(lines, nullptr);
    ASSERT_EQ(lines->size(), 3);

    CHECK_OFFSETS(lines, 0, 0, 14);
    CHECK_OFFSETS(lines, 1, 14, 15);
    CHECK_OFFSETS(lines, 2, 29, 7);

    delete lines;
}

TEST(HELPERS, PARSE4) {
    const std::string input_buffer = "RTSP/1.0 200\r\nheader: value\r\ncontent\r\n\r\n";

    const unsigned char *ibuf = (const unsigned char*)input_buffer.c_str();
    const size_t length = input_buffer.size();
    const std::string delim = "\r\n";

    std::vector<std::pair<int,int>> *lines = Overflow::Helper::SplitBufferByDelimiter(ibuf, length, delim);

    ASSERT_NE(lines, nullptr);
    ASSERT_EQ(lines->size(), 4);

    CHECK_OFFSETS(lines, 0, 0, 14);
    CHECK_OFFSETS(lines, 1, 14, 15);
    CHECK_OFFSETS(lines, 2, 29, 9);
    CHECK_OFFSETS(lines, 3, 38, 2);

    delete lines;
}
