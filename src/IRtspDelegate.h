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
        CLIENT_RECEIVED_RESPONSE,
        CLIENT_DISCONNECTED,
        CLIENT_TIMEOUT,
        CLIENT_ERROR
    } RtspClientState;
    
    class IRtspDelegate
    {
    public:
        virtual ~IRtspDelegate() { };
        
        virtual void onRtspClientStateChange(RtspClientState oldState,
                                             RtspClientState newState) = 0;

        // Payload
        virtual void onPayload(const unsigned char * buffer,
                               const size_t length) = 0;

        static std::string stateToString(RtspClientState state)
        {
            std::string state_string;
            switch (state)
            {
            case CLIENT_INITILIZED:
                state_string = "initialized";
                break;
            case CLIENT_CONNECTING:
                state_string = "connecting";
                break;
            case CLIENT_CONNECTED:
                state_string = "connected";
                break;
            case CLIENT_SENDING_OPTIONS:
                state_string = "sending-options-request";
                break;
            case CLIENT_OPTIONS_OK:
                state_string = "options-ok";
                break;
            case CLIENT_SENDING_DESCRIBE:
                state_string = "sending-describe-request";
                break;
            case CLIENT_DESCRIBE_OK:
                state_string = "describe-ok";
                break;
            case CLIENT_SENDING_SETUP:
                state_string = "sending-setup-request";
                break;
            case CLIENT_SETUP_OK:
                state_string = "setup-ok";
                break;
            case CLIENT_SENDING_PLAY:
                state_string = "sending-play-request";
                break;
            case CLIENT_PLAY_OK:
                state_string = "play-ok";
                break;
            case CLIENT_SENDING_PAUSE:
                state_string = "sending-pause-request";
                break;
            case CLIENT_PAUSE_OK:
                state_string = "pause-ok";
                break;
            case CLIENT_SENDING_TEARDOWN:
                state_string = "sending-teardown-request";
                break;
            case CLIENT_RECEIVED_RESPONSE:
                state_string = "received-response";
                break;
            case CLIENT_DISCONNECTED:
                state_string = "disconnected";
                break;
            case CLIENT_TIMEOUT:
                state_string = "timeout";
                break;
            case CLIENT_ERROR:
                state_string = "error";
                break;
            };            
            return state_string;
        }
    };    
};

#endif //__IRTSP_DELEGATE_H__
