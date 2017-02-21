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

#ifndef __MJPEG_DEPACKETIZER_H__
#define __MJPEG_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"

#include <vector>

namespace Overflow {

    class MJPEGDepacketizer {
    public:

        MJPEGDepacketizer(const SessionDescription* palette, const RtpPacket *packet, bool isFirstPayload);

        void AddToFrame(std::vector<unsigned char> * const frame);

    private:

        void ParseJpegHeader(const unsigned char * buffer);

        void ParseRestartMarkerHeader(const unsigned char * buffer);

        void ParseQuantizationHeader(const unsigned char * buffer);

        void ParseQuantizationTableData(const unsigned char * buffer);

        int m_height;
        int m_width;
        int m_type;
        int m_qValue;
        int m_fragmentOffset;
        int m_restartHeaderSize;
        int m_dri;
        int m_quantizationPayloadLength;
        unsigned char m_lumq[64];
        unsigned char m_chrq[64];
        
        const SessionDescription *m_palette;
        const RtpPacket *m_packet;

        std::vector<unsigned char> m_payload;
    };
    
};

#endif //__MJPEG_DEPACKETIZER_H__
