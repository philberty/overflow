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

#include <cstdlib>

#include "Response.h"


namespace Overflow {

    class RtpPacket {
    public:        
        RtpPacket(const unsigned char *buffer, uint16_t length);

        RtpPacket(const Response* response)
            : RtpPacket(response->BytesPointer(), response->PointerLength())
            { }

        ~RtpPacket() {
            if (HasExtension()) {
                free(m_extension);
            }
            free(m_payload);
        }

        int GetVersion() const { return m_version; }

        int GetSequenceNumber() const { return m_sequenceNumber; }

        int GetType() const { return m_type; }

        bool IsMarked() const { return m_marker; }

        bool HasExtension() const { return m_extension != nullptr; }

        int GetExtensionID() const { return m_extensionID; }

        const unsigned char * GetExtensionData() const { return m_extension; }

        size_t GetExtensionLength() const { return m_extensionLength; }

        bool HasPayload() const { return m_payload != nullptr; }

        const unsigned char * PayloadData() const { return m_payload; }

        size_t PayloadLength() const { return m_payloadLength; }

        uint32_t GetTimestamp() const { return m_timestmap; }

    private:
        uint32_t m_timestmap;
        int m_version;
        int m_sequenceNumber;
        int m_type;
        bool m_marker;
        int m_extensionID;
        unsigned char *m_extension;
        size_t m_extensionLength;
        unsigned char *m_payload;
        size_t m_payloadLength;
    };
    
};

#endif // __RTP_PACKET_H__
