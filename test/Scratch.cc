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

#include <RtspWanClient.h>

#include "Util.h"


class Delegate: public Overflow::IRtspDelegate
{
    void onPaletteType(Overflow::RtspSessionType type)
    {
        LOG(INFO) << "palette-type: "
                  << Overflow::SessionDescription::typeToString(type);
    }
    
    void onRtspClientStateChange(Overflow::RtspClientState oldState,
                                 Overflow::RtspClientState newState) override
    {
        LOG(INFO) << "client-state-change: "
                  << stateToString(oldState)
                  << " -> "
                  << stateToString(newState);
    }

    void onRtpPacketExtension(int id,
                              const unsigned char * buffer,
                              const size_t length) override
    {
        LOG(INFO) << "onRtpPacketExtension: " << id;
    }
    
    void onPayload(const unsigned char * buffer, const size_t length) override
    {
        OverflowTest::Helpers::printOutAllNaluTypes (buffer, length);
    }
};


TEST(DEV, SCRATCH)
{
    OverflowTest::Helpers::setupTestLogger ();
    
    Delegate delegate;
    Overflow::RtspWanClient client (&delegate,
                                    "rtsp://127.0.0.1:8554/test.264");
    
    client.start ();
    OverflowTest::Helpers::sleep (20);
    client.stop ();
    client.join ();
}
