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

#include <string>


namespace Overflow
{
    class ByteBuffer
    {
    public:
        ByteBuffer();

        ByteBuffer(const std::string& buffer);

        ~ByteBuffer();

        void append(const unsigned char *bytes,
                    const size_t length);
        
        void append(const std::string& bytes);
            
        void insert(const unsigned char *bytes,
                    const size_t length,
                    const size_t offs);

        void insert(const std::string& bytes,
                    const size_t offs);

        const unsigned char * getBytesPointer() const;

        const size_t length() const;

        void reset();

    private:
        unsigned char *m_bytes;
        size_t m_length;
    };
    
};

#endif //__BYTE_BUFFER_H__
