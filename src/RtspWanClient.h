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

#include <functional>

#include "IRtspDelegate.h"
#include "ITransportDelegate.h"
#include "InterleavedTcpTransport.h"
#include "RtspFactory.h"
#include "SessionDescription.h"
#include "RtpPacket.h"

#include <uvpp/loop.hpp>
#include <uvpp/async.hpp>
#include <uvpp/timer.hpp>
#include <uvpp/work.hpp>

#include <string>
#include <thread>
#include <functional>
#include <mutex>

namespace Overflow {

    class RtspWanClient: protected ITransportDelegate {
    public:
        RtspWanClient(IRtspDelegate * const delegate, const std::string& url);

        virtual ~RtspWanClient();

        bool Start();

        void Stop();

        bool SendPlayRequest();

        bool SendPauseRequest();

    protected:
        
        // ITransportDelegate
        void OnRtpPacket(const RtpPacket*) override;
        
        void OnAnnounce() override { }

        void OnRedirect() override { }

        void OnSocketWriteError() override { }

    private:

        void StartKeepAliveTimer();

        bool SendDescribeRequest();

        bool SendSetupRequest(bool serverAllowsAggregate=true);

        bool SendOptionsRequst();

        bool SendTeardownRequest();

        SessionDescription AskDelegateForPalette(const std::vector<SessionDescription>& palettes)
        {
            return palettes[0];
        }

        void NotifyDelegateOfTimeout() {
            if (m_delegate != nullptr) {
                m_delegate->Timeout();
            }
        }

        void NotifyDelegatePayload(const unsigned char * buffer, size_t size) {
            if (m_delegate != nullptr) {
                m_delegate->Payload(buffer, size);
            }
        }

        void AppendPayloadToCurrentFrame(const unsigned char *buffer, size_t length) {
            size_t old_size = m_currentFrame.size();
            m_currentFrame.resize(old_size + length);
            std::copy(buffer, buffer + length, m_currentFrame.begin() + old_size);
        }

        void ResetCurrentPayload() {
            m_currentFrame.clear();
            m_currentFrame.resize(0);
        }

        const unsigned char * GetCurrentFrame() const { return &(m_currentFrame[0]); }

        size_t GetCurrentFrameSize() const { return m_currentFrame.size(); }

        bool IsFirstPayload() const { return !m_processedFirstPayload; }

        bool SendRtsp(Rtsp* const rtsp, Response*& resp);

        void ProcessRtpPacket(const RtpPacket* packet);

        void ProcessH264Packet(const RtpPacket* packet);

        std::string m_url;
        
        uvpp::loop m_loop;
        uvpp::Timer m_keep_alive_timer;
        
        int m_rtsp_timeout_milliseconds;
        
        std::thread* m_thread;
        IRtspDelegate * const m_delegate;
        InterleavedTcpTransport m_transport;
        
        RtspFactory m_factory;
        SessionDescription m_palette;

        int m_keepAliveIntervalInSeconds;
        std::string m_session;
        bool m_processedFirstPayload;
        std::vector<unsigned char> m_currentFrame;
    };
    
};


#endif //__RTSP_WAN_CLIENT_H__
