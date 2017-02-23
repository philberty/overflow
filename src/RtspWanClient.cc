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

#include "RtspWanClient.h"
#include "DescribeResponse.h"
#include "SetupResponse.h"

#include "H264Depacketizer.h"


Overflow::RtspWanClient::RtspWanClient(Overflow::IRtspDelegate * const delegate, const std::string& url)
    : m_url(url),
      m_loop(),
      m_keep_alive_timer(m_loop),
      m_rtsp_timeout_milliseconds(3000),
      m_thread(nullptr),
      m_delegate(delegate),
      m_transport(this, m_loop, url),
      m_factory(url),
      m_palette(),
      m_keepAliveIntervalInSeconds(60),
      m_session(),
      m_processedFirstPayload(false) { }

Overflow::RtspWanClient::~RtspWanClient()
{
    Stop();
}

bool Overflow::RtspWanClient::SendDescribeRequest()
{
    Describe* describe_request = m_factory.DescribeRequest();

    Response *resp = nullptr;
    bool received = SendRtsp(describe_request, resp);

    if (received == false) {
        LOG(ERROR) << "describe request timeout";
        delete describe_request;
        return false;
    }
    
    const DescribeResponse rtsp_resp(resp);
    bool ok = rtsp_resp.Ok();
    
    if (ok) {
        const std::vector<SessionDescription>& palettes = rtsp_resp.GetSessionDescriptions();
        m_palette = AskDelegateForPalette(palettes);
    }

    delete describe_request;
    delete resp;

    return ok;
}

bool Overflow::RtspWanClient::SendSetupRequest(bool serverAllowsAggregate)
{   
    // control url is where we put any more requests to
    std::string setup_url = (m_palette.IsControlUrlComplete()) ?
        m_palette.GetControl() :
        m_factory.GetPath() + "/" + m_palette.GetControl();

    LOG(INFO) << "SETUP URL: " << setup_url;
    
    // Gstreamer doesnt like using the control url for subsequent rtsp requests post setup
    // only applicable in non complete control url's.
    if (serverAllowsAggregate && !m_palette.IsControlUrlComplete()) {
        // this is the new url we need to use for all requests now
        m_factory.SetPath(setup_url);
    }
    else if (m_palette.IsControlUrlComplete()) {
        m_factory.SetPath(setup_url);
    }

    if (m_palette.GetType() == RtspSessionType::UNKNOWN) {
        LOG(ERROR) << "unknown palette type: " << m_palette.GetType();
        return false;
    }

    Setup* setup_request = m_factory.SetupRequest(m_transport.GetTransportHeaderString());

    Response *resp = nullptr;
    bool received = SendRtsp(setup_request, resp);
    if (received == false) {
        LOG(ERROR) << "Failed to get response for SETUP";
        delete setup_request;
        return false;
    }
            
    const SetupResponse rtsp_resp(resp);
    
    if (not rtsp_resp.IsInterleaved()) {
        LOG(ERROR) << "Unsupported transport in SETUP response";
        delete setup_request;
        delete resp;
        return false;
    }

    bool ok = rtsp_resp.Ok();

    if (ok)
    // ensure channel is inline with server, some servers in the wild change...
    m_transport.SetRtpInterleavedChannel(rtsp_resp.GetRtpInterleavedChannel());
    LOG(INFO) << "Rtp Interleaved Channel set to: " << rtsp_resp.GetRtpInterleavedChannel();
            
    m_keepAliveIntervalInSeconds = rtsp_resp.GetTimeoutSeconds();
    StartKeepAliveTimer();
    
    m_session = rtsp_resp.GetSession();

    LOG(INFO) << "RtspClient Session set to: " << m_session;
    LOG(INFO) << "RtspClient KeepAlive Interval set to: " << m_keepAliveIntervalInSeconds;

    delete setup_request;
    delete resp;

    return ok;
}

bool Overflow::RtspWanClient::SendPlayRequest()
{
    if (m_session.empty()) {
        return false;
    }

    m_factory.SetUrl(m_url);
    Play* play_request = m_factory.PlayRequest(m_session);

    Response* resp = nullptr;
    bool received = SendRtsp(play_request, resp);
    
    if (received == false) {
        delete play_request;
        return false;
    }

    RtspResponse rtsp_resp(resp->BytesPointer(), resp->PointerLength());
    bool ok = rtsp_resp.Ok();
    
    delete play_request;
    delete resp;

    return ok;
}

bool Overflow::RtspWanClient::SendPauseRequest()
{
    if (m_session.empty()) {
        return false;
    }

    Pause* pause_request = m_factory.PauseRequest(m_session);

    Response* resp = nullptr;
    bool received = SendRtsp(pause_request, resp);

    if (received == false) {
        delete pause_request;
        return false;
    }

    RtspResponse rtsp_resp(resp->BytesPointer(), resp->PointerLength());
    bool ok = rtsp_resp.Ok();

    delete pause_request;
    delete resp;
    
    return ok;
}

bool Overflow::RtspWanClient::SendOptionsRequst()
{
    if (m_session.empty()) {
        return false;
    }

    Options* options_request = m_factory.OptionsRequest();

    Response* resp = nullptr;
    bool received = SendRtsp(options_request, resp);

    if (received == false) {
        delete options_request;
        return false;
    }

    RtspResponse rtsp_resp(resp->BytesPointer(), resp->PointerLength());
    bool ok = rtsp_resp.Ok();

    delete options_request;
    delete resp;
    
    return ok;
}

bool Overflow::RtspWanClient::SendTeardownRequest()
{
    if (m_session.empty()) {
        LOG(WARNING) << "no current session to teardown";
        return false;
    }

    Teardown* teardown_request = m_factory.TeardownRequest(m_session);

    Response* resp = nullptr;
    bool received = SendRtsp(teardown_request, resp);

    if (received == false) {
        delete teardown_request;
        return false;
    }

    RtspResponse rtsp_resp(resp->BytesPointer(), resp->PointerLength());
    bool ok = rtsp_resp.Ok();

    delete teardown_request;
    delete resp;
    
    return ok;
}

bool Overflow::RtspWanClient::Start()
{
    m_thread = new std::thread([&]() {
            try {
                LOG(INFO) << "event-loop started on:  " << std::this_thread::get_id();
                m_transport.Start();
                m_loop.run();
                m_keep_alive_timer.stop();
                m_transport.Stop();
            } catch(std::exception& e) {
                LOG(INFO) << "exception from event loop: [" << e.what() << "]";
            }
        });


    // flag to cleanup
    bool failed = false;

    // connect
    bool did_connect = m_transport.WaitForConnection();
    if (not did_connect) {
        LOG(INFO) << "rtsp-wan-client failed to connect";
        failed = true;
    }

    if (did_connect) {
        // start with describe/setup/play
        bool received = SendDescribeRequest();
        if (received == false) {
            LOG(ERROR) << "DESCRIBE FAILED";
            failed = true;
        }
        else {
            received = SendSetupRequest();
            
            if (received == false) {
                LOG(ERROR) << "SETUP FAILED";
                failed = true;
            }
            else {
                received = SendPlayRequest();
                
                if (received == false) {
                    LOG(ERROR) << "PLAY FAILED";
                    failed = true;
                }
            }
        }
    }

    if (failed) {
        Stop();
        return false;
    }

    return true;
}

void Overflow::RtspWanClient::StartKeepAliveTimer()
{
    // uvpp::Async async_keep_alive(m_loop, [&]() {
            
    //         bool ok = SendOptionsRequst();
    //         if (!ok) {
    //             LOG(ERROR) << "Failed keep-alive";
                
    //         }
    //     });
    
    uint64_t timeout_millis = (m_keepAliveIntervalInSeconds - 5) * 1000;
    std::chrono::duration<uint64_t, std::milli> timeout(timeout_millis);
    m_keep_alive_timer.start([&]() {
            
            LOG(INFO) << "Keep alive request";
            
            bool ok = SendOptionsRequst();
            if (!ok) {
                LOG(ERROR) << "Failed keep-alive";
                
            }
            LOG(INFO) << ok;
        },
        std::chrono::duration<uint64_t, std::milli>(5000), timeout);
    
    LOG(INFO) << "started keep-alive timer";
}

void Overflow::RtspWanClient::OnRtpPacket(const RtpPacket* packet)
{
    ProcessRtpPacket(packet);
    delete packet;
}

void Overflow::RtspWanClient::Stop()
{
    if (m_thread == nullptr) {
        return;
    }
    
    SendTeardownRequest();
    
    LOG(INFO) << "Stopping RtspWanClient";
    uvpp::Async async(m_loop, [&]() {
            m_loop.stop();
            LOG(INFO) << "stopped event-loop core";
        });
    async.send();
    
    m_thread->join();
    delete m_thread;
    m_thread = nullptr;
    LOG(INFO) << "stopped event-loop thread";
}

bool Overflow::RtspWanClient::SendRtsp(Rtsp* const request, Response*& resp)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const ByteBuffer& buf = request->GetBuffer();

    LOG(INFO) << "Sending:: " << request->ToString();
    m_transport.Write(buf.BytesPointer(), buf.Length());

    return m_transport.WaitForResponse(resp, m_rtsp_timeout_milliseconds);
}

void Overflow::RtspWanClient::ProcessRtpPacket(const RtpPacket* packet) {

    switch (m_palette.GetType()) {
    case H264:
        ProcessH264Packet(packet);
        break;
        
    default:
        LOG(ERROR) << "Unhandled session type: " << m_palette.GetType();
        return;
    }

    m_processedFirstPayload = true;
    if (!packet->IsMarked()) {
        return;
    }

    size_t current_frame_size = GetCurrentFrameSize();
    const unsigned char *current_frame = GetCurrentFrame();

    // notify delegate of new payload
    NotifyDelegatePayload(current_frame, current_frame_size);

    // reset
    ResetCurrentPayload();
}

void Overflow::RtspWanClient::ProcessH264Packet(const RtpPacket* packet) {
    H264Depacketizer depacketizer(&m_palette, packet, IsFirstPayload());

    const unsigned char *payload = depacketizer.PayloadBytes();
    size_t payload_size = depacketizer.PayloadLength();

    AppendPayloadToCurrentFrame(payload, payload_size);
}
