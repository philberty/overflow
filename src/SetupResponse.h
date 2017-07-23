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

#ifndef __SETUP_RESPONSE_H__
#define __SETUP_RESPONSE_H__

#include "RtspResponse.h"


namespace Overflow
{    
    class SetupResponse: public RtspResponse
    {
    public:
        SetupResponse(const Response *resp);
        
        int getRtpInterleavedChannel() const { return mRtpInterleavedChannel; }
        
        int getRtcpInterleavedChannel() const { return mRtcpInterleavedChannel; }
        
        const std::string getSession() const { return mSession; }
        
        int getTimeoutSeconds() const { return mTimeout; }
        
        bool isInterleaved() const { return mIsInterleaved; }
        
    private:
        
        bool mIsInterleaved;
        int mRtpInterleavedChannel;
        int mRtcpInterleavedChannel;
        std::string mSession;
        int mTimeout;
    };
};
#endif //__SETUP_RESPONSE_H__
