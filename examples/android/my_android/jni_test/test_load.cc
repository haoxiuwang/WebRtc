/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <jni.h>

#include <assert.h>


// Macro for native functions that can be found by way of jni-auto discovery.
// Note extern "C" is needed for "discovery" of native methods to work.
#define JOWW(rettype, name)                                             \
  extern "C" rettype JNIEXPORT JNICALL Java_com_example_nativeaudio_##name

static JavaVM* g_vm;

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  // Only called once.
  //CHECK(!g_vm, "OnLoad called more than once");
  g_vm = vm;
  return JNI_VERSION_1_4;
}


//WH——libjingle Native接口函数

//登录
JOWW(void, NativeAudio_Login)(JNIEnv* env, jobject obj, jstring name, jstring passWord) {     


}


