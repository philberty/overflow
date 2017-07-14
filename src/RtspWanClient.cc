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

#include <glog/logging.h>
#include <uvpp/async.hpp>


Overflow::RtspWanClient::RtspWanClient(IRtspDelegate * const delegate,
                                       const std::string& url)
    : mDelegate(delegate),
      mUrl(url),
      mFactory(url),
      mLoop(),
      mTcpTransport(this, mLoop, url),
      mTransport(&mTcpTransport),
      mEventLoop(nullptr),
      mState(CLIENT_INITILIZED)
{ }

Overflow::RtspWanClient::~RtspWanClient()
{
    stop();
}

bool
Overflow::RtspWanClient::start()
{
    mEventLoop = new std::thread([&]() {
            try {
                LOG(INFO) << "event-loop started on:  " << std::this_thread::get_id();
                mTransport->connect ();
                mLoop.run ();
            } catch(std::exception& e) {
                LOG(INFO) << "exception from event loop: [" << e.what() << "]";
            }
        });    
    return true;
}

void
Overflow::RtspWanClient::stop()
{
    if (mEventLoop == nullptr)
        return;

    LOG(INFO) << "Stopping RtspWanClient";
    uvpp::Async async(mLoop, [&]() {
            mLoop.stop();
            LOG(INFO) << "stopped event-loop core";
        });
    async.send();
    
    mEventLoop->join();
    delete mEventLoop;
    mEventLoop = nullptr;
    
    LOG(INFO) << "stopped event-loop thread";
}

void
Overflow::RtspWanClient::onRtpPacket(const RtpPacket* packet)
{
    LOG(INFO) << "onRtpPacket";
}

void
Overflow::RtspWanClient::onRtspResponse(const Response* response)
{
    RtspClientState oldState = mState;
    onStateChange(CLIENT_RECEIVED_RESPONSE);

    if (oldState == CLIENT_SENDING_OPTIONS)
    {
        // HANDLE OPTIONS RESPONSE
        LOG(INFO) << "Received: "
                  << response->getStringBuffer();
        RtspResponse resp(response);

        if (not resp.ok())
            onStateChange(CLIENT_ERROR);
        else
            onStateChange(CLIENT_OPTIONS_OK);
    }

}

void
Overflow::RtspWanClient::onStateChange(TransportState oldState,
                                       TransportState newState)
{
    LOG(INFO) << "transport-state-change: "
              <<  stateToString(oldState) << "::old-state - "
              << stateToString(newState) << "::new-state";

    if (newState == CONNECTING)
        onStateChange(CLIENT_CONNECTING);
    else if (newState == CONNECTED)
        onStateChange(CLIENT_CONNECTED);

    if (oldState == CONNECTING and newState == CONNECTED)
    {
        // SEND OPTIONS
        sendOptionsRequest();
    }
}

void
Overflow::RtspWanClient::onTransportError(TransportErrorReason reason)
{
    LOG(INFO) << "transport-error: " << stateToString(reason);
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
    const ByteBuffer& buf = options->getBuffer();

    mTransport->write(buf.bytesPointer(),
                      buf.length());
    delete options;
}
