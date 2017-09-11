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
    : mLoop (false),
      mKeepAliveTimer (mLoop),
      mReconnectTimer (mLoop),
      
      mStopEventLoopHandler([&]() { stopEventLoop(); }),
      mReconnectHandler([&]() { startReconnectTimer(); }),
      mStopTransportHandler([&]() { stopTransport(); }),
      mEventLoopHandler([&]() { eventLoopMain(); }),
      mTransportLoopHandler([&]() { transportLoopMain(); }),
      
      mStopEventLoop(mLoop, mStopEventLoopHandler),
      mReconnect(mLoop, mReconnectHandler),
      mStopTransport(mLoop, mStopTransportHandler),
      
      mIsReconnecting(false),
      mTransport(nullptr),
      mEventLoop(nullptr),
      mTransportLoop(nullptr)
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
        mEventLoop = new std::thread (mEventLoopHandler);
}

void
Overflow::TransportController::stop ()
{
    if (isRunning ())
        mStopEventLoop.send();
}

void
Overflow::TransportController::join ()
{
    if (not isRunning ())
        return;

    mEventLoop->join ();
    
    delete mEventLoop;
    mEventLoop = nullptr;
}

bool
Overflow::TransportController::isRunning () const
{
    return mEventLoop != nullptr and mEventLoop->joinable();
}

bool
Overflow::TransportController::isConnected () const
{
    return mTransport != nullptr and mTransport->getState () == CONNECTED;
}

void
Overflow::TransportController::stopTransport ()
{
    if (mTransport != nullptr)
    {
        mTransport->stop ();        
        mTransportLoop->join ();
        
        delete mTransportLoop;
        delete mTransport;

        mTransport = nullptr;
        mTransportLoop = nullptr;
    }
}

void
Overflow::TransportController::stopTransportAsync ()
{
    mStopTransport.send ();
}

void
Overflow::TransportController::startTransport ()
{
    if (mTransportLoop == nullptr)
        mTransportLoop = new std::thread (mTransportLoopHandler);
}

void
Overflow::TransportController::transportLoopMain ()
{
    auto transportThreadId = std::this_thread::get_id();
    LOG(INFO) << "starting transport on: "<< transportThreadId;
    
    mTransport = createTransport ();
    mTransport->start ();
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

bool
Overflow::TransportController::isReconnecting () const
{
    return mIsReconnecting;
}

std::string
Overflow::TransportController::getTransportHeaderString () const
{
    return mTransport->getTransportHeaderString ();
}

void
Overflow::TransportController::onTransportConnected ()
{
    stopReconnectTimer ();
}

void
Overflow::TransportController::stopReconnectTimer ()
{
    LOG(INFO) << "stopping reconnect timer";
    
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
    auto eventThreadId = std::this_thread::get_id();
    LOG(INFO) << "starting event-loop on: " << eventThreadId;
    mLoop.run ();

    // lbuv task.h make_valgrind_happy
    uv_walk (mLoop.get (), closeWalkCb, NULL);
    uv_run (mLoop.get (), UV_RUN_DEFAULT);
}

void
Overflow::TransportController::stopEventLoop ()
{
    LOG(INFO) << "stopping event-loop";
    
    stopTransport ();
    mReconnectTimer.stop ();
    mKeepAliveTimer.stop ();
    mLoop.stop ();
    
    LOG(INFO) << "stopped event-loop core";   
}
        
void
Overflow::TransportController::sendRtspBytes (const unsigned char* buffer,
                                              size_t length,
                                              int timeout)
{
    mTransport->writeRtsp (buffer, length, timeout);
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

void
Overflow::TransportController::closeWalkCb (uv_handle_t* handle, void* arg)
{
    if (!uv_is_closing (handle))
        uv_close (handle, NULL);
}
