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


#include "ByteBuffer.h"

#include <cstring>
#include <cstdlib>


Overflow::ByteBuffer::ByteBuffer()
    : mBytes(nullptr),
      mLength(0)
{ }

Overflow::ByteBuffer::ByteBuffer(const std::string& buffer)
    : mBytes(nullptr),
      mLength(0)
{
    append(buffer);
}

Overflow::ByteBuffer::~ByteBuffer()
{
    if (m_bytes != nullptr) {
        free(mBytes);
    }
}

void
Overflow::ByteBuffer::append(const unsigned char *bytes,
                             const size_t length)
{
    if (mBytes == nullptr)
    {
        mBytes = (unsigned char*)malloc(length);
    }
    else
    {
        unsigned char * buffer = (unsigned char*)malloc(length + m_length);
        memcpy(buffer, mBytes, mLength);
        
        free(mBytes);
        mBytes = buffer;
    }
    
    memcpy(mBytes+mLength, bytes, length);
    mLength += length;
}

void
Overflow::ByteBuffer::append(const std::string& bytes)
{
    append((const unsigned char *)bytes.c_str(),
           bytes.length());
}

void
Overflow::ByteBuffer::insert(const unsigned char *bytes,
                             const size_t length,
                             const size_t offs)
{
    if (mBytes == nullptr)
    {
        append(bytes, length);
    }
    else if (mLength - offs >= length)
    {
        memcpy(mBytes+offs, bytes, length);
    }
    else
    {
        unsigned char *buffer = (unsigned char *)malloc(offs + length);
        memcpy(buffer, m_bytes, offs);
        memcpy(buffer+offs, bytes, length);
        
        free(mBytes);
        mBytes = buffer;
        mLength = offs + length;
    }
}

void
Overflow::ByteBuffer::insert(const std::string& bytes,
                             const size_t offs)
{
    insert((const unsigned char *)bytes.c_str(),
           bytes.length(),
           offs);
}

const unsigned char*
Overflow::ByteBuffer::getBytesPointer()
{
    return mBytes;
}

const size_t
Overflow::ByteBuffer::length()
{
    return mLength;
}

void
Overflow::ByteBuffer::reset()
{
    if (m_bytes != nullptr) {
        free(mBytes);
    }
    mBytes = nullptr;
    mLength = 0;
}
