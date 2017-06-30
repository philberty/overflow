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

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <string>
#include <vector>
#include <utility>


namespace Overflow
{
    class Helper
    {
    public:
        static std::string intToString(int number);

        
        static std::string findKeyAndValuePair(const std::vector<std::string>* values,
                                               const std::string& key);
        
        static std::vector<std::string> stringSplit(const std::string& data,
                                                    std::string token);
        
        static std::vector<std::pair<int, int>> splitBuffer(const unsigned char * buffer,
                                                            const size_t length,
                                                            const std::string& delim);
    };
};

#endif //__HELPERS_H__
