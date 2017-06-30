// -*-c++-*-
// Copyright (c) 2016 Philip Herron
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

#ifndef __RTP_PACKET_H__
#define __RTP_PACKET_H__

#include "Response.h"

#include <cstdlib>


namespace Overflow {

    class RtpPacket {
    public:        
        RtpPacket(const unsigned char *buffer, uint16_t length);

        RtpPacket(const Response* response);

        ~RtpPacket();

        int getVersion() const { return mVersion; }

        int getSequenceNumber() const { return mSequenceNumber; }

        int getType() const { return mType; }

        bool isMarked() const { return mMarker; }

        bool hasExtension() const { return mExtension != nullptr; }

        int getExtensionID() const { return mExtensionID; }

        const unsigned char * getExtensionData() const { return mExtension; }

        size_t getExtensionLength() const { return mExtensionLength; }

        bool hasPayload() const { return mPayload != nullptr; }

        const unsigned char * payloadData() const { return mPayload; }

        size_t payloadLength() const { return mPayloadLength; }

        uint32_t getTimestamp() const { return mTimestmap; }

    private:
        uint32_t mTimestmap;
        int mVersion;
        int mSequenceNumber;
        int mType;
        bool mMarker;
        int mExtensionID;
        unsigned char *mExtension;
        size_t mExtensionLength;
        unsigned char *mPayload;
        size_t mPayloadLength;
    };
    
};

#endif // __RTP_PACKET_H__
