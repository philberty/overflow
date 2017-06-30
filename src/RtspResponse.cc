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

#include "RtspResponse.h"
#include "Helpers.h"

#include <string>
#include <map>
#include <cstdio>
#include <sstream>
#include <stdexcept>


Overflow::RtspResponse::RtspResponse(const unsigned char* buffer,
                                     const size_t length)
    : mCode(500)
{
    // pair is offset and length delimiter inclusive
    std::vector<std::pair<int,int>> lines =
        Helper::splitBuffer(buffer, length, "\r\n");
    
    // if null or empty
    if (lines == nullptr or lines->size() == 0)
    {
        std::ostringstream message;
        message << "Invalid RTSP Response no headers";
        throw runtime_error{message.str()};
    }
    
    const std::pair<int,int>& status_line_offset = lines[0];
    const unsigned char *status_line_buffer = buffer + status_line_offset.first;
    const size_t status_line_length = status_line_offset.second;
    
    std::vector<std::pair<int,int>> status_line_tokens =
        Helper::SplitBufferByDelimiter(
            status_line_buffer, status_line_length, " ");
    
    if (status_line_tokens.size() < 3) {
        ostringstream message;
        message << "Invalid RTSP Response - not enough status tokens";
        throw runtime_error{ message.str() };
    }
    
    std::pair<int,int>& protocol = status_line_tokens[0];
    
    // -1 as this buffer split table includes the delim length
    bool protocol_matches = memcmp(buffer+protocol.first, "RTSP/1.0", protocol.second - 1) == 0;
    if (not protocol_matches) {
        ostringstream message;
        message << "Invalid RTSP Response - invalid protocol for rtsp response";
        throw runtime_error{ message.str() };
    }

    std::pair<int,int>& status_code_token = status_line_tokens[1];

    const char *begin = (const char *)buffer + status_code_token.first;
    m_code = atoi(begin);

    bool is_body = false;
    for (auto it = lines.begin() + 1; it != lines.end(); ++it) {

        const std::pair<int,int>& current_line_token = *it;

        void *line_buffer = alloca(current_line_token.second - 1);
        memset(line_buffer, 0, current_line_token.second - 1);
        memcpy(line_buffer, buffer + current_line_token.first, current_line_token.second - 2);
                
        std::string current_line((const char *)line_buffer);
                
        if (!is_body && current_line.empty()) {
            is_body = true;
            continue;
        }

        if (is_body && !current_line.empty()) {
            m_body += current_line + "\r\n";
        }
        else if (!is_body && !current_line.empty()) {
            size_t pos = current_line.find(':');
            std::string key = current_line.substr(0, pos);
            std::string value = current_line.substr(pos + 2, current_line.length());

            m_headers.insert(std::pair<std::string, std::string>(key, value));
        }
    }
}


Overflow::RtspResponse::RtspResponse(const Response* resp)
    : RtspResponse(resp->BytesPointer(), resp->PointerLength())
{
}

Overflow::RtspResponse::RtspResponse(int code, std::string body)
    : mCode(code),
      mBody(body)
{
}

Overflow::RtspResponse::~RtspResponse()
{
}

const std::string
Overflow::RtspResponse::headerValueForKey(const std::string& key)
{
    std::string header_value;
    
    const std::map<std::string, std::string> headers = GetHeaders();
    auto search = headers.find(key);
    
    if (search != headers.end()) {
        header_value = search->second;
    }
    
    return header_value;
}
