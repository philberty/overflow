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

#ifndef __ITRANSPORT_DELEGATE_H__
#define __ITRANSPORT_DELEGATE_H__

#include "RtpPacket.h"


namespace Overflow
{
    typedef enum
    {
        CONNECTED,
        DISCONNECTED,
        CONNECTING,
        ERROR
    } TransportState;

    typedef enum
    {
        SOCKET_ERROR,
        UNKNOWN,
        OK
    } TransportErrorReason;
    
    class ITransportDelegate
    {
    public:
        virtual void onRtpPacket(const RtpPacket* packet) = 0;

        // TODO: not implemented
        // virtual void onRtcpPacket(const RtcpPackate* packet) = 0;

        virtual void onRtspResponse(const Response* response) = 0;

        virtual void onStateChange(TransportState oldState, TransportState newState) = 0;
        
        virtual void onTransportError(TransportErrorReason reason) = 0;

        static std::string stateToString(TransportState state)
        {
            std::string state_string;
            switch (state)
            {
            case CONNECTED:
                state_string = "connected";
                break;
            case DISCONNECTED:
                state_string =  "disconnected";
                break;
            case CONNECTING:
                state_string = "connecting";
                break;
            case ERROR:
                state_string = "error";
                break;
            }
            return state_string;
        }

        static std::string stateToString(TransportErrorReason reason)
        {
            std::string reason_string;
            switch (reason)
            {
            case SOCKET_ERROR:
                reason_string = "socket-error";
                break;
            case UNKNOWN:
                reason_string = "unknown";
                break;
            case OK:
                reason_string = "all-ok";
                break;
            }
            return reason_string;
        }
    };
};

#endif //__ITRANSPORT_DELEGATE_H__
