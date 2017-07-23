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

#include "TransportController.h"

#include <chrono>
#include <glog/logging.h>


Overflow::TransportController::TransportController ()
    : mLoop(),
      mKeepAliveTimer(mLoop),
      mReconnectTimer(mLoop),
      mWorkers(mLoop),
      mTransport(nullptr),
      mEventLoop(nullptr),
      mStopEventLoopHandler([&]() { stopEventLoop(); }),
      mReconnectHandler([&]() { startReconnectTimer(); }),
      mStopTransportHandler([&]() { stopTransport(); }),
      mStopEventLoop(mLoop, mStopEventLoopHandler),
      mReconnect(mLoop, mReconnectHandler),
      mStopTransport(mLoop, mStopTransportHandler),
      mIsReconnecting(false)
{
}

Overflow::TransportController::~TransportController ()
{
    stop ();
    join ();
}

void
Overflow::TransportController::start ()
{
    startTransport ();
    
    if (not isRunning())
        mEventLoop = new std::thread([&]() { eventLoopMain(); });
}

void
Overflow::TransportController::stop ()
{
    if (isRunning())
        mStopEventLoop.send();
}

void
Overflow::TransportController::join ()
{
    if (not isRunning())
        return;

    mEventLoop->join();
    delete mEventLoop;
    mEventLoop = nullptr;
}

bool
Overflow::TransportController::isRunning () const
{
    return mEventLoop != nullptr and mEventLoop->joinable();
}

void
Overflow::TransportController::stopTransport ()
{
    std::lock_guard<std::mutex> guard(mMutex);
    if (mTransport == nullptr)
        return;

    sendTeardownRequest ();
    mTransport->stop ();
}

void
Overflow::TransportController::startTransport ()
{
    mWorkers.execute([&]() {
            mTransport = createTransport ();
            mTransport->start ();
        },
        [&](uvpp::error) {
            std::lock_guard<std::mutex> guard(mMutex);
            delete mTcpTransport;
            mTransport = nullptr;
        });
}

void
Overflow::TransportController::startReconnectTimer ()
{
    LOG(INFO) << "starting-reconnect-timer";
    
    uint64_t timeout = 3 * 1000;
    mIsReconnecting = true;
    
    mReconnectTimer.start([&]() { 
            startTransport ();
        },
        std::chrono::duration<uint64_t, std::milli>(timeout),
        std::chrono::duration<uint64_t, std::milli>(timeout));
}

void
Overflow::TransportController::isReconnecting () const
{
    return mIsReconnecting;
}

void
Overflow::TransportController::onTransportConnected ()
{
    mIsReconnecting = false;
    mReconnectTimer.stop ();
}

void
Overflow::TransportController::reconnect ()
{
    mReconnect.send ();
}

void
Overflow::TransportController::eventLoopMain ()
{
    auto mEventThreadId = std::this_thread::get_id();
    LOG(INFO) << "event-loop started on:  " << mEventThreadId;
    mLoop.run ();
}
        
void
Overflow::TransportController::stopEventLoop ()
{
    LOG(INFO) << "stopping event-loop";
    
    mReconnectTimer.stop ();
    mKeepAliveTimer.stop ();
    stopTransport ();
    
    mLoop.stop ();
    
    LOG(INFO) << "stopped event-loop core";   
}
        
void
Overflow::TransportController::send (const unsigned char* buffer,
                                     size_t length,
                                     int timeout)
{
    mTransport->writeRtsp (buf.bytesPointer(),
                           buf.length(),
                           timeout);
}

void
Overflow::TransportController::startKeepAliveTimer (int seconds)
{
    // trim a few seconds to ensure keep-alive is sent in time
    uint64_t timeout = (seconds - 5) * 1000;
    
    mKeepAliveTimer.start([&]() { onKeepAlive (); },
        std::chrono::duration<uint64_t, std::milli>(timeout),
        std::chrono::duration<uint64_t, std::milli>(timeout));
}

