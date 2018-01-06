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

#include "RtspController.h"
#include "RtspResponse.h"
#include "DescribeResponse.h"
#include "SetupResponse.h"

#include "H264Depacketizer.h"
#include "MP4VDepacketizer.h"
#include "MJPEGDepacketizer.h"

#include <glog/logging.h>


Overflow::RtspController::RtspController (IRtspDelegate* delegate,
                                          std::string url)
    : TransportController (),
      mDelegate (delegate),
      mUrl (url),
      mFactory (mUrl),
      mState (CLIENT_INITILIZED),
      mServerAllowsAggregate (false),
      mLastSeqNum (-1),
      mIsFirstPayload (true)
{
    
}

Overflow::RtspController::~RtspController ()
{
    
}

bool
Overflow::RtspController::haveSession () const
{
    return not mSession.empty();
}

void
Overflow::RtspController::standby ()
{
    if (isConnected () and haveSession ())
        sendTeardownRequest ();
    
    stopReconnectTimer ();
    stopTransport ();
    
    resetClientState ();
}

void
Overflow::RtspController::sendPlayRequest ()
{
    onStateChange (CLIENT_SENDING_PLAY);

    Play* play = mFactory.playRequest (mSession);
    sendRtsp (play);
    delete play;
}

void
Overflow::RtspController::sendPauseRequest ()
{
    onStateChange (CLIENT_SENDING_PAUSE);

    Pause* pause = mFactory.pauseRequest(mSession);
    sendRtsp (pause);
    delete pause;
}

void
Overflow::RtspController::onKeepAlive ()
{
    sendKeepAlive ();
}

void
Overflow::RtspController::onRtpPacket (const RtpPacket* packet)
{
    int seqNum = packet->getSequenceNumber ();
    bool initialized_last_seq_num = mLastSeqNum != -1;    
    if (initialized_last_seq_num)
    {
        bool out_of_sequence = (mLastSeqNum + 1) != seqNum;
        if (out_of_sequence)
        {
            LOG(ERROR) << "out of sequence rtp-packets: "
                       << mLastSeqNum << "[LAST] - "
                       << seqNum << "[CURRENT]";
        }
    }
    mLastSeqNum = seqNum;

    if (packet->hasExtension ())
    {
        notifyDelegateOfExtension (packet);
    }

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
Overflow::RtspController::onRtspResponse(const Response* response)
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
Overflow::RtspController::onStateChange(TransportState oldState,
                                        TransportState newState)
{
    if (newState == CONNECTING)
    {
        onStateChange (CLIENT_CONNECTING);
    }
    else if (newState == CONNECTED)
    {
        onTransportConnected ();
        onStateChange (CLIENT_CONNECTED);
        
        // SEND OPTIONS
        sendOptionsRequest ();
    }
    else if (newState == DISCONNECTED)
    {
        onStateChange (CLIENT_DISCONNECTED);
        
        stopTransportAsync ();
        resetClientState ();
        
        if (not isReconnecting())
            reconnect ();
    }
    else
    {
        LOG(INFO) << "transport-state-change: "
                  <<  stateToString(oldState)
                  << " -> "
                  << stateToString(newState);
    }
}
   
void
Overflow::RtspController::onTransportError(TransportErrorReason reason)
{
    LOG(INFO) << "transport-error: " << stateToString(reason);
    onStateChange(CLIENT_ERROR);
}
        
void
Overflow::RtspController::onStateChange(RtspClientState state)
{
    RtspClientState oldState = mState;
    mState = state;

    notifyDelegateOfStateChange (oldState, mState);
}

void
Overflow::RtspController::notifyDelegateOfPayload ()
{
    if (mDelegate != nullptr)
        mDelegate->onPayload(getCurrentFrame(),
                             getCurrentFrameSize());
}

void
Overflow::RtspController::notifyDelegateOfExtension (const RtpPacket* packet)
{
    if (mDelegate != nullptr)
        mDelegate->onRtpPacketExtension(
            packet->getExtensionID (),
            packet->getExtensionData (),
            packet->getExtensionLength ());
}

void
Overflow::RtspController::notifyDelegateOfPaletteType ()
{
    if (mDelegate != nullptr)
        mDelegate->onPaletteType(mPalette.getType());
}

void
Overflow::RtspController::processH264Packet(const RtpPacket* packet)
{
    H264Depacketizer depacketizer(&mPalette, packet, mIsFirstPayload);

    const unsigned char *payload = depacketizer.bytes();
    size_t payload_size = depacketizer.length();

    appendPayloadToCurrentFrame(payload, payload_size);
}

void
Overflow::RtspController::processMP4VPacket(const RtpPacket* packet)
{
    MP4VDepacketizer depacketizer(&mPalette, packet, mIsFirstPayload);

    const unsigned char *payload = depacketizer.bytes();
    size_t payload_size = depacketizer.length();

    appendPayloadToCurrentFrame(payload, payload_size);
}

void
Overflow::RtspController::processMJPEGPacket(const RtpPacket* packet)
{
    MJPEGDepacketizer depacketizer(&mPalette, packet, mIsFirstPayload);

    depacketizer.addToFrame(&mCurrentFrame);
}

void
Overflow::RtspController::resetCurrentPayload()
{
    mCurrentFrame.clear ();
    mCurrentFrame.resize (0);
}

void
Overflow::RtspController::resetClientState()
{
    mSession.clear();
    mLastSeqNum = -1;
    mIsFirstPayload = true;
    resetCurrentPayload ();
}

size_t
Overflow::RtspController::getCurrentFrameSize() const
{
    return mCurrentFrame.size();
}

const unsigned char*
Overflow::RtspController::getCurrentFrame() const
{
    return &(mCurrentFrame[0]);
}

void
Overflow::RtspController::appendPayloadToCurrentFrame(
    const unsigned char* buffer,
    size_t length)
{
    size_t old_size = mCurrentFrame.size();
    mCurrentFrame.resize(old_size + length);
    std::copy(buffer, buffer + length, mCurrentFrame.begin() + old_size);
}

void
Overflow::RtspController::notifyDelegateOfStateChange(
    RtspClientState oldState,
    RtspClientState newState)
{
    if (mDelegate != nullptr)
        mDelegate->onRtspClientStateChange (oldState, newState);
}

void
Overflow::RtspController::sendOptionsRequest()
{
    onStateChange(CLIENT_SENDING_OPTIONS);
    
    Options* options = mFactory.optionsRequest();
    sendRtsp(options);
    delete options;
}

void
Overflow::RtspController::onOptionsResponse(const Response* response)
{
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    RtspResponse resp(response);
    
    if (not resp.ok())
        onStateChange(CLIENT_ERROR);
    else
    {
        onStateChange (CLIENT_OPTIONS_OK);

        if (not haveSession ())
            sendDescribeRequest();
    }
}

void
Overflow::RtspController::sendDescribeRequest()
{
    onStateChange(CLIENT_SENDING_DESCRIBE);
    
    Describe* describe = mFactory.describeRequest (true);
    sendRtsp (describe);
    delete describe;
}

void
Overflow::RtspController::onDescribeResponse(const Response* response)
{
    LOG(INFO) << "Received: " << response->getStringBuffer ();
    DescribeResponse resp (response);
    
    if (not resp.ok())
        onStateChange (CLIENT_ERROR);
    else
    {
        onStateChange (CLIENT_DESCRIBE_OK);
        mPalette = resp.getSessionDescriptions()[0];
        notifyDelegateOfPaletteType ();
        sendSetupRequest ();
    }
}

void
Overflow::RtspController::sendSetupRequest()
{
    if (mPalette.getType() == RtspSessionType::UNKNOWN_PALETTE)
    {
        LOG(ERROR) << "unknown palette type: " << mPalette.getType ();
        onStateChange (CLIENT_ERROR);
        return;
    }
    
    onStateChange (CLIENT_SENDING_SETUP);

    // control url is where we put any more requests to
    std::string setup_url = (mPalette.isControlUrlComplete()) ?
        mPalette.getControl() :
        mFactory.getPath() + "/" + mPalette.getControl();
    
    // Gstreamer doesnt like using the control url for subsequent rtsp requests post setup
    // only applicable in non complete control url's.
    if (mServerAllowsAggregate && !mPalette.isControlUrlComplete ())
    {
        // this is the new url we need to use for all requests now
        mFactory.setPath (setup_url);
    }
    else if (mPalette.isControlUrlComplete ())
    {
        mFactory.setPath (setup_url);
    }

    Setup* setup = mFactory.setupRequest (getTransportHeaderString ());
    sendRtsp (setup);
    delete setup;
}

void
Overflow::RtspController::onSetupResponse(const Response* response)
{
    LOG(INFO) << "Received: " << response->getStringBuffer();
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
        onStateChange (CLIENT_ERROR);
    }
}

void
Overflow::RtspController::onPlayResponse(const Response* response)
{
    // HANDLE PLAY RESPONSE
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    RtspResponse resp(response);
    onStateChange (resp.ok() ? CLIENT_PLAY_OK :CLIENT_ERROR);
}

void
Overflow::RtspController::onPauseResponse(const Response* response)
{
    // HANDLE PAUSE RESPONSE
    LOG(INFO) << "Received: "
              << response->getStringBuffer();
    RtspResponse resp(response);
    onStateChange (resp.ok() ? CLIENT_PLAY_OK :CLIENT_ERROR);
}

void
Overflow::RtspController::sendTeardownRequest ()
{
    Teardown* teardown = mFactory.teardownRequest(mSession);
    sendRtsp (teardown);
    delete teardown;
}

void
Overflow::RtspController::sendKeepAlive ()
{
    onStateChange(CLIENT_SENDING_KEEP_ALIVE);
    
    Options* options = mFactory.optionsRequest ();
    sendRtsp (options, false);
    delete options;
}

void
Overflow::RtspController::sendRtsp (Rtsp* request, bool responseTimeout)
{
    int timeout = (responseTimeout) ? 3 : -1;
    const ByteBuffer& buf = request->getBuffer();
    sendRtspBytes (buf.bytesPointer (), buf.length (), timeout);
}
