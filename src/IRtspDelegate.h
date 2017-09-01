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

#ifndef __IRTSP_DELEGATE_H__
#define __IRTSP_DELEGATE_H__

#include "SessionDescription.h"

#include <string>
#include <cstddef>


namespace Overflow
{
    typedef enum
    {
        CLIENT_INITILIZED,
        CLIENT_CONNECTING,
        CLIENT_CONNECTED,
        CLIENT_SENDING_OPTIONS,
        CLIENT_OPTIONS_OK,
        CLIENT_SENDING_DESCRIBE,
        CLIENT_DESCRIBE_OK,
        CLIENT_SENDING_SETUP,
        CLIENT_SETUP_OK,
        CLIENT_SENDING_PLAY,
        CLIENT_PLAY_OK,
        CLIENT_SENDING_PAUSE,
        CLIENT_PAUSE_OK,
        CLIENT_SENDING_TEARDOWN,
        CLIENT_SENDING_KEEP_ALIVE,
        CLIENT_RECEIVED_RESPONSE,
        CLIENT_DISCONNECTED,
        CLIENT_TIMEOUT,
        CLIENT_ERROR
    } RtspClientState;
    
    class IRtspDelegate
    {
    public:
        virtual ~IRtspDelegate() { }

        virtual void onPaletteType(RtspSessionType type) = 0;
        
        virtual void onRtspClientStateChange(RtspClientState oldState,
                                             RtspClientState newState) = 0;

        virtual void onRtpPacketExtension(int id,
                                          const unsigned char * buffer,
                                          const size_t length) = 0;
        
        virtual void onPayload(const unsigned char * buffer,
                               const size_t length) = 0;

        static std::string stateToString(RtspClientState state)
        {
            switch (state)
            {
            case CLIENT_INITILIZED:
                return "initialized";
            case CLIENT_CONNECTING:
                return "connecting";
            case CLIENT_CONNECTED:
                return "connected";
            case CLIENT_SENDING_OPTIONS:
                return "sending-options-request";
            case CLIENT_OPTIONS_OK:
                return "options-ok";
            case CLIENT_SENDING_DESCRIBE:
                return "sending-describe-request";
            case CLIENT_DESCRIBE_OK:
                return "describe-ok";
            case CLIENT_SENDING_SETUP:
                return "sending-setup-request";
            case CLIENT_SETUP_OK:
                return "setup-ok";
            case CLIENT_SENDING_PLAY:
                return "sending-play-request";
            case CLIENT_PLAY_OK:
                return "play-ok";
            case CLIENT_SENDING_PAUSE:
                return "sending-pause-request";
            case CLIENT_PAUSE_OK:
                return "pause-ok";
            case CLIENT_SENDING_TEARDOWN:
                return "sending-teardown-request";
            case CLIENT_SENDING_KEEP_ALIVE:
                return "sending-keep-alive";
            case CLIENT_RECEIVED_RESPONSE:
                return "received-response";
            case CLIENT_DISCONNECTED:
                return "disconnected";
            case CLIENT_TIMEOUT:
                return "timeout";
            case CLIENT_ERROR:
                return "error";
            };
            
            return "unknown";
        }
    };    
};

#endif //__IRTSP_DELEGATE_H__
