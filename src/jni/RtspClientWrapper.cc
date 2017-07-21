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

#include "RtspClientWrapper.h"
#include "../RtspWanClient.h"
#include "RtspDelegateWrapper.h"

#include "JniLogSink.h"


static void JniSetupLogging()
{
    static OverflowJni::JniLogSink _logsink;
    static bool setup = false;

    if (not setup) {
        AddLogSink(&_logsink);
        setup = true;
    }
}

Overflow::RtspWanClient* get_native_client_pointer(JNIEnv *env, jobject callingObject) {
    jclass c = env->GetObjectClass(callingObject);
    jfieldID field = env->GetFieldID(c, "_client", "J");
    jlong handle = env->GetLongField(callingObject, field);
    return reinterpret_cast<Overflow::RtspWanClient*>(handle);
}

OverflowJni::RtspDelegateWrapper* get_native_delegate_pointer(JNIEnv *env, jobject callingObject) {
    jclass c = env->GetObjectClass(callingObject);
    jfieldID field = env->GetFieldID(c, "_delegate", "J");
    jlong handle = env->GetLongField(callingObject, field);
    return reinterpret_cast<OverflowJni::RtspDelegateWrapper*>(handle);
}

void set_native_delegate_pointer(JNIEnv *env, jobject callingObject, OverflowJni::RtspDelegateWrapper* delegate) {
    jclass c = env->GetObjectClass(callingObject);
    jfieldID field = env->GetFieldID(c, "_delegate", "J");
    env->SetLongField(callingObject, field, reinterpret_cast<jlong>(delegate));
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_github_redbrain_overflow_RtspClient
 * Method:    start
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_github_redbrain_overflow_RtspClient_start
(JNIEnv *env, jobject calling_object)
{
    Overflow::RtspWanClient* client = get_native_client_pointer(env, calling_object);
    if (client != nullptr) {
        client->start();
    }
}

/*
 * Class:     com_github_redbrain_overflow_RtspClient
 * Method:    Stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_github_redbrain_overflow_RtspClient_stop
(JNIEnv *env, jobject calling_object)
{
    Overflow::RtspWanClient* client = get_native_client_pointer(env, calling_object);
    if (client != nullptr) {
        client->stop();
    }
}

/*
 * Class:     com_github_redbrain_overflow_RtspClient
 * Method:    _new_client
 * Signature: (Lcom/github/redbrain/overflow/RtspClientDelegate;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_github_redbrain_overflow_RtspClient__1new_1client
        (JNIEnv *env, jobject calling_object, jobject jdelegate, jstring jurl)
{
    JniSetupLogging();
    
    const char *u = env->GetStringUTFChars(jurl, JNI_FALSE);
    std::string url = std::string(u);

    OverflowJni::RtspDelegateWrapper *delegate = new OverflowJni::RtspDelegateWrapper(env, jdelegate);
    set_native_delegate_pointer(env, calling_object, delegate);

    jlong retval = 0;
    try {
        retval = reinterpret_cast<jlong>(new Overflow::RtspWanClient(delegate, url));
    }
    catch (...) {
        LOG(ERROR) << "Failed to create jni client";
    }
    return retval;
}

/*
 * Class:     com_github_redbrain_overflow_RtspClient
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_github_redbrain_overflow_RtspClient_close
        (JNIEnv *env, jobject calling_object)
{
    Overflow::RtspWanClient* client = get_native_client_pointer(env, calling_object);

    client->join();
    
    if (client != nullptr) {
        delete client;
    }

    OverflowJni::RtspDelegateWrapper *delegate = get_native_delegate_pointer(env, calling_object);
    if (delegate != nullptr) {
        delete delegate;
    }
}

#ifdef __cplusplus
}
#endif
