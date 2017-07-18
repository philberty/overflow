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

#include <cppcodec/base64_default_rfc4648.hpp>

#include "Rtsp.h"
#include "RtspFactory.h"
#include "Url.h"
#include "Helpers.h"


Overflow::RtspFactory::RtspFactory(const std::string& url)
    : mSeqNum(1)
{
    setUrl(url);
}
        
Overflow::Describe*
Overflow::RtspFactory::describeRequest(bool isLive)
{
    Describe *request = new Describe(mPath, mSeqNum++, isLive);
    if (hasAuth()) {
        request->addAuth(mAuth64);
    }
    return request;
}

Overflow::Setup*
Overflow::RtspFactory::setupRequest(const std::string& transport)
{
    Setup *request = new Setup(mPath, mSeqNum++, transport);
    if (hasAuth()) {
        request->addAuth(mAuth64);
    }
    return request;
}

Overflow::Play*
Overflow::RtspFactory::playRequest(const std::string& session)
{
    Play *request = new Play(mPath, mSeqNum++, session);
    if (hasAuth()) {
        request->addAuth(mAuth64);
    }
    return request;
}

Overflow::Options*
Overflow::RtspFactory::optionsRequest()
{
    Options *request = new Options(mPath, mSeqNum++);
    if (hasAuth()) {
        request->addAuth(mAuth64);
    }
    return request;
}

Overflow::Pause*
Overflow::RtspFactory::pauseRequest(const std::string& session)
{
    Pause *request = new Pause(mPath, mSeqNum++, session);
    if (hasAuth()) {
        request->addAuth(mAuth64);
    }
    return request;
}

Overflow::Teardown*
Overflow::RtspFactory::teardownRequest(const std::string& session)
{
    Teardown *request = new Teardown(mPath, mSeqNum++, session);
    if (hasAuth()) {
        request->addAuth(mAuth64);
    }
    return request;
}

void
Overflow::RtspFactory::setPath(std::string& path)
{
    mPath.assign(path);
}

const std::string&
Overflow::RtspFactory::getPath() const
{
    return mPath;
}

void
Overflow::RtspFactory::setUrl(const std::string& url)
{
    Url uri(url, 554);

    int port = uri.getPort();   
    std::string path_buffer = uri.getProtocol() + "://"
        + uri.getHost()
        + ":" + Helper::intToString(port)
        +  uri.getPath();
            
    mPath.assign(path_buffer);

    // ensure auth is updated
    if (not uri.hasAuth())
    {
        mAuth64.assign("");
    }
    else
    {
        const std::string& auth = uri.getAuth();
        mAuth64.assign(base64::encode(auth.c_str(), auth.length()));
    }
}

bool
Overflow::RtspFactory::hasAuth() const
{
    return mAuth64.length() > 0;
}
