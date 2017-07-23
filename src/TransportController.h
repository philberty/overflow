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

#ifndef __TRANSPORT_CONTROLLER_H__
#define __TRANSPORT_CONTROLLER_H__

#include "IRtspDelegate.h"
#include "ITransportDelegate.h"
#include "Transport.h"
#include "InterleavedTcpTransport.h"
#include "RtspFactory.h"
#include "SessionDescription.h"


#include <string>
#include <thread>
#include <mutex>


namespace Overflow
{
    class TransportController
    {
    public:
        TransportController ();

        virtual ~TransportController ();

        void start ();

        void stop ();

        void join ();

        void standby ();

        bool isRunning () const;

        bool isReconnecting () const;
        
    protected:
        void stopTransport ();

        void startTransport ();

        void startReconnectTimer ();
        
        void eventLoopMain ();
        
        void stopEventLoop ();
        
        void sendRtsp (Rtsp* request);
        
        void startKeepAliveTimer (int seconds);

        void onTransportConnected ();

        void reconnect ();

        virtual void onKeepAlive() = 0;

        virtual Transport* createTransport () = 0;

    private:
        uvpp::loop mLoop;
        uvpp::Timer mKeepAliveTimer;
        uvpp::Timer mReconnectTimer;
        uvpp::Work mWorkers;

        std::function<void ()> mStopEventLoopHandler;
        std::function<void ()> mReconnectHandler;
        std::function<void ()> mStopTransportHandler;
        
        uvpp::Async mStopEventLoop;
        uvpp::Async mReconnect;
        uvpp::Async mStopTransport;

        bool mIsReconnecting;
        Transport* mTransport;
        std::mutex mMutex;
        std::thread* mEventLoop;
    };
};

#endif //__TRANSPORT_CONTROLLER_H__
