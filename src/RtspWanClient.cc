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

#include "RtspWanClient.h"
#include "RtspResponse.h"
#include "DescribeResponse.h"
#include "SetupResponse.h"

#include "H264Depacketizer.h"
#include "MP4VDepacketizer.h"
#include "MJPEGDepacketizer.h"

#include <chrono>
#include <glog/logging.h>
#include <uvpp/async.hpp>


Overflow::RtspWanClient::RtspWanClient(IRtspDelegate * const delegate,
                                       const std::string& url)
    : mDelegate(delegate),
      mUrl(url),
      mFactory(url),
      mLoop(),
      mKeepAliveTimer(mLoop),
      mRtspRequestTimeoutTimer(mLoop),
      mTcpTransport(this, mLoop, url),
      mTransport(&mTcpTransport),
      mEventLoop(nullptr),
      mState(CLIENT_INITILIZED),
      mServerAllowsAggregate(false),
      mLastSeqNum(-1),
      mIsFirstPayload(true),
      mStopEventLoopHandler([&]() { stopEventLoop(); }),
      mStopEventLoop(mLoop, mStopEventLoopHandler)
{ }

Overflow::RtspWanClient::~RtspWanClient()
{
    stop();
    join();
}

void
Overflow::RtspWanClient::start()
{
    if (not isRunning())
        mEventLoop = new std::thread([&]() { eventLoopMain(); });
}

void
Overflow::RtspWanClient::eventLoopMain()
{
    try
    {
        auto mEventThreadId = std::this_thread::get_id();
        LOG(INFO) << "event-loop started on:  " << mEventThreadId;
        mTransport->connect ();
        mLoop.run ();
    }
    catch (std::exception& e)
    {
        LOG(INFO) << "exception from event loop: [" << e.what() << "]";
        onStateChange(CLIENT_ERROR);
    }
}

void
Overflow::RtspWanClient::stop()
{
    if (isRunning())
        mStopEventLoop.send();
}

void
Overflow::RtspWanClient::stopEventLoop()
{
    LOG(INFO) << "stopping event-loop";
    mTransport->shutdown();
    mKeepAliveTimer.stop();
    mLoop.stop();
    LOG(INFO) << "stopped event-loop core";
}

void
Overflow::RtspWanClient::join()
{
    if (not isRunning())
        return;

    mEventLoop->join();
    mEventLoop = nullptr;
}

bool
Overflow::RtspWanClient::isRunning() const
{
    return mEventLoop != nullptr and mEventLoop->joinable();
}

void
Overflow::RtspWanClient::onRtpPacket(const RtpPacket* packet)
{
    int seq_num = packet->getSequenceNumber ();
    bool initialized_last_seq_num = mLastSeqNum != -1;    
    if (initialized_last_seq_num)
    {
        bool out_of_sequence = (mLastSeqNum + 1) != seq_num;
        if (out_of_sequence)
        {
            LOG(ERROR) << "out of sequence rtp-packets: "
                       << mLastSeqNum << "[LAST] - "
                       << seq_num << "[CURRENT]";
        }
    }
    mLastSeqNum = seq_num;

    switch (mPalette.getType())
    {
    case H264:
        processH264Packet(packet);
        break;

    case MP4V:
        processMP4VPacket(packet);
        break;

    case MJPEG:
        processMJPEGPacket(packet);
        break;
        
    default:
        LOG(ERROR) << "Unhandled session type: "
                   << mPalette.getType();
        onStateChange(CLIENT_ERROR);
        return;
    }

    mIsFirstPayload = false;
    if (not packet->isMarked())
        return;

    notifyDelegateOfPayload();
    resetCurrentPayload();
}

void
Overflow::RtspWanClient::processH264Packet(const RtpPacket* packet)
{
    H264Depacketizer depacketizer(&mPalette, packet, mIsFirstPayload);

    const unsigned char *payload = depacketizer.bytes();
    size_t payload_size = depacketizer.length();

    appendPayloadToCurrentFrame(payload, payload_size);
}

void
Overflow::RtspWanClient::processMP4VPacket(const RtpPacket* packet)
{
    MP4VDepacketizer depacketizer(&mPalette, packet, mIsFirstPayload);

    const unsigned char *payload = depacketizer.bytes();
    size_t payload_size = depacketizer.length();

    appendPayloadToCurrentFrame(payload, payload_size);
}

void
Overflow::RtspWanClient::processMJPEGPacket(const RtpPacket* packet)
{
    MJPEGDepacketizer depacketizer(&mPalette, packet, mIsFirstPayload);

    depacketizer.addToFrame(&mCurrentFrame);
}

size_t
Overflow::RtspWanClient::getCurrentFrameSize() const
{
    return mCurrentFrame.size();
}

const unsigned char*
Overflow::RtspWanClient::getCurrentFrame() const
{
    return &(mCurrentFrame[0]);
}

void
Overflow::RtspWanClient::appendPayloadToCurrentFrame(const unsigned char* buffer,
                                                     size_t length)
{
    size_t old_size = mCurrentFrame.size();
    mCurrentFrame.resize(old_size + length);
    std::copy(buffer, buffer + length, mCurrentFrame.begin() + old_size);
}

void
Overflow::RtspWanClient::resetCurrentPayload()
{
    mCurrentFrame.clear();
    mCurrentFrame.resize(0);
}

void
Overflow::RtspWanClient::notifyDelegateOfPayload()
{
    if (mDelegate != nullptr)
        mDelegate->onPayload(getCurrentFrame(),
                             getCurrentFrameSize());
}

void
Overflow::RtspWanClient::onRtspResponse(const Response* response)
{
    RtspClientState oldState = mState;
    onStateChange(CLIENT_RECEIVED_RESPONSE);

    RtspResponse rtsp(response);
    if (oldState == CLIENT_SENDING_OPTIONS)
    {
        onOptionsResponse(response);
    }
    else if (oldState == CLIENT_SENDING_DESCRIBE)
    {
        onDescribeResponse(response);
    }
    else if (oldState == CLIENT_SENDING_SETUP)
    {
        onSetupResponse(response);
    }
    else if (oldState == CLIENT_SENDING_PLAY)
    {
        onPlayResponse(response);
    }
    else if (oldState == CLIENT_SENDING_PAUSE)
    {
        onPauseResponse(response);
    }
    else
    {
        LOG(INFO) << response->getStringBuffer();
        
        onStateChange(
            rtsp.ok() ?
              CLIENT_RECEIVED_RESPONSE
            : CLIENT_ERROR
            );
    }
}

void
Overflow::RtspWanClient::onOptionsResponse(const Response* response)
{
    // HANDLE OPTIONS RESPONSE
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    RtspResponse resp(response);
    
    if (not resp.ok())
        onStateChange(CLIENT_ERROR);
    else
    {
        onStateChange(CLIENT_OPTIONS_OK);

        bool haveSession = not mSession.empty();
        if (not haveSession)
            sendDescribeRequest();
    }
}

void
Overflow::RtspWanClient::onDescribeResponse(const Response* response)
{
    // HANDLE DESCRIBE RESPONSE
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    DescribeResponse resp(response);
    
    if (not resp.ok())
        onStateChange(CLIENT_ERROR);
    else
    {
        onStateChange(CLIENT_DESCRIBE_OK);
        mPalette = resp.getSessionDescriptions()[0];
        notifyDelegateOfPaletteType();
        sendSetupRequest();
    }
}

void
Overflow::RtspWanClient::onSetupResponse(const Response* response)
{
    // HANDLE SETUP RESPONSE
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    try
    {
        SetupResponse resp(response);       
        
        if (not resp.ok())
            onStateChange(CLIENT_ERROR);
        else
        {
            onStateChange(CLIENT_SETUP_OK);
            mSession = resp.getSession();
            startKeepAliveTimer(resp.getTimeoutSeconds());
            sendPlayRequest();
        }
    }
    catch (std::exception& e)
    {
        LOG(ERROR) << "setup-response: " << e.what();
        onStateChange(CLIENT_ERROR);
    }
}

void
Overflow::RtspWanClient::onPlayResponse(const Response* response)
{
    // HANDLE PLAY RESPONSE
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    RtspResponse resp(response);
    onStateChange(resp.ok() ? CLIENT_PLAY_OK :CLIENT_ERROR);
}

void
Overflow::RtspWanClient::onPauseResponse(const Response* response)
{
    // HANDLE PAUSE RESPONSE
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    RtspResponse resp(response);
    onStateChange(resp.ok() ? CLIENT_PLAY_OK :CLIENT_ERROR);
}

void
Overflow::RtspWanClient::onStateChange(TransportState oldState,
                                       TransportState newState)
{
    LOG(INFO) << "transport-state-change: "
              <<  stateToString(oldState)
              << " -> "
              << stateToString(newState);

    if (newState == CONNECTING)
        onStateChange(CLIENT_CONNECTING);
    else if (newState == CONNECTED)
        onStateChange(CLIENT_CONNECTED);

    if (newState == CONNECTED)
    {
        // SEND OPTIONS
        sendOptionsRequest();
    }
    else if (newState == DISCONNECTED)
    {
        mSession.clear();

        
    }
}

void
Overflow::RtspWanClient::onTransportError(TransportErrorReason reason)
{
    LOG(INFO) << "transport-error: " << stateToString(reason);
    onStateChange(CLIENT_ERROR);
}

void
Overflow::RtspWanClient::onStateChange(RtspClientState state)
{
    RtspClientState oldState = mState;
    mState = state;

    notifyDelegateOfStateChange (oldState, mState);
}

void
Overflow::RtspWanClient::notifyDelegateOfStateChange(RtspClientState oldState,
                                                     RtspClientState newState)
{
    if (mDelegate != nullptr)
        mDelegate->onRtspClientStateChange (oldState, newState);
}

void
Overflow::RtspWanClient::sendOptionsRequest()
{
    onStateChange(CLIENT_SENDING_OPTIONS);
    
    Options* options = mFactory.optionsRequest();
    sendRtsp(options);
    delete options;
}

void
Overflow::RtspWanClient::sendDescribeRequest()
{
    onStateChange(CLIENT_SENDING_DESCRIBE);
    
    Describe* describe = mFactory.describeRequest(true);
    sendRtsp(describe);
    delete describe;
}

void
Overflow::RtspWanClient::sendSetupRequest()
{
    if (mPalette.getType() == RtspSessionType::UNKNOWN_PALETTE)
    {
        LOG(ERROR) << "unknown palette type: " << mPalette.getType();
        onStateChange(CLIENT_ERROR);
        return;
    }
    
    onStateChange(CLIENT_SENDING_SETUP);

    // control url is where we put any more requests to
    std::string setup_url = (mPalette.isControlUrlComplete()) ?
        mPalette.getControl() :
        mFactory.getPath() + "/" + mPalette.getControl();
    
    // Gstreamer doesnt like using the control url for subsequent rtsp requests post setup
    // only applicable in non complete control url's.
    if (mServerAllowsAggregate && !mPalette.isControlUrlComplete())
    {
        // this is the new url we need to use for all requests now
        mFactory.setPath(setup_url);
    }
    else if (mPalette.isControlUrlComplete())
    {
        mFactory.setPath(setup_url);
    }

    Setup* setup = mFactory.setupRequest(
        mTransport->getTransportHeaderString());
    sendRtsp(setup);
    delete setup;
}

void
Overflow::RtspWanClient::sendPlayRequest()
{
    onStateChange(CLIENT_SENDING_PLAY);

    Play* play = mFactory.playRequest(mSession);
    sendRtsp(play);
    delete play;
}

void
Overflow::RtspWanClient::sendPauseRequest()
{
    onStateChange(CLIENT_SENDING_PAUSE);

    Pause* pause = mFactory.pauseRequest(mSession);
    sendRtsp(pause);
    delete pause;
}

void
Overflow::RtspWanClient::sendTeardownRequest()
{
    Teardown* teardown = mFactory.teardownRequest(mSession);
    sendRtsp(teardown);
    delete teardown;
}

void
Overflow::RtspWanClient::sendRtsp(Rtsp* request)
{
    const ByteBuffer& buf = request->getBuffer();

    mTransport->write(buf.bytesPointer(),
                      buf.length());
}

void
Overflow::RtspWanClient::startKeepAliveTimer(int seconds)
{
    // trim a few seconds to ensure keep-alive is sent in time
    uint64_t timeout = (seconds - 5) * 1000;
    
    mKeepAliveTimer.start([&]() { sendOptionsRequest (); },
        std::chrono::duration<uint64_t, std::milli>(timeout),
        std::chrono::duration<uint64_t, std::milli>(timeout));
}

void
Overflow::RtspWanClient::notifyDelegateOfPaletteType()
{
    if (mDelegate != nullptr)
        mDelegate->onPaletteType(mPalette.getType());
}
