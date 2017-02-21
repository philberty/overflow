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

#include "ByteBuffer.h"


TEST(BYTE_BUFFER, APPEND_BUFFER) {
    Overflow::ByteBuffer buffer;

    unsigned char test_buf[] = { 'a', 'b', 'c' };
    buffer.Append(test_buf, sizeof(test_buf));

    ASSERT_EQ(3, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('b', buffer.BytesPointer()[1]);
    ASSERT_EQ('c', buffer.BytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_STRING) {
    Overflow::ByteBuffer buffer;

    buffer.Append("abc");

    ASSERT_EQ(3, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('b', buffer.BytesPointer()[1]);
    ASSERT_EQ('c', buffer.BytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_STRING_2) {
    Overflow::ByteBuffer buffer;

    buffer.Append("abc");
    buffer.Append("def");

    ASSERT_EQ(6, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('b', buffer.BytesPointer()[1]);
    ASSERT_EQ('c', buffer.BytesPointer()[2]);
    ASSERT_EQ('d', buffer.BytesPointer()[3]);
    ASSERT_EQ('e', buffer.BytesPointer()[4]);
    ASSERT_EQ('f', buffer.BytesPointer()[5]);
}

TEST(BYTE_BUFFER, APPEND_STRING_3) {
    Overflow::ByteBuffer buffer;

    buffer.Append("abc");
    buffer.Append("defgh");
    buffer.Append("12");

    ASSERT_EQ(10, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('b', buffer.BytesPointer()[1]);
    ASSERT_EQ('c', buffer.BytesPointer()[2]);
    ASSERT_EQ('d', buffer.BytesPointer()[3]);
    ASSERT_EQ('e', buffer.BytesPointer()[4]);
    ASSERT_EQ('f', buffer.BytesPointer()[5]);
    ASSERT_EQ('g', buffer.BytesPointer()[6]);
    ASSERT_EQ('h', buffer.BytesPointer()[7]);
    ASSERT_EQ('1', buffer.BytesPointer()[8]);
    ASSERT_EQ('2', buffer.BytesPointer()[9]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_0) {
    Overflow::ByteBuffer buffer;

    buffer.Insert("abc", 0);

    ASSERT_EQ(3, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('b', buffer.BytesPointer()[1]);
    ASSERT_EQ('c', buffer.BytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_1) {
    Overflow::ByteBuffer buffer;

    buffer.Insert("abc", 0);
    buffer.Insert("12", 1);

    ASSERT_EQ(3, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('1', buffer.BytesPointer()[1]);
    ASSERT_EQ('2', buffer.BytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_2) {
    Overflow::ByteBuffer buffer;

    buffer.Insert("abc", 0);
    buffer.Insert("12", 1);
    buffer.Insert("12", 2);

    ASSERT_EQ(4, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('1', buffer.BytesPointer()[1]);
    ASSERT_EQ('1', buffer.BytesPointer()[2]);
    ASSERT_EQ('2', buffer.BytesPointer()[3]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_3) {
    Overflow::ByteBuffer buffer;

    buffer.Insert("abc", 0);
    buffer.Insert("12", 1);
    buffer.Insert("12", 2);
    buffer.Insert("35", 4);

    ASSERT_EQ(6, buffer.Length());
    ASSERT_EQ('a', buffer.BytesPointer()[0]);
    ASSERT_EQ('1', buffer.BytesPointer()[1]);
    ASSERT_EQ('1', buffer.BytesPointer()[2]);
    ASSERT_EQ('2', buffer.BytesPointer()[3]);
    ASSERT_EQ('3', buffer.BytesPointer()[4]);
    ASSERT_EQ('5', buffer.BytesPointer()[5]);
}
