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

#ifndef __OVERFLOW_URL_H__
#define __OVERFLOW_URL_H__

#include <string>
#include <map>


namespace Overflow
{
    class Url
    {
    public:
        Url(const std::string& raw, int default_port);

        int getPort() const { return m_port; }

        const std::string& getPath() const { return m_path; }

        const std::string& getHost() const { return m_host; }

        const std::string& getProtocol() const { return m_protocol; }

        const bool hasAuth() const { return !m_auth.empty(); }

        const std::string& getAuth() const { return m_auth; }

        const std::map<std::string, std::string>& getQueries() const { return m_queries; }

    private:
        int m_port;
        std::string m_host;
        std::string m_path;
        std::string m_auth;
        std::string m_protocol;
        std::map<std::string,std::string> m_queries;
    };
    
};

#endif //__OVERFLOW_URL_H__
