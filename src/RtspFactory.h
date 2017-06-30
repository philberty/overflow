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

#ifndef __RTSP_FACTORY_H__
#define __RTSP_FACTORY_H__

#include "Describe.h"
#include "Setup.h"
#include "Play.h"
#include "Pause.h"
#include "Options.h"
#include "Teardown.h"


namespace Overflow
{
    class RtspFactory
    {
    public:
        RtspFactory(const std::string& url);
        
        Describe* describeRequest(bool isLive);

        Setup* setupRequest(const std::string& transport);

        Play* playRequest(const std::string& session);

        Options* optionsRequest();

        Pause* pauseRequest(const std::string& session);

        Teardown* teardownRequest(const std::string& session);

        void setPath(std::string& path);

        const std::string& getPath();

        void setUrl(const std::string& url);

        bool hasAuth() const;

    private:
        int mSeqNum;
        std::string mPath;
        std::string mAuth64;
        
    };
};

#endif // __RTSP_FACTORY_H__
