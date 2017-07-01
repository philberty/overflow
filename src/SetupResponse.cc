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

#include "SetupResponse.h"
#include "Helpers.h"

#include <cstdlib>
#include <sstream>
#include <stdexcept>


Overflow::SetupResponse::SetupResponse(const Response* resp)
    : RtspResponse(resp),
      mIsInterleaved(false),
      mRtpInterleavedChannel(0),
      mRtcpInterleavedChannel(1),
      mTimeout(60)
{
    const std::string session_header = headerValueForKey("Session");
    const std::string transport_header = headerValueForKey("Transport");
    
    if (session_header.empty() or transport_header.empty())
    {
        std::ostringstream message;
        message << "Invalid Setup RTSP Response";
        throw std::runtime_error(message.str());
    }
    
    char delim[2];
    delim[0] = ';';
    delim[1] = 0;
    
    std::vector<std::string> values =
        Helper::stringSplit(session_header, std::string((const char *)delim));
    
    if (values.size() > 1)
    {
        // we probably have session;timeout=
        mSession = values[0];
        delim[0] = '=';
        values = Helper::stringSplit(values[1], std::string((const char *)delim));
        
        if (values.size() > 1) {
            mTimeout = atoi(values[1].c_str());
        }
    }
    else
    {
        // simply just session
        mSession = values[0];
    }
    
    delim[0] = ';';
    values = Helper::stringSplit(transport_header, std::string((const char *)delim));
    std::string value = Helper::findKeyAndValuePair(&values, "interleaved");
    
    if (!value.empty()) {
        mIsInterleaved = true;
        
        delim[0] = '=';
        values = Helper::stringSplit(value, std::string((const char *)delim));
        delim[0] = '-';
        values = Helper::stringSplit(values[1], std::string((const char *)delim));
        
        mRtpInterleavedChannel = atoi(values[0].c_str());
        mRtcpInterleavedChannel = atoi(values[1].c_str());
    }
}
