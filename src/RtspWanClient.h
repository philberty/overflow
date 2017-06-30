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

#ifndef __RTSP_WAN_CLIENT_H__
#define __RTSP_WAN_CLIENT_H__

#include "IRtspDelegate.h"
#include "ITransportDelegate.h"
#include "Transport.h"
#include "InterleavedTcpTransport.h"

#include <uvpp/loop.hpp>

#include <string>
#include <thread>


namespace Overflow
{
    class RtspWanClient: protected ITransportDelegate
    {
    public:
        RtspWanClient(IRtspDelegate * const delegate, const std::string& url);

        ~RtspWanClient();

        bool start();

        void stop();

    protected:
        void onRtpPacket(const RtpPacket* packet) override;

        // void onRtcpPacket(const RtcpPackate* packet) = 0;

        void onStateChange(TransportState state) override;
        
        void onTransportError(TransportErrorReason reason) override;

    private:
        IRtspDelegate * const mDelegate;
        std::string mUrl;
        uvpp::loop mLoop;
        InterleavedTcpTransport mTcpTransport;
        Transport* mTransport;
    };
    
};

#endif //__RTSP_WAN_CLIENT_H__
