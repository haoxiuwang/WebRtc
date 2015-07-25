
#ifndef WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_ANDROID_H_
#define WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_ANDROID_H_

#include <string>

#include <jni.h>
#include <android/log.h>

#include "webrtc/libjingle/examples/call/PublicCallback.h"

//以下定义在 webrtc/examples/android/my_android/jni_android/talk_call_android.cc

//SurfaceView
extern void* sv_local;
extern void* sv_remote;

//WorkThread
extern rtc::Thread* my_workerthread;
extern rtc::Thread* my_clientthread;

//获取跟线程相关的Env //暂废
extern JNIEnv* GetThreadEnv(bool thread);

//JAVA_VM
extern JavaVM* g_vm;

//WH——Java CallBack接口函数
extern void AndroidCallback(Enum_RecievedMessage command, ThreadShareData* data);
extern void callBack_Test_Method(std::string s);

#endif  // WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_ANDROID_H_
