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

#include <stdexcept>
#include <cstdlib>

#include "Helpers.h"

using std::runtime_error;


namespace Overflow {

    class Url {
    public:
        Url(const std::string& raw, int default_port=80) {
            std::size_t protocol_index = raw.find("://");
            if (protocol_index == std::string::npos) {
                throw runtime_error{"Failed to parse url"};
            }

            // scheme
            m_protocol.assign(raw.substr(0, protocol_index));

            // offset
            std::size_t offs = protocol_index + 3;

            // auth
            std::size_t auth_end_index = raw.find("@", offs);
            if (auth_end_index != std::string::npos) {
                std::size_t auth_len = auth_end_index - offs;
                m_auth.assign(raw.substr(offs, auth_len));
                offs += auth_len + 1;
            }

            // host
            std::size_t host_end_index = raw.find("/", offs);
            std::size_t port_index = raw.find(":", offs);
            bool has_port = port_index != std::string::npos;

            if (has_port) {
                
                std::size_t host_len = port_index - offs;
                m_host.assign(raw.substr(offs, host_len));

                std::string str_port;
                if (host_end_index == std::string::npos) {
                    std::size_t port_len = raw.length() - port_index;
                    str_port.assign(raw.substr(port_index + 1, port_len));
                } else {
                    std::size_t port_len = host_end_index - port_index;
                    str_port.assign(raw.substr(port_index + 1, port_len));
                }

                const char *c_mport = str_port.c_str();
                m_port = strtol(c_mport, nullptr, 10); // std::stoi(str_port);
            } else {
                m_port = default_port;
                
                if (host_end_index == std::string::npos) {
                    m_host.assign(std::string(raw.c_str() + offs));
                }
                else {
                    std::size_t host_len = host_end_index - offs;
                    m_host.assign(raw.substr(offs, host_len));
                }
            }

            if (host_end_index == std::string::npos) {
                m_path.assign("/");
            } else {
                m_path.assign(raw.substr(host_end_index, raw.length()));
            }
        }

        int GetPort() const { return m_port; }

        const std::string& GetPath() const { return m_path; }

        const std::string& GetHost() const { return m_host; }

        const std::string& GetProtocol() const { return m_protocol; }

        const bool HasAuth() const { return !m_auth.empty(); }

        const std::string& GetAuth() const { return m_auth; }

        const std::map<std::string, std::string>& GetQueries() const { return m_queries; }

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
