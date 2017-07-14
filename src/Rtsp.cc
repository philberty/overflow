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

#include "Rtsp.h"
#include "Helpers.h"


Overflow::Rtsp::Rtsp(const std::string& method,
                     const std::string& path,
                     int seqNum)
    : mMethod(method),
      mPath(path)
{
    addHeader("CSeq", Helper::intToString(seqNum));
}

const Overflow::ByteBuffer&
Overflow::Rtsp::getBuffer()
{
    mBuffer.reset();
    
    mBuffer.append(mMethod + " " + mPath + " RTSP/1.0\r\n");
    for (auto it = mHeaders.begin(); it != mHeaders.end(); ++it)
    {
        mBuffer.append(it->first + ": " + it->second + "\r\n");
    }
    
    mBuffer.append("\r\n\r\n");
    
    // TODO:
    // add content
    
    return mBuffer;
}

void
Overflow::Rtsp::addAuth(const std::string& encoded)
{
    addHeader("Authorization", "Basic " + encoded);
}

const std::string&
Overflow::Rtsp::getMethod() const
{
    return mMethod;
}

std::string
Overflow::Rtsp::toString()
{
    std::string buf;
    buf += mMethod + ":" + mPath;
    return buf;    
}

void
Overflow::Rtsp::addHeader(const std::string& key,
                          const std::string& value)
{
    mHeaders.insert(std::pair<std::string,std::string>(key, value));
}
