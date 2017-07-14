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
    buffer.append(test_buf, sizeof(test_buf));

    ASSERT_EQ(3, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('b', buffer.bytesPointer()[1]);
    ASSERT_EQ('c', buffer.bytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_STRING) {
    Overflow::ByteBuffer buffer;

    buffer.append("abc");

    ASSERT_EQ(3, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('b', buffer.bytesPointer()[1]);
    ASSERT_EQ('c', buffer.bytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_STRING_2) {
    Overflow::ByteBuffer buffer;

    buffer.append("abc");
    buffer.append("def");

    ASSERT_EQ(6, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('b', buffer.bytesPointer()[1]);
    ASSERT_EQ('c', buffer.bytesPointer()[2]);
    ASSERT_EQ('d', buffer.bytesPointer()[3]);
    ASSERT_EQ('e', buffer.bytesPointer()[4]);
    ASSERT_EQ('f', buffer.bytesPointer()[5]);
}

TEST(BYTE_BUFFER, APPEND_STRING_3) {
    Overflow::ByteBuffer buffer;

    buffer.append("abc");
    buffer.append("defgh");
    buffer.append("12");

    ASSERT_EQ(10, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('b', buffer.bytesPointer()[1]);
    ASSERT_EQ('c', buffer.bytesPointer()[2]);
    ASSERT_EQ('d', buffer.bytesPointer()[3]);
    ASSERT_EQ('e', buffer.bytesPointer()[4]);
    ASSERT_EQ('f', buffer.bytesPointer()[5]);
    ASSERT_EQ('g', buffer.bytesPointer()[6]);
    ASSERT_EQ('h', buffer.bytesPointer()[7]);
    ASSERT_EQ('1', buffer.bytesPointer()[8]);
    ASSERT_EQ('2', buffer.bytesPointer()[9]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_0) {
    Overflow::ByteBuffer buffer;

    buffer.insert("abc", 0);

    ASSERT_EQ(3, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('b', buffer.bytesPointer()[1]);
    ASSERT_EQ('c', buffer.bytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_1) {
    Overflow::ByteBuffer buffer;

    buffer.insert("abc", 0);
    buffer.insert("12", 1);

    ASSERT_EQ(3, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('1', buffer.bytesPointer()[1]);
    ASSERT_EQ('2', buffer.bytesPointer()[2]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_2) {
    Overflow::ByteBuffer buffer;

    buffer.insert("abc", 0);
    buffer.insert("12", 1);
    buffer.insert("12", 2);

    ASSERT_EQ(4, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('1', buffer.bytesPointer()[1]);
    ASSERT_EQ('1', buffer.bytesPointer()[2]);
    ASSERT_EQ('2', buffer.bytesPointer()[3]);
}

TEST(BYTE_BUFFER, APPEND_INSERT_3) {
    Overflow::ByteBuffer buffer;

    buffer.insert("abc", 0);
    buffer.insert("12", 1);
    buffer.insert("12", 2);
    buffer.insert("35", 4);

    ASSERT_EQ(6, buffer.length());
    ASSERT_EQ('a', buffer.bytesPointer()[0]);
    ASSERT_EQ('1', buffer.bytesPointer()[1]);
    ASSERT_EQ('1', buffer.bytesPointer()[2]);
    ASSERT_EQ('2', buffer.bytesPointer()[3]);
    ASSERT_EQ('3', buffer.bytesPointer()[4]);
    ASSERT_EQ('5', buffer.bytesPointer()[5]);
}
