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


#ifndef __RTSP_DELEGATE_WRAPPER_JNI__
#define __RTSP_DELEGATE_WRAPPER_JNI__

#include "../IRtspDelegate.h"

#include <glog/logging.h>
#include <jni.h>


namespace OverflowJni
{
    class RtspDelegateWrapper: public Overflow::IRtspDelegate
    {
    public:
        RtspDelegateWrapper(JNIEnv *env, jobject jdelegate)
        {
            env->GetJavaVM(&jvm);

            delegate = env->NewGlobalRef(jdelegate);
            jclass c = env->GetObjectClass(delegate);
            payload_method_id = env->GetMethodID(c, "payload", "(Ljava/nio/ByteBuffer;)V");
        }

        // Invalid Response
        void Invalid() override {

        }

        // Timeout
        void Timeout() override {

            // TODO
        }

        // Payload
        void Payload(const unsigned char * buf, const size_t length) override
        {
            JNIEnv *env;
            
            // double check it's all ok
            int getEnvStat = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
            if (getEnvStat == JNI_EDETACHED) {
                
		if (jvm->AttachCurrentThread(&env, NULL) != 0) {
                    LOG(ERROR) << "Failed to attach";
                    return;
		}
            }
            else if (getEnvStat == JNI_EVERSION) {
		LOG(ERROR) << "GetEnv: version not supported";
                return;
            }
            else if (getEnvStat != JNI_OK) {
		LOG(ERROR) << "unknown not ok error attaching";
                return;
            }

            jobject framebuffer = env->NewDirectByteBuffer((void *)buf, length);
            env->CallVoidMethod(delegate, payload_method_id, framebuffer);
            
            if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
            }

            jvm->DetachCurrentThread();
        }

        // REDIRECT
        void ServerRedirect() override {

        }

        // ANNOUNCE
        void ServerAnnounce() override {

        }

    private:
        JavaVM *jvm;
        jobject delegate;
        jmethodID payload_method_id;
    };

};

#endif //RTSP_DELEGATE_WRAPPER_JNI
