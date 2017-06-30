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

#include "Helpers.h"

#include <cstring>
#include <sstream>


std::string
Overflow::Helper::intToString(int number)
{
    std::ostringstream ss;
    ss << number;
    return ss.str();
}

std::string
Overflow::Helper::findKeyAndValuePair(const std::vector<std::string>* values,
                                      const std::string& key)
{
    for (auto it = values->begin(); it != values->end(); ++it) {
        const std::string& value = *it;
        
        if (value.find(key) != std::string::npos) {
            return value;
        }
    }
    return std::string();
}

std::vector<std::string>
Overflow::Helper::stringSplit(const std::string& data,
                              std::string token)
{
    std::vector<std::string> output;
    std::string input(data);
    
    size_t pos = std::string::npos;
    do {
        pos = input.find(token);
        output.push_back(input.substr(0, pos));
        
        if (std::string::npos != pos) {
            input = input.substr(pos + token.size());
        }
    } while (std::string::npos != pos);
    
    return output;
}

std::vector<std::pair<int, int>>
Overflow::Helper::splitBuffer(const unsigned char * buffer,
                              const size_t length,
                              const std::string& delim)
{
    std::vector<std::pair<int, int>> lines;
    size_t delim_length = delim.size();
    
    size_t line_begin_offset = 0;
    for (size_t i = 0; i < length; ++i)
    {
        unsigned char idx = buffer[i];
        unsigned char first_char = delim[0];
        
        if (first_char == idx)
        {    
            bool is_long_enough = (length - i) >= delim_length;
            
            if (is_long_enough)
            {    
                const void* cmp_buf = buffer + i;
                bool is_match = memcmp(cmp_buf, delim.c_str(), delim_length) == 0;
                
                if (is_match)
                {
                    int line_length = (i + delim_length) - line_begin_offset;     
                    lines.push_back(std::pair<int, int>(line_begin_offset, line_length));
                    i += delim_length - 1;
                    line_begin_offset = i + 1; 
                }
            }
        }
    }
    
    // handle trailing and empty cases
    if (lines.size() == 0)
    {
        lines.push_back(std::pair<int,int>(0, length));
    }
    else
    {
        std::pair<int,int>& last_pair = lines.back();
        size_t total = last_pair.first + last_pair.second;
        bool did_read_end = total == length;
        
        if (did_read_end == false)
        {
            // offset is always the total from previous lines as each
            // split is inclusive of delim
            lines.push_back(std::pair<int,int>(total, length - total));
        }
    }
    
    return lines;
}

