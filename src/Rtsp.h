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


#ifndef __RTSP_H__
#define __RTSP_H__

#include <string>
#include <map>

#include "ByteBuffer.h"
#include "Helpers.h"


namespace Overflow {

    class Rtsp {
    public:
        Rtsp(const std::string& method, const std::string& path, int seqNum)
            : m_headers(), m_method(method), m_path(path), m_buffer()
        {
            AddHeader("CSeq", Helper::NumberToString(seqNum)/* std::to_string(seqNum) */);
        }

        const ByteBuffer& GetBuffer()
        {
            m_buffer.Reset();
            
            m_buffer.Append(m_method + " " + m_path + " RTSP/1.0\r\n");
            for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
                m_buffer.Append(it->first + ": " + it->second + "\r\n");
            }
            m_buffer.Append("\r\n\r\n");

            // TODO:
            // add content
            
            return m_buffer;
        }

        void AddAuth(const std::string& encoded) {
            AddHeader("Authorization", "Basic " + encoded);
        }

        const std::string& GetMethod() const { return m_method; }

        std::string ToString() {
            std::string buf;
            buf += m_method + ":" + m_path;
            return buf;
            
        }

    protected:
        void AddHeader(const std::string& key, const std::string& value) {
            m_headers.insert(std::pair<std::string,std::string>(key, value));
        }

    private:
        std::map<std::string, std::string> m_headers;
        std::string m_method;
        std::string m_path;

        ByteBuffer m_buffer;
    };
    
};

#endif //__RTSP_H__
