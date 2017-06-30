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


#include "InterleavedTcpTransport.h"
#include "Url.h"

#include <glog/logging.h>
#include <uvpp/resolver.hpp>


Overflow::InterleavedTcpTransport::InterleavedTcpTransport(ITransportDelegate * const delegate,
                                                           uvpp::loop& loop,
                                                           const std::string& url)
    : Transport(delegate), 
      mTcp(loop),
      mRtpInterleavedChannel(0),
      mRtcpInterleavedChannel(1)
{
    Url uri(url, 554);
    mHost = uri.getHost();
    mPort = uri.getPort();
}

std::string
Overflow::InterleavedTcpTransport::getTransportHeaderString() const
{
    char rtp_channel[12];
    char rtcp_channel[12];
    memset(rtp_channel, 0, sizeof(rtp_channel));
    memset(rtcp_channel, 0, sizeof(rtcp_channel));
    snprintf(rtp_channel, sizeof(rtp_channel), "%d", mRtpInterleavedChannel);
    snprintf(rtcp_channel, sizeof(rtcp_channel), "%d", mRtcpInterleavedChannel);
    
    return "RTP/AVP/TCP;unicast;interleaved="
        + std::string(rtp_channel)
        + "-"
        + std::string(rtcp_channel);
}

void
Overflow::InterleavedTcpTransport::setRtpInterleavedChannel(int channel)
{
    mRtpInterleavedChannel = channel;
}

void
Overflow::InterleavedTcpTransport::setRtcpInterleavedChannel(int channel)
{
    mRtcpInterleavedChannel = channel;
}

void
Overflow::InterleavedTcpTransport::write(const unsigned char *buffer,
                                         const size_t length)
{    
    mTcp.write((const char *)buffer, (int)length,
               [&](uvpp::error e) {
                   onError(UNKNOWN);
               });
}

void
Overflow::InterleavedTcpTransport::shutdown()
{
    mTcp.read_stop();
    
    // mTcp.shutdown([&](uvpp::error e) {
    //         // TODO
    //     });
}

bool
Overflow::InterleavedTcpTransport::connect()
{
    auto connection_handler = [&](const uvpp::error& error) {
        if (error)
        {
            LOG(INFO) << "failed to connect: tcp://" << mHost << ":" << mPort;
            onError(UNKNOWN);
            return;
        }
        
        LOG(INFO) << "Connected: tcp://" << mHost << ":" << mPort;
        // mTcp.read_start(readCallback);
    };

    return mTcp.connect(mHost, mPort, connection_handler);
}

size_t
Overflow::InterleavedTcpTransport::readResponse(const unsigned char* buffer,
                                                size_t length)
{
    return 0;
}

