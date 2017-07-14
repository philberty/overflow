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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <glog/logging.h>

#include <thread>
#include <chrono>


namespace OverflowTest
{
    class Helpers
    {
    public:
        static void setupTestLogger ()
        {
            FLAGS_logtostderr = 1;
            google::InitGoogleLogging ("Overflow Test");
        }

        static void sleep (const unsigned int seconds)
        {
            std::this_thread::sleep_for (std::chrono::seconds(seconds));
        }

        static void printOutAllNaluTypes (const unsigned char *buffer, size_t length)
        {
            std::string nalus;
            for (size_t i = 0; i < length - 4; ++i)
            {
                if (buffer[i] == 0x00 && buffer[i+1] == 0x00 && buffer[i+2] == 0x01)
                {    
                    if (i > 0 && buffer[i - 1] == 0x00)
                    {
                        int nalu_type = (buffer[i + 3] & 0x1F);
                        nalus += "[" + std::to_string(nalu_type) + "] ";
                    }
                    else
                    {
                        int nalu_type = (buffer[i + 3] & 0x1F);
                        nalus += "[" + std::to_string(nalu_type) + "] ";
                    }
                }
            }
            
            LOG(INFO) << "NALUS: " << nalus;
        }        
    };
};

#endif //__UTIL_H__
