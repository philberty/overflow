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

namespace Overflow
{
    class MJPEGDepacketizer
    {
    public:
        MJPEGDepacketizer(const SessionDescription* palette,
                          const RtpPacket *packet,
                          bool isFirstPayload);

        void addToFrame(std::vector<unsigned char> * const frame);

    private:
        void parseJpegHeader(const unsigned char * buffer);

        void parseRestartMarkerHeader(const unsigned char * buffer);

        void parseQuantizationHeader(const unsigned char * buffer);

        void parseQuantizationTableData(const unsigned char * buffer);

        int mHeight;
        int mWidth;
        int mType;
        int mQValue;
        int mFragmentOffset;
        int mRestartHeaderSize;
        int mDri;
        int mQuantizationPayloadLength;
        unsigned char mLumq[64];
        unsigned char mChrq[64];
        
        const SessionDescription *mPalette;
        const RtpPacket *mPacket;
        std::vector<unsigned char> mPayload;
    };
};

#endif //__MJPEG_DEPACKETIZER_H__
