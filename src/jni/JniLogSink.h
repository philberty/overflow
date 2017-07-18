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

#ifndef __OVERFLOW_JNI_LOG_SINK__
#define __OVERFLOW_JNI_LOG_SINK__

#include <glog/logging.h>
#include <android/log.h>

#define  LOG_TAG    "Overflow"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


namespace OverflowJni
{
    class JniLogSink: public google::LogSink
    {
    public:
        ~JniLogSink() { }
        
        // Sink's logging logic (message_len is such as to exclude '\n' at the end).
        // This method can't use LOG() or CHECK() as logging system mutex(s) are held
        // during this call.
        void send(google::LogSeverity severity,
                  const char* full_filename,
                  const char* base_filename,
                  int line,
                  const struct ::tm* tm_time,
                  const char* message,
                  size_t message_len) override
        {
            std::string msg(
                google::LogSink::ToString(severity, base_filename, line, tm_time, message, message_len)
                );

            switch (severity)
            {
            default:
                LOGI("%s\n", msg.c_str());
                break;
            }
        }  
    };
};

#endif //__OVERFLOW_JNI_LOG_SINK__
