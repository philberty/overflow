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

#include "Response.h"

#include <cstdlib>
#include <cstring>


Overflow::Response::Response(const unsigned char *buffer,
                             size_t length)
    : mLength(length)
{
    unsigned char *copy = (unsigned char*)malloc(length);
    memcpy((void*)copy, (const void *)buffer, length);
    mBuffer = copy;
}

Overflow::Response::~Response()
{
    if (mBuffer != NULL)
        free(mBuffer);
}

bool
Overflow::Response::isInterleavedPacket() const
{
    if (mLength > 0) {
        return mBuffer[0] == '$';
    }
    
    return false;
}

const unsigned char*
Overflow::Response::bytesPointer() const
{
    return mBuffer;
}

const size_t
Overflow::Response::length() const
{
    return mLength;
}

int
Overflow::Response::getInterleavedPacketChannelNumber() const
{
    if (not isInterleavedPacket()) {
        return -1;
    }
    
    unsigned char channel_number = mBuffer[1];
    return static_cast<int>(channel_number);
}

std::string
Overflow::Response::getStringBuffer() const
{
    std::string buf;
    
    const unsigned char * buffer = bytesPointer();
    for (size_t i = 0; i < length(); ++i) {
        buf += buffer[i];
    }
            
    return buf;
}
