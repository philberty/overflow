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

#ifndef __MP4V_DEPACKETIZER_H__
#define __MP4V_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"

#include <vector>

namespace Overflow {

    class MP4VDepacketizer {
    public:

        MP4VDepacketizer(const SessionDescription* palette,
                         const RtpPacket *packet,
                         bool isFirstPayload);

        const unsigned char *PayloadBytes() const { return &(m_payload[0]); }

        size_t PayloadLength() const { return m_payload.size(); }

    private:

        void PushBytesToCurrentPayload(const unsigned char *payload, size_t length) {
            size_t i;
            for (i = 0; i < length; ++i) {
                m_payload.push_back(payload[i]);
            }
        }
        
        const SessionDescription *m_palette;
        const RtpPacket *m_packet;

        std::vector<unsigned char> m_payload;
    };
    
};

#endif //__MP4V_DEPACKETIZER_H__
