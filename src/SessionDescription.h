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

#ifndef __SESSION_DESCRIPTION_H__
#define __SESSION_DESCRIPTION_H__

#include <string>
#include <vector>


namespace Overflow
{
    typedef enum {
        H264,
        MP4V,
        MJPEG,
        UNKNOWN_PALETTE
    } RtspSessionType;

    class SessionDescription
    {
    public:
        SessionDescription(): mType(UNKNOWN_PALETTE),
                              mFrameRate(-1),
                              mResolutionWidth(-1),
                              mResolutionHeight(-1)
        { }

        SessionDescription(RtspSessionType type,
                           std::string control,
                           std::string rtpMap,
                           std::string fmtp,
                           int frameRate,
                           int width,
                           int height)
            : mType(type),
              mControl(control),
              mRtpMap(rtpMap),
              mFmtp(fmtp),
              mFrameRate(frameRate),
              mResolutionWidth(width),
              mResolutionHeight(height)
        { }

        virtual ~SessionDescription() { }

        RtspSessionType getType() const { return mType; }

        bool isControlUrlComplete() const { return mControl.find("://") != std::string::npos; }

        const std::string getControl() const { return mControl; }

        const std::string getRtpMap() const { return mRtpMap; }

        const std::string getFmtp() const { return mFmtp; }

        const std::string getFmtpH264ConfigParameters() const
        {
            if (mType != RtspSessionType::H264) {
                return std::string();
            }

            size_t pos = mFmtp.find("sprop-parameter-sets=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = mFmtp.find(";", pos);
            return mFmtp.substr(pos + 21, end - pos - 21);
        }

        const std::string getFmtpConfigParameters() const
        {
            size_t pos = mFmtp.find("config=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = mFmtp.find(";", pos);
            if (end == std::string::npos) {
                end = mFmtp.length();
            }

            return mFmtp.substr(pos + 7, end - pos);
        }

        int getFrameRate() const { return mFrameRate; }

        int getResolutionWidth() const { return mResolutionWidth; }

        int getResolutionHeight() const { return mResolutionHeight; }

        static RtspSessionType getTypeFromMime(const std::string& raw_mime)
        {
            if (raw_mime.find("H264") != std::string::npos) {
                return H264;
            }
            else if (raw_mime.find("MP4V-ES") != std::string::npos) {
                return MP4V;
            }
            else if (raw_mime.find("JPEG") != std::string::npos) {
                return MJPEG;
            }
            return UNKNOWN_PALETTE;
        }

    protected:
        RtspSessionType mType;
        std::string mControl;
        std::string mRtpMap;
        std::string mFmtp;
        int mFrameRate;
        int mResolutionWidth;
        int mResolutionHeight;
    };

}

#endif //__SESSION_DESCRIPTION_H__
