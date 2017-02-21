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

#include <cppcodec/base64_default_rfc4648.hpp>

#include "Describe.h"
#include "Setup.h"
#include "Play.h"
#include "Pause.h"
#include "Options.h"
#include "Teardown.h"

#include "Url.h"
#include "Helpers.h"


namespace Overflow {
    
    class RtspFactory {
    public:
        RtspFactory(const std::string& url)
            : m_seqNum(1)
            {
                SetUrl(url);
            }
        
        Describe* DescribeRequest(bool isLive=true) {
            Describe *request = new Describe(m_path, m_seqNum++, isLive);
            if (hasAuth()) {
                request->AddAuth(m_auth64);
            }
            return request;
        }

        Setup* SetupRequest(const std::string& transport) {
            Setup *request = new Setup(m_path, m_seqNum++, transport);
            if (hasAuth()) {
                request->AddAuth(m_auth64);
            }
            return request;
        }

        Play* PlayRequest(const std::string& session) {
            Play *request = new Play(m_path, m_seqNum++, session);
            if (hasAuth()) {
                request->AddAuth(m_auth64);
            }
            return request;
        }

        Options* OptionsRequest() {
            Options *request = new Options(m_path, m_seqNum++);
            if (hasAuth()) {
                request->AddAuth(m_auth64);
            }
            return request;
        }

        Pause* PauseRequest(const std::string& session) {
            Pause *request = new Pause(m_path, m_seqNum++, session);
            if (hasAuth()) {
                request->AddAuth(m_auth64);
            }
            return request;
        }

        Teardown* TeardownRequest(const std::string& session) {
            Teardown *request = new Teardown(m_path, m_seqNum++, session);
            if (hasAuth()) {
                request->AddAuth(m_auth64);
            }
            return request;
        }

        void SetPath(std::string path) {
            m_path.assign(path);
        }

        const std::string& GetPath() {
            return m_path;
        }

        void SetUrl(const std::string& url) {
            Url uri(url);

            int port = uri.GetPort();

            std::string path_buffer = uri.GetProtocol() + "://"
                + uri.GetHost()
                + ":" + Helper::NumberToString(port) // std::to_string(uri.GetPort())
                +  uri.GetPath();
            
            m_path.assign(path_buffer);
            
            if (uri.HasAuth()) {
                const std::string& auth = uri.GetAuth();
                m_auth64.assign(base64::encode(auth.c_str(), auth.length()));
            }
        }

    private:

        bool hasAuth() const { return m_auth64.length() > 0; }
        
        int m_seqNum;
        std::string m_path;
        std::string m_auth64;
        
    };
};

#endif // __RTSP_FACTORY_H__
