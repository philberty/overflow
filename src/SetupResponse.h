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

#ifndef __SETUP_RESPONSE_H__
#define __SETUP_RESPONSE_H__

#include "RtspResponse.h"
#include "Helpers.h"

#include <cstdlib>

namespace Overflow {
    
    class SetupResponse: public RtspResponse {
    public:
        SetupResponse(const Response *resp): RtspResponse(resp),
                                             m_isInterleaved(false),
                                             m_rtpInterleavedChannel(0),
                                             m_rtcpInterleavedChannel(1),
                                             m_session(),
                                             m_timeout(60)
        {
            const std::string session_header = HeaderValueForKey("Session");
            const std::string transport_header = HeaderValueForKey("Transport");

            if (session_header.empty() || transport_header.empty()) {
                ostringstream message;
                message << "Invalid Setup RTSP Response";
                throw runtime_error{ message.str() };
            }

            char delim[2];
            delim[0] = ';';
            delim[1] = 0;
            
            std::vector<std::string> values = Helper::StringSplit(session_header, std::string((const char *)delim));
            if (values.size() > 1) {
                // we probably have session;timeout=
                m_session = values[0];
                delim[0] = '=';
                values = Helper::StringSplit(values[1], std::string((const char *)delim));

                if (values.size() > 1) {
                    m_timeout = atoi(values[1].c_str());
                }
            } else {
                // simply just session
                m_session = values[0];
            }

            delim[0] = ';';
            values = Helper::StringSplit(transport_header, std::string((const char *)delim));
            std::string value = Helper::FindKeyAndValuePair(&values, "interleaved");
            
            if (!value.empty()) {
                m_isInterleaved = true;

                delim[0] = '=';
                values = Helper::StringSplit(value, std::string((const char *)delim));
                delim[0] = '-';
                values = Helper::StringSplit(values[1], std::string((const char *)delim));

                m_rtpInterleavedChannel = atoi(values[0].c_str());
                m_rtcpInterleavedChannel = atoi(values[1].c_str());
            }
        }
        
        int GetRtpInterleavedChannel() const { return m_rtpInterleavedChannel; }
        
        int GetRtcpInterleavedChannel() const { return m_rtcpInterleavedChannel; }
        
        const std::string GetSession() const { return m_session; }
        
        int GetTimeoutSeconds() const { return m_timeout; }
        
        bool IsInterleaved() const { return m_isInterleaved; }
        
    private:
        
        bool m_isInterleaved;
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;
        std::string m_session;
        int m_timeout;
    };

};
#endif //__SETUP_RESPONSE_H__
