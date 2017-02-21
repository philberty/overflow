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

#ifndef __DESCRIBE_RESPONSE_H__
#define __DESCRIBE_RESPONSE_H__

#include "Response.h"
#include "RtspResponse.h"
#include "SessionDescriptionFactory.h"

#include <vector>

namespace Overflow {

    class DescribeResponse: public RtspResponse {
    public:
        DescribeResponse(const Response* resp)
            : RtspResponse(resp),
              sessionDescriptions(SessionDescriptionFactory::ParseSessionDescriptionsFromBuffer(GetBodyString()))
            { }

        const std::vector<SessionDescription>& GetSessionDescriptions() const { return sessionDescriptions; }

        const bool Ok() const override { return GetCode() == 200 && sessionDescriptions.size() > 0; }

    private:
        std::vector<SessionDescription> sessionDescriptions;
    };

};

#endif //__DESCRIBE_RESPONSE_H__
