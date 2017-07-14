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

#include "Url.h"


TEST(URL, PARSE0) {
    Overflow::Url uri("http://google.com", 80);
    
    ASSERT_STREQ("http", uri.getProtocol().c_str());
}


TEST(URL, PARSE1) {
    Overflow::Url uri("http://google.com", 80);
    
    ASSERT_STREQ("google.com", uri.getHost().c_str());
}


TEST(URL, PARSE2) {
    Overflow::Url uri("http://google.com/", 80);
    
    ASSERT_STREQ("google.com", uri.getHost().c_str());
}


TEST(URL, PARSE3) {
    Overflow::Url uri("http://google.com:8080", 0);

    ASSERT_EQ(8080, uri.getPort());
    ASSERT_STREQ("google.com", uri.getHost().c_str());
}


TEST(URL, PARSE4) {
    Overflow::Url uri("http://google.com:8080/", 0);

    ASSERT_EQ(8080, uri.getPort());
    ASSERT_STREQ("google.com", uri.getHost().c_str());
}


TEST(URL, PARSE5) {
    Overflow::Url uri("http://google.com/", 80);
    
    ASSERT_EQ(80, uri.getPort());
}

TEST(URL, PARSE6) {
    Overflow::Url uri("http://admin:admin@google.com/", 80);
    
    ASSERT_EQ(80, uri.getPort());
    ASSERT_EQ(true, uri.hasAuth());
    ASSERT_STREQ("admin:admin", uri.getAuth().c_str());
    ASSERT_STREQ("/", uri.getPath().c_str());
}

TEST(URL, PARSE7) {
    Overflow::Url uri("http://admin:admin@google.com/mypath/1/2/3", 80);
    
    ASSERT_EQ(80, uri.getPort());
    ASSERT_EQ(true, uri.hasAuth());
    ASSERT_STREQ("admin:admin", uri.getAuth().c_str());
    ASSERT_STREQ("/mypath/1/2/3", uri.getPath().c_str());
    ASSERT_STREQ("google.com", uri.getHost().c_str());
}

TEST(URL, PARSE8) {
    Overflow::Url uri("rtsp://admin:admin@localhost:8554/test.264", 8554);

    ASSERT_EQ(8554, uri.getPort());
    ASSERT_EQ(true, uri.hasAuth());
    ASSERT_STREQ("admin:admin", uri.getAuth().c_str());
    ASSERT_STREQ("/test.264", uri.getPath().c_str());
    ASSERT_STREQ("localhost", uri.getHost().c_str());
}
