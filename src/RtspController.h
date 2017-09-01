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

#ifndef __RTSP_CONTROLLER_H__
#define __RTSP_CONTROLLER_H__

#include "TransportController.h"
#include "ITransportDelegate.h"
#include "Transport.h"
#include "IRtspDelegate.h"
#include "RtspFactory.h"
#include "SessionDescription.h"

#include <string>


namespace Overflow
{
    class RtspController: public TransportController,
                          protected ITransportDelegate
    {
    public:
        RtspController (IRtspDelegate* delegate, std::string url);

        virtual ~RtspController ();

        void sendPlayRequest ();

        void sendPauseRequest ();

        void standby ();

    protected:
        void onKeepAlive () override;

        virtual Transport* createTransport () { return nullptr; }
        
    private:
        // ITransportDelegate
        void onRtpPacket (const RtpPacket* packet) override;

        // void onRtcpPacket(const RtcpPackat* packet) = 0;

        void onRtspResponse(const Response* response) override;

        void onStateChange(TransportState oldState, TransportState newState) override;
        
        void onTransportError(TransportErrorReason reason) override;
        // ITransportDelegate
        
        void onStateChange(RtspClientState state);

        void notifyDelegateOfPayload();

        void notifyDelegateOfExtension(const RtpPacket* packet);

        void notifyDelegateOfPaletteType();

        void processH264Packet(const RtpPacket* packet);

        void processMP4VPacket(const RtpPacket* packet);

        void processMJPEGPacket(const RtpPacket* packet);

        void resetCurrentPayload ();

        void resetClientState ();

        bool haveSession () const;

        size_t getCurrentFrameSize() const;

        const unsigned char* getCurrentFrame() const;

        void appendPayloadToCurrentFrame(const unsigned char* buffer, size_t length);

        void notifyDelegateOfStateChange (RtspClientState oldState,
                                          RtspClientState newState);

        void sendOptionsRequest ();

        void onOptionsResponse (const Response* response);

        void sendDescribeRequest ();

        void onDescribeResponse (const Response* response);

        void sendSetupRequest ();

        void onSetupResponse (const Response* response);

        void onPlayResponse (const Response* response);

        void onPauseResponse (const Response* response);

        void sendTeardownRequest ();

        void sendKeepAlive ();

        void sendRtsp (Rtsp* request, bool timeout=true);

        // fields
        IRtspDelegate* mDelegate;
        std::string mUrl;
        RtspFactory mFactory;
        RtspClientState mState;
        SessionDescription mPalette;
        bool mServerAllowsAggregate;
        std::string mSession;
        int mLastSeqNum;
        bool mIsFirstPayload;
        std::vector<unsigned char> mCurrentFrame;
    };
};

#endif //__RTSP_CONTROLLER_H__
