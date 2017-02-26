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


namespace Overflow {

    typedef enum {

        H264,
        MP4V,
        MJPEG,

        UNKNOWN

    } RtspSessionType;

    class SessionDescription {
    public:

        SessionDescription(): m_type(UNKNOWN),
                              m_control(""),
                              m_rtpMap(""),
                              m_fmtp(""),
                              m_frameRate(-1),
                              m_resolutionWidth(-1),
                              m_resolutionHeight(-1) { }

        SessionDescription(RtspSessionType type,
                           std::string control,
                           std::string rtpMap,
                           std::string fmtp,
                           int frameRate,
                           int width,
                           int height): m_type(type),
                                        m_control(control),
                                        m_rtpMap(rtpMap),
                                        m_fmtp(fmtp),
                                        m_frameRate(frameRate),
                                        m_resolutionWidth(width),
                                        m_resolutionHeight(height) { }

        virtual ~SessionDescription() { }

        RtspSessionType GetType() const { return m_type; }

        bool IsControlUrlComplete() const { return m_control.find("://") != std::string::npos; }

        const std::string GetControl() const { return m_control; }

        const std::string GetRtpMap() const { return m_rtpMap; }

        const std::string GetFmtp() const { return m_fmtp; }

        const std::string GetFmtpH264ConfigParameters() const {
            if (m_type != RtspSessionType::H264) {
                return std::string();
            }

            size_t pos = m_fmtp.find("sprop-parameter-sets=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = m_fmtp.find(";", pos);
            return m_fmtp.substr(pos + 21, end - pos - 21);
        }

        const std::string GetFmtpConfigParameters() const {
            size_t pos = m_fmtp.find("config=");
            if (pos == std::string::npos) {
                return std::string();
            }

            size_t end = m_fmtp.find(";", pos);
            if (end == std::string::npos) {
                end = m_fmtp.length();
            }

            return m_fmtp.substr(pos + 7, end - pos);
        }

        int GetFrameRate() const { return m_frameRate; }

        int GetResolutionWidth() const { return m_resolutionWidth; }

        int GetResolutionHeight() const { return m_resolutionHeight; }

        SessionDescription* DeepCopy() const {
            return new SessionDescription(m_type,
                                          m_control,
                                          m_rtpMap,
                                          m_fmtp,
                                          m_frameRate,
                                          m_resolutionWidth,
                                          m_resolutionHeight);
        }

        static RtspSessionType GetTypeFromMime(const std::string& raw_mime) {
            if (raw_mime.find("H264") != std::string::npos) {
                return H264;
            }
            else if (raw_mime.find("MP4V-ES") != std::string::npos) {
                return MP4V;
            }
            else if (raw_mime.find("JPEG") != std::string::npos) {
                return MJPEG;
            }
            return UNKNOWN;
        }

    protected:
        RtspSessionType m_type;
        std::string m_control;
        std::string m_rtpMap;
        std::string m_fmtp;
        int m_frameRate;
        int m_resolutionWidth;
        int m_resolutionHeight;
    };

}

#endif //__SESSION_DESCRIPTION_H__
