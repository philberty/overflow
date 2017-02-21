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

#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <string>
#include <cstdlib>
#include <cstring>


namespace Overflow {

    class Response {
    public:
        Response(const unsigned char *buffer, size_t length): m_length(length)
        {
            unsigned char *copy = (unsigned char*)malloc(length);
            memcpy((void*)copy, (const void *)buffer, length);
            m_buffer = copy;
        }

        Response(const Response& other)
        {
            const unsigned char* buffer = other.BytesPointer();
            size_t length = other.PointerLength();
            m_length = length;

            unsigned char *copy = (unsigned char*)malloc(length);
            memcpy((void*)copy, (const void *)buffer, length);
            m_buffer = copy;
        }

        ~Response()
        {
            free(m_buffer);
        }

        const unsigned char *BytesPointer() const { return m_buffer; }

        const size_t PointerLength() const { return m_length; }

        bool IsInterleavedPacket() const { return m_buffer[0] == '$'; }

        int GetInterleavedPacketChannelNumber() {
            if (!IsInterleavedPacket()) {
                return -1;
            }
            unsigned char channel_number = m_buffer[1];
            return static_cast<int>(channel_number);
        }

        std::string GetStringBuffer() const {
            std::string buf;

            const unsigned char * buffer = BytesPointer();
            for (size_t i = 0; i < PointerLength(); ++i) {
                buf += buffer[i];
            }
            
            return buf;
        }

    private:
        unsigned char *m_buffer;
        size_t m_length;
    };
};

#endif //__RESPONSE_H__
