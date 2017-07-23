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
                                                           const std::string& url)
    : Transport(delegate),
      mLoop(),
      mTcp(mLoop),
      mConnectionTimer(mLoop),
      mRtpInterleavedChannel(0),
      mRtcpInterleavedChannel(1),
      mConnectionHandler([&](const uvpp::error& error) { connectionHandler(error); }),
      mReadHandler([&](const char* buf, ssize_t len) { readHandler(buf, len); }),
      mStopHandler([&]() { shutdown(); }),
      mStop(mLoop, mStopHandler)
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
                   if (e)
                       onError(UNKNOWN);
               });
}
 
void
Overflow::InterleavedTcpTransport::shutdown()
{
    mTcp.read_stop();
    mTcp.shutdown([&](uvpp::error) {
            mLoop.stop();
        });
}

void
Overflow::InterleavedTcpTransport::startConnectionTimer()
{
    // trim a few seconds to ensure keep-alive is sent in time
    uint64_t timeout = 3 * 10;
    
    mConnectionTimer.start([&]() {
            LOG(INFO) << "connection-timeout exceeded";
            
            uvpp::error e(1);
            connectionHandler(e);
        },
        std::chrono::duration<uint64_t, std::milli>(timeout));
}

void
Overflow::InterleavedTcpTransport::stop()
{
    mStop.send();
}

void
Overflow::InterleavedTcpTransport::start()
{
    onStateChange(CONNECTING);
    
    startConnectionTimer();
    mTcp.connect(mHost, mPort, mConnectionHandler);
    
    mLoop.run();
}

void
Overflow::InterleavedTcpTransport::connectionHandler(const uvpp::error& error)
{
    mConnectionTimer.stop();
    if (error)
    {
        LOG(INFO) << "failed to connect: tcp://" << mHost << ":" << mPort;
        onError(UNKNOWN); // TODO error types
        onStateChange(DISCONNECTED);
        return;
    }
    
    onStateChange(CONNECTED);
    LOG(INFO) << "Connected: tcp://" << mHost << ":" << mPort;

    // start reading
    mTcp.read_start<1024>(mReadHandler);
}

void
Overflow::InterleavedTcpTransport::readHandler(const char* buf, ssize_t len)
{
    bool isEof = buf == NULL or len < 0;
    if (isEof)
    {
        onStateChange(DISCONNECTED);
        return;
    }
    
    std::vector<unsigned char> response;
    if (mReceivedBuffer.size() > 0)
    {
        response.resize (mReceivedBuffer.size());
        std::copy (mReceivedBuffer.begin(), mReceivedBuffer.end(), response.begin());
        mReceivedBuffer.clear ();
        mReceivedBuffer.resize (0);
    }
    
    size_t response_offset = response.size ();
    size_t total_buffer_size = response_offset + len;
    response.resize(total_buffer_size);
    std::copy(buf, buf + len, response.begin() + response_offset);

    size_t read_size = readResponse(&(response[0]), response.size());
    if (read_size < total_buffer_size)
    {
        size_t trailing_length = total_buffer_size - read_size;
        mReceivedBuffer.resize (trailing_length);
        std::copy(response.begin() + read_size, response.end(), mReceivedBuffer.begin());
    }
}

size_t
Overflow::InterleavedTcpTransport::readResponse(const unsigned char* buffer,
                                                size_t length)
{
    size_t offset = 0;
    do {
        bool is_rtp = ((length - offset) > 4) && buffer[offset] == '$';
        bool is_rtsp = ((length - offset) > 8)
            and strncmp((const char*)buffer + offset, "RTSP/1.0", 8) == 0;
        
        bool is_announce = ((length - offset) > 8)
            and strncmp((const char*)buffer + offset, "ANNOUNCE", 8) == 0;
        
        bool is_redirect = ((length - offset) > 8)
            and strncmp((const char*)buffer + offset, "REDIRECT", 8) == 0;

        // dont have enough data
        if (not is_rtp and not is_rtsp and not is_announce and not is_redirect)
            break;

        if (is_announce)
        {
            // TODO
            break;
        }
        else if (is_redirect)
        {
            // TODO
            break;
        }
        else if (is_rtp)
        {
            int channel = static_cast<int>(buffer[offset + 1]);

            uint16_t network_order_packet_length;
            memcpy(&network_order_packet_length, buffer + offset + 2, 2);
            uint16_t packet_length = ntohs(network_order_packet_length);

            bool have_whole_packet = (length - (offset + 4)) >= packet_length;
            if (not have_whole_packet)
                break;
            
            if (channel == mRtpInterleavedChannel)
            {
                RtpPacket pack(buffer + offset + 4, packet_length);
                onRtpPacket(&pack);
            }
            // TODO RTCP
            
            offset += packet_length + 4;
        }
        else if (is_rtsp)
        {
            std::string string_response;
            string_response.resize(length - offset);
            std::copy(buffer + offset, buffer + length, string_response.begin());

            size_t body_pos = string_response.find("\r\n\r\n");
            bool has_body = body_pos != std::string::npos;
            if (not has_body)
                break;
            
            size_t pos = string_response.find("Content-Length:");
            size_t content_length = (pos != std::string::npos) ?
                static_cast<size_t>(atoi(&string_response[pos + 16])) :
                0;
                
            size_t headers_length = body_pos;
            const unsigned char *rtsp_buffer = buffer + offset;
            size_t rtsp_buffer_length = content_length + 4 + headers_length;

            bool has_full_rtsp_response = (length - offset) >= rtsp_buffer_length;
            if (not has_full_rtsp_response)
                break;

            Response response(rtsp_buffer, rtsp_buffer_length);
            onRtspResponse(&response);
            offset += rtsp_buffer_length;
        }        
    } while (offset < length);
    
    return offset;
}

