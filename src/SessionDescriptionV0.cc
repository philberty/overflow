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

#include "SessionDescriptionV0.h"
#include "Helpers.h"


Overflow::SessionDescriptionV0::SessionDescriptionV0(const std::string& rawSessionDescription)
    : SessionDescription()
{
    std::vector<std::string> lines = Helper::stringSplit(rawSessionDescription, "\r\n");
    
    for (auto it = lines.begin(); it != lines.end(); ++it)
    {
        const std::string current_line = *it;
        
        bool is_rtpmap = current_line.find("a=rtpmap") != std::string::npos;
        bool is_fmtp = current_line.find("a=fmtp") != std::string::npos;
        bool is_control = current_line.find("a=control") != std::string::npos;
        bool is_mime = is_rtpmap;
        
        if (is_rtpmap)
        {
            mRtpMap = current_line;
        }
        
        if (is_fmtp)
        {
            mFmtp = current_line;
        }
        
        if (is_control)
        {
            size_t end_position = current_line.find(';');
            mControl = current_line.substr(10, end_position - 10);
        }
        
        if (is_mime)
        {
            mType = getTypeFromMime(current_line);
        }
    }
}
