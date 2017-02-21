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

#include <glog/logging.h>
#include <uvpp/resolver.hpp>

#include <exception>

#include "InterleavedTcpTransport.h"
#include "Url.h"


Overflow::InterleavedTcpTransport::InterleavedTcpTransport(ITransportDelegate * const delegate,
                                                           uvpp::loop& loop,
                                                           const std::string& url)
    : m_delegate(delegate),
      m_loop(loop),
      m_tcp(loop),
      m_host(),
      m_port(0),
      m_did_connect(false),
      m_rtpInterleavedChannel(0),
      m_rtcpInterleavedChannel(1),
      m_receivedBuffer()
{
    Url uri(url);
    
    m_host = uri.GetHost();
    m_port = uri.GetPort();
}

void Overflow::InterleavedTcpTransport::Write(const unsigned char *buffer, const size_t length)
{    
    m_tcp.write((const char *)buffer, (int)length, [&](uvpp::error e) {
            NotifyDelegateOfWriteFailure();
        });
}

void Overflow::InterleavedTcpTransport::Stop()
{
    m_tcp.shutdown([](uvpp::error){ });
}

void Overflow::InterleavedTcpTransport::Start()
{
    auto connection_handler = [&](const uvpp::error& error) {
        if (error) {
            LOG(INFO) << "failed to connect: tcp://" << m_host << ":" << m_port;
            return;
        }
        
        LOG(INFO) << "Connected: tcp://" << m_host << ":" << m_port;
        m_did_connect = true;
    };

    m_tcp.connect(m_host, m_port, connection_handler);
    m_loop.run_once();

    if (not m_did_connect) {
        Stop();
        throw std::runtime_error("failed to connect");
    }
    NotifyOfConnection();
    
    m_tcp.read_start([&](const char* buf, ssize_t len) {

            std::vector<unsigned char> response;
    
            if (m_receivedBuffer.size() > 0) {
                // copy trailing data into response
                response.resize(m_receivedBuffer.size());
                std::copy(m_receivedBuffer.begin(), m_receivedBuffer.end(), response.begin());
                m_receivedBuffer.clear();
                m_receivedBuffer.resize(0);
            }

            size_t response_offset = response.size();
            size_t total_buffer_size = response_offset + len;
            response.resize(total_buffer_size);
            std::copy(buf, buf + len, response.begin() + response_offset);
            
            Read(response);
        });
}

void Overflow::InterleavedTcpTransport::Read(const std::vector<unsigned char>& response)
{
    size_t total_buffer_size = response.size();
    
    size_t offs = 0;
    do {
        bool is_rtp = ((total_buffer_size - offs) > 4) && response[offs] == '$';
        bool is_rtsp = ((total_buffer_size - offs) > 8) && strncmp((char*)&(response[offs]), "RTSP/1.0", 8) == 0;
        bool is_announce = ((total_buffer_size - offs) > 8) && strncmp((char*)&(response[offs]), "ANNOUNCE", 8) == 0;
        bool is_redirect = ((total_buffer_size - offs) > 8) && strncmp((char*)&(response[offs]), "REDIRECT", 8) == 0;

        if (is_announce) {
            NotifyDelegateOfAnnounce();
            break;
        }
        else if (is_redirect) {
            NotifyDelegateOfRedirect();
            break;
        }
        else if (is_rtp) {
            int channel = static_cast<int>(response[offs + 1]);

            uint16_t network_order_packet_length = 0;
            memcpy(&network_order_packet_length, &(response[offs + 2]), 2);
            uint16_t packet_length = ntohs(network_order_packet_length);

            bool have_whole_packet = (total_buffer_size - (offs + 4)) >= packet_length;
            if (have_whole_packet && channel == m_rtpInterleavedChannel) {
                try {
                    RtpPacket *pack = new RtpPacket(&(response[offs + 4]), packet_length);
                    NotifyDelegateOfRtpPacket(pack);
                    
                } catch (const std::exception& e) {
                    LOG(ERROR) << "Invalid Rtp Packet: " << e.what();
                }
            } else if (!have_whole_packet) {
                // copy into received buffer
                size_t trailing_length = total_buffer_size - offs;
                m_receivedBuffer.resize(trailing_length);
                std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
                break;
            }
            offs += packet_length + 4;
        }
        else if (is_rtsp) {
            std::string string_response;
            string_response.resize(total_buffer_size - offs);
            std::copy(response.begin() + offs, response.end(), string_response.begin());

            size_t body_pos; 
            if ((body_pos = string_response.find("\r\n\r\n")) != std::string::npos) {
                size_t pos = string_response.find("Content-Length:");
                size_t content_length = (pos != std::string::npos) ?
                    static_cast<size_t>(atoi(&string_response[pos + 16])) :
                    0;
                
                size_t headers_length = body_pos;
                
                const unsigned char *rtsp_buffer = &(response[offs]);
                size_t rtsp_buffer_length = content_length + 4 + headers_length;

                if ((total_buffer_size - offs) < rtsp_buffer_length) {
                    size_t trailing_length = total_buffer_size - offs;
                    m_receivedBuffer.resize(trailing_length);
                    std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
                    break;
                }

                Response *response = new Response(rtsp_buffer, rtsp_buffer_length);
                NotifyOfRtspResponse(response);
                
                offs += rtsp_buffer_length;
            } else {
                size_t trailing_length = total_buffer_size - offs;
                m_receivedBuffer.resize(trailing_length);
                std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
                break;
            }
        }
        else {
            size_t trailing_length = total_buffer_size - offs;
            m_receivedBuffer.resize(trailing_length);
            std::copy(response.begin() + offs, response.end(), m_receivedBuffer.begin());
            break;
        }
    } while (offs < total_buffer_size);
}
