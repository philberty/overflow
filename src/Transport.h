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

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include "ITransportDelegate.h"
#include "RtpPacket.h"
#include "Response.h"
#include "Queue.h"


namespace Overflow
{
    class Transport
    {
    public:
        Transport(ITransportDelegate* const delegate)
            : mDelegate(delegate),
              mState(DISCONNECTED),
              mErrorReason(OK)
        { }

        TransportState getState() const { return mState; }

        bool isRunning() const { return getState() == CONNECTED; }

        virtual void write(const unsigned char *buffer, const size_t length) = 0;

        virtual std::string getTransportHeaderString() const = 0;

        virtual bool connect() = 0;

        virtual void shutdown() = 0;

    protected:
        Response* waitForResponse(int timeout_millis, bool& elapsed)
        {    
            std::int64_t timeout_usecs = timeout_millis * 1000;
            Response* resp = nullptr;
            elapsed = mQueue.wait(resp, timeout_usecs);
            return resp;
        }

        void onStateChange(TransportState state)
        {
            mState = state;
            notifyDelegateStateChange();
        }

        void onRtpPacket(const RtpPacket* packet)
        {
            notifyDelegateOfRtpPacket(packet);
        }

        void onError(TransportErrorReason reason)
        {
            notifyDelegateOfError(reason);
        }
        
    private:
        void notifyDelegateStateChange()
        {
            if (mDelegate != nullptr)
            {
                mDelegate->onStateChange(mState);
            }
        }

        void notifyDelegateOfRtpPacket(const RtpPacket* packet)
        {
            if (mDelegate != nullptr)
            {
                mDelegate->onRtpPacket(packet);
            }   
        }

        void notifyDelegateOfError(TransportErrorReason reason)
        {
            if (mDelegate != nullptr)
            {
                mDelegate->onTransportError(reason);
            }
        }
        
        ITransportDelegate* const mDelegate;
        Queue<Response*> mQueue;
        TransportState mState;
        TransportErrorReason mErrorReason;
    };
};

#endif //__TRANSPORT_H__