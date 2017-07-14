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
#include <glog/logging.h>

#include "Util.h"

#include <RtspWanClient.h>


class Delegate: public Overflow::IRtspDelegate
{        
    void Timeout() override
    {
        LOG(INFO) << "TIMEOUT";
    }
        
    void Payload(const unsigned char * buffer, const size_t length) override
    {
        OverflowTest::Helpers::PrintOutAllNaluTypes(buffer, length);
    }
};


TEST(DEV, SCRATCH)
{
    OverflowTest::Helpers::SetupTestLogger();
    
    Delegate delegate;
    Overflow::RtspWanClient client(&delegate, "rtsp://127.0.0.1:8554/test.264");
    
    bool ok = client.start();
    if (ok) {
        OverflowTest::Helpers::Sleep(10);
    }

    client.stop();
}
