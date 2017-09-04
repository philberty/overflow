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

#ifndef __RTSP_CLIENT_CONTROLLER_HPP__
#define __RTSP_CLIENT_CONTROLLER_HPP__

#include "RtspController.h"
#include "IRtspDelegate.h"
#include "Transport.h"

#include <uvpp/loop.hpp>
#include <uvpp/timer.hpp>
#include <uvpp/async.hpp>

#include <string>
#include <thread>
#include <mutex>


namespace Overflow
{
    template<class T>
    class RtspClientController : public RtspController
    {
        static_assert (std::is_base_of<Transport, T>::value, "T must derive from Transport");
        
    public:
        RtspClientController(IRtspDelegate* delegate,
                             std::string url)
            : RtspController (delegate,
                              url,
                              [&](const unsigned char* buf, size_t length, bool timeout) {
                                  mTransport.writeRtsp (buf, length, timeout);
                              },
                              [&](int seconds) {
                                  startKeepAliveTimer (seconds);
                              },
                              [&]() {
                                  return mTransport.getTransportHeaderString ();
                              },
                              [&]() {
                                  startReconnect ();
                              }),
              mLoop (),
              mKeepAliveTimer (mLoop),
              mReconnectTimer (mLoop),
              mTransport (mLoop, this, url),
              mEventLoop (nullptr)
        { }

        virtual ~RtspClientController ()
        {
            stop ();
            join ();
        }

        void start () {
            startTransport ();
    
            if (not isRunning ())
                mEventLoop = new std::thread ([&]() {
                        auto eventThreadId = std::this_thread::get_id ();
                        LOG(INFO) << "starting event-loop on: " << eventThreadId;
                        mLoop.run ();
                    });
        }

        void stop () {
            if (isRunning ())
            {
                uvpp::Async stopAsync (mLoop, [&]() {
                        LOG(INFO) << "stopping event-loop";
                        
                        stopTransport ();
                        stopKeepAliveTimer ();
                        mLoop.stop ();
                        
                        LOG(INFO) << "stopped event-loop core";   
                    });

                stopAsync.send ();
            }
        }

        void join () {
            if (not isRunning ())
                return;
            
            mEventLoop->join ();
            delete mEventLoop;
            mEventLoop = nullptr;
        }

        bool isRunning () const {
            return mEventLoop != nullptr and mEventLoop->joinable ();
        }

        bool isConnected () const {
            return mTransport.getState () == CONNECTED;
        }

    protected:

        void onTransportConnected () override {
            LOG(INFO) << "onTransportConnected!";
        }

    private:

        void startTransport () {
            mTransport.start ();
        }
        
        void stopTransport () {
            mTransport.stop ();
        }
        
        void startReconnect () {
            LOG(INFO) << "starting-reconnect";
            
            startTransport ();
        }

        void startKeepAliveTimer (int seconds) {
            // trim a few seconds to ensure keep-alive is sent in time
            uint64_t timeout = (seconds - 5) * 1000;
    
            mKeepAliveTimer.start([&]() { onKeepAlive (); },
                                  std::chrono::duration<uint64_t, std::milli>(timeout),
                                  std::chrono::duration<uint64_t, std::milli>(timeout));
        }

        void stopKeepAliveTimer () {
            mKeepAliveTimer.stop ();
        }
        
        uvpp::loop mLoop;
        uvpp::Timer mKeepAliveTimer;
        uvpp::Timer mReconnectTimer;
        T mTransport;
        
        bool mIsReconnecting;
        std::thread* mEventLoop;
        
    };
};

#endif //__RTSP_CLIENT_CONTROLLER_HPP__
