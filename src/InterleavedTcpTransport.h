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

#include <uvpp/loop.hpp>
#include <uvpp/tcp.hpp>
#include <blockingconcurrentqueue.h>
#include <glog/logging.h>

#include "ITransportDelegate.h"
#include "RtpPacket.h"
#include "Response.h"


namespace Overflow {
    
    class InterleavedTcpTransport {
    public:
        InterleavedTcpTransport(ITransportDelegate * const delegate, uvpp::loop& loop, const std::string& url);

        void Write(const unsigned char *buffer, const size_t length);

        void SetRtpInterleavedChannel(int channel) { m_rtpInterleavedChannel = channel; }

        std::string GetTransportHeaderString() const {
            char rtp_channel[12];
            char rtcp_channel[12];
            memset(rtp_channel, 0, sizeof(rtp_channel));
            memset(rtcp_channel, 0, sizeof(rtcp_channel));
            snprintf(rtp_channel, sizeof(rtp_channel), "%d", m_rtpInterleavedChannel);
            snprintf(rtcp_channel, sizeof(rtcp_channel), "%d", m_rtcpInterleavedChannel);

            return "RTP/AVP/TCP;unicast;interleaved="
                + std::string(rtp_channel)
                + "-"
                + std::string(rtcp_channel);
        }

        void Start();

        void Stop();

        bool WaitForResponse(Response*& resp, int timeout_millis) {
            if (not IsRunning()) {
                return false;
            }
            
            std::int64_t timeout_usecs = timeout_millis * 1000;
            return m_responseQueue.wait_dequeue_timed(resp, timeout_usecs);
        }

        bool IsRunning() {
            return m_did_connect;
        }

        bool WaitForConnection() {
            int timeout_millis = 3000;
            std::int64_t timeout_usecs = timeout_millis * 1000;
            
            Response *resp;
            return m_responseQueue.wait_dequeue_timed(resp, timeout_usecs);
        }

    private:

        void Read(const std::vector<unsigned char>& response);

        void NotifyOfConnection() {
            m_responseQueue.enqueue(nullptr);
        }

        void NotifyOfRtspResponse(Response* resp) {
            std::string resp_str = resp->GetStringBuffer();
            LOG(INFO) << "Received: " << resp_str;
            m_responseQueue.enqueue(resp);
        }

        void NotifyDelegateOfRtpPacket(const RtpPacket* packet) {
            if (m_delegate != nullptr) {
                m_delegate->OnRtpPacket(packet);
            }
        }

        void NotifyDelegateOfAnnounce() {
            if (m_delegate != nullptr) {
                m_delegate->OnAnnounce();
            }
        }

        void NotifyDelegateOfRedirect() {
            if (m_delegate != nullptr) {
                m_delegate->OnRedirect();
            }
        }

        void NotifyDelegateOfWriteFailure() {
            if (m_delegate != nullptr) {
                m_delegate->OnSocketWriteError();
            }
        }        
        
        ITransportDelegate* const m_delegate;
        uvpp::loop& m_loop;
        uvpp::Tcp m_tcp;

        std::string m_host;
        int m_port;
        
        bool m_did_connect;

        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;

        std::vector<unsigned char> m_receivedBuffer;
        moodycamel::BlockingConcurrentQueue<Response*> m_responseQueue;
    };
};

#endif //__INTERLEAVED_TCP_TRANSPORT_H__
