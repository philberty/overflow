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

#ifndef __BYTE_BUFFER_H__
#define __BYTE_BUFFER_H__

#include <cstring>
#include <cstdlib>


namespace Overflow {

    class ByteBuffer {
    public:

        ByteBuffer(): m_bytes(nullptr), m_length(0)  { }

        ByteBuffer(const std::string& buffer): m_bytes(nullptr), m_length(0)  {
            Append(buffer);
        }

        ByteBuffer(ByteBuffer& buffer): m_bytes(nullptr), m_length(0)  {
            Append(buffer.BytesPointer(), buffer.Length());
        }

        ~ByteBuffer() {
            if (m_bytes != nullptr) {
                free(m_bytes);
            }
        }

        void Append(const unsigned char *bytes, const size_t length) {
            if (m_bytes == nullptr) {
                m_bytes = (unsigned char*)malloc(length);
            } else {
                unsigned char * buffer = (unsigned char*)malloc(length + m_length);
                memcpy(buffer, m_bytes, m_length);

                free(m_bytes);
                m_bytes = buffer;
            }

            memcpy(m_bytes+m_length, bytes, length);
            m_length += length;
        }

        void Append(const std::string& bytes) {
            Append((const unsigned char *)bytes.c_str(), bytes.length());
        }

        void Insert(const unsigned char *bytes, const size_t length, const size_t offs) {
            if (m_bytes == nullptr) {
                Append(bytes, length);
            } else if (m_length - offs >= length) {
                memcpy(m_bytes+offs, bytes, length);
            } else {
                unsigned char *buffer = (unsigned char *)malloc(offs + length);
                memcpy(buffer, m_bytes, offs);
                memcpy(buffer+offs, bytes, length);

                free(m_bytes);
                m_bytes = buffer;
                m_length = offs + length;
            }
        }

        void Insert(const std::string& bytes, const size_t offs) {
            Insert((const unsigned char *)bytes.c_str(), bytes.length(), offs);
        }

        const unsigned char * BytesPointer() const {
            return m_bytes;
        }

        const size_t Length() const {
            return m_length;
        }

        void GetBytes(unsigned char ** const buffer, size_t * const length) const {
            *buffer = (unsigned char *)malloc(m_length);
            memcpy(*buffer, m_bytes, m_length);
            *length = m_length;
        }

        void Reset() {
            if (m_bytes != nullptr) {
                delete m_bytes;
            }
            m_bytes = nullptr;
            m_length = 0;
        }

    private:
        unsigned char *m_bytes;
        size_t m_length;
    };
    
};

#endif //__BYTE_BUFFER_H__
