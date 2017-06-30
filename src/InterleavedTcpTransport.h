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

#ifndef __INTERLEAVED_TCP_TRANSPORT_H__
#define __INTERLEAVED_TCP_TRANSPORT_H__

#include "ITransportDelegate.h"
#include "Transport.h"

#include <functional>

#include <uvpp/loop.hpp>
#include <uvpp/tcp.hpp>


namespace Overflow
{
    class InterleavedTcpTransport: public Transport
    {
    public:
        InterleavedTcpTransport(ITransportDelegate * const delegate,
                                uvpp::loop& loop,
                                const std::string& url);

        void setRtpInterleavedChannel(int channel);

        void setRtcpInterleavedChannel(int channel);

        void write(const unsigned char *buffer, const size_t length) override;

        std::string getTransportHeaderString() const override;

        bool connect() override;

        void shutdown() override;

    private:
        size_t readResponse(const unsigned char* buffer, size_t length);

        uvpp::Tcp mTcp;
        std::string mHost;
        int mPort;
        int mRtpInterleavedChannel;
        int mRtcpInterleavedChannel;
        std::vector<unsigned char> mReceivedBuffer;
    };
};

#endif //__INTERLEAVED_TCP_TRANSPORT_H__
