/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <assert.h>

#include "webrtc/libjingle/examples/call/talk_call_android.h"

#include "webrtc/examples/android/media_demo/jni_wh/jni_helpers.h"
#include "webrtc/modules/video_capture/video_capture_internal.h"
#include "webrtc/modules/video_render/video_render_internal.h"
#include "webrtc/voice_engine/include/voe_base.h"
#include "webrtc/libjingle/examples/call/talk_call.h"

// Macro for native functions that can be found by way of jni-auto discovery.
// Note extern "C" is needed for "discovery" of native methods to work.
#define JOWW(rettype, name)                                             \
  extern "C" rettype JNIEXPORT JNICALL Java_org_webrtc_webrtcdemo_##name

JavaVM* g_vm = NULL;
rtc::Thread* my_workerthread = NULL;
rtc::Thread* my_clientthread = NULL;
void* sv_local = NULL;
void* sv_remote = NULL;

//static JavaVM* g_vm = NULL;
static jobject jObj_;
static jobject gClassLoader;
static jmethodID gFindClassMethod;
static JNIEnv* client_env = NULL;
static JNIEnv* worker_env = NULL;

//获取JNIEnv
bool isAttached = false;
JNIEnv* MyGetEnv()
{
    int status;
    JNIEnv* env = NULL;
    status = g_vm->GetEnv((void **)&env, JNI_VERSION_1_4);
    if(status < 0)  
    {
        status = g_vm->AttachCurrentThread(&env, NULL);
        if(status < 0)  
        {
            return NULL;
        }  
        isAttached = true;
    }
    return env;
}

void DetachCurrent()  
{  
    if(isAttached)
    {
	isAttached = false;
        g_vm->DetachCurrentThread();
    }  
}

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  // Only called once.
  //CHECK(!g_vm, "OnLoad called more than once");
  
  g_vm = vm;

  return JNI_VERSION_1_4;
}

extern void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data);
void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data) {
	AndroidCallback(command, data);
}

//WH——Java CallBack_MethodID

//Xmpp_MethodID
jmethodID callBack_Xmpp_mID_;

//Connection State_MethodID
jmethodID callBack_Logging_mID_;
jmethodID callBack_Logged_In_mID_;
jmethodID callBack_Logged_Out_mID_;

//P2P File_MethodID
jmethodID callBack_File_SessionID_mID_;
jmethodID callBack_File_Request_mID_;
jmethodID callBack_File_Accept_mID_;
jmethodID callBack_File_Reject_mID_;
jmethodID callBack_File_Terminate_mID_;
jmethodID callBack_File_Progress_mID_;

//Call_MethodID
jmethodID callBack_Call_mID_;
jmethodID callBack_Call_Accept_mID_;
jmethodID callBack_Call_Reject_mID_;
jmethodID callBack_Call_Terminate_mID_;
jmethodID callBack_Screencast_mID_;
jmethodID callBack_Camera_mID_;
jmethodID callBack_Camera_Stop_mID_;
jmethodID callBack_Local_Frame_MetaData_mID_;
jmethodID callBack_Remote_Frame_MetaData_mID_;
jmethodID callBack_Local_Frame_Coming_mID_;
jmethodID callBack_Remote_Frame_Coming_mID_;
//要求指针类型参数，待完成
jmethodID callBack_Local_Renderer_Handle_mID_;
jmethodID callBack_Remote_Renderer_Handle_mID_;

//Debug Info_MethodID
jmethodID callBack_Debug_Info_mID_;


//注册 Java 回调函数
void RegisterCallbackmID(JNIEnv* env, jobject obj) {

	jclass native_webRtcTransport_class = env->GetObjectClass(obj);

	//Xmpp_MethodID
	callBack_Xmpp_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Xmpp", "([B)V");

	//Connection State_MethodID
	callBack_Logging_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Logging", "()V");

	callBack_Logged_In_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Logged_In", "(Ljava/lang/String;)V");

	callBack_Logged_Out_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Logged_Out", "()V");

	//P2P File_MethodID
	callBack_File_SessionID_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_File_SessionID", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

	callBack_File_Request_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_File_Request", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

	callBack_File_Accept_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_File_Accept", "(Ljava/lang/String;)V");

	callBack_File_Reject_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_File_Reject", "(Ljava/lang/String;)V");

	callBack_File_Terminate_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_File_Terminate", "(Ljava/lang/String;)V");

	callBack_File_Progress_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_File_Progress", "(Ljava/lang/String;I)V");

	//Call_MethodID
	callBack_Call_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Call", "(Ljava/lang/String;)V");

	callBack_Call_Accept_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Call_Accept", "()V");

	callBack_Call_Reject_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Call_Reject", "()V");

	callBack_Call_Terminate_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Call_Terminate", "()V");

	callBack_Screencast_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Screencast", "()V");

	callBack_Camera_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Camera", "()V");

	callBack_Camera_Stop_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Camera_Stop", "()V");

	callBack_Local_Frame_MetaData_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Local_Frame_MetaData", "()V");

	callBack_Remote_Frame_MetaData_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Remote_Frame_MetaData", "()V");

	callBack_Local_Frame_Coming_mID_  = GetMethodID(env, native_webRtcTransport_class, "callBack_Local_Frame_Coming", "()V");

	callBack_Remote_Frame_Coming_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Remote_Frame_Coming", "()V");

	//要求指针类型参数，待完成
	callBack_Local_Renderer_Handle_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Local_Renderer_Handle", "()V");

	callBack_Remote_Renderer_Handle_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Remote_Renderer_Handle", "()V");

	//Debug Info_MethodID
	callBack_Debug_Info_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Debug_Info", "(Ljava/lang/String;Ljava/lang/String;)V");

}


//WH——Java CallBack接口函数
void AndroidCallback(Enum_RecievedMessage command, ThreadShareData* data) {

	JNIEnv* env_ = MyGetEnv();

	switch(command)
	{
	  //Xmpp
	  case R_XMPP_PROTOCOL:
		/* 参数描述
		  string xmppData // android下废弃 int ByteLength
		*/
		env_->CallVoidMethod(jObj_, callBack_Xmpp_mID_, str2jbyteArray(env_, data->ReadString(0).c_str()));

		break;

	  //Connection State
	  case R_LOGGING:
		env_->CallVoidMethod(jObj_, callBack_Logging_mID_);
		break;

	  case R_LOGGED_IN:
		/* 参数描述
		  int ConnectionState
		  string userJid
		*/
		env_->CallVoidMethod(jObj_, callBack_Logged_In_mID_, mystr2jstring(env_, data->ReadString(0)));

		break;

	  case R_LOGGED_OUT:
		env_->CallVoidMethod(jObj_, callBack_Logged_Out_mID_);

		break;

	  //P2P File
	  case R_FILE_SESSION_ID:
		/* 参数描述
		  string sId
		  string jid
		  string file
		*/
		env_->CallVoidMethod(jObj_, callBack_File_SessionID_mID_, mystr2jstring(env_, data->ReadString(0)),  mystr2jstring(env_, data->ReadString(1)),  mystr2jstring(env_, data->ReadString(2)));

		break;
	  case R_FILE_REQUEST:
		/* 参数描述
		  string sId
		  string jid
		  string description
		*/
		env_->CallVoidMethod(jObj_, callBack_File_Request_mID_, mystr2jstring(env_, data->ReadString(0)), mystr2jstring(env_, data->ReadString(1)), mystr2jstring(env_, data->ReadString(2)));

		break;
	  case R_FILE_ACCEPT:
		/* 参数描述
		  string sId
		*/
		env_->CallVoidMethod(jObj_, callBack_File_Accept_mID_, mystr2jstring(env_, data->ReadString(0)));

		break;
	  case R_FILE_REJECT:
		/* 参数描述
		  string sId
		*/
		env_->CallVoidMethod(jObj_, callBack_File_Reject_mID_, mystr2jstring(env_, data->ReadString(0)));

		break;
	  case R_FILE_TERMINATE:
		/* 参数描述
		  string sId
		*/
		env_->CallVoidMethod(jObj_, callBack_File_Terminate_mID_, mystr2jstring(env_, data->ReadString(0)));

		break;
	  case R_FILE_PROGRESS:
		/* 参数描述
		  string sId
		  int progress
		*/
		env_->CallVoidMethod(jObj_, callBack_File_Progress_mID_, mystr2jstring(env_, data->ReadString(0)), data->ReadInt(0));

		break;

	  //Call
	  case R_CALL:
		/* 参数描述
		  string jid
		*/
		env_->CallVoidMethod(jObj_, callBack_Call_mID_, mystr2jstring(env_, data->ReadString(0)));

		break;
	  case R_CALL_ACCEPT:
		env_->CallVoidMethod(jObj_, callBack_Call_Accept_mID_);

		break;
	  case R_CALL_REJECT:
		env_->CallVoidMethod(jObj_, callBack_Call_Reject_mID_);

		break;
	  case R_CALL_TERMINATE:
		env_->CallVoidMethod(jObj_, callBack_Call_Terminate_mID_);

		break;
	  case R_SCREENCAST:
		env_->CallVoidMethod(jObj_, callBack_Screencast_mID_);

		break;
	  case R_CAMERA:
		env_->CallVoidMethod(jObj_, callBack_Camera_mID_);

		break;
	  case R_STOP_CAPTURE:
		env_->CallVoidMethod(jObj_, callBack_Camera_Stop_mID_);

		break;
	  case R_LOCAL_FRAME_METADATA:
		env_->CallVoidMethod(jObj_, callBack_Local_Frame_MetaData_mID_);

		break;
	  case R_REMOTE_FRAME_METADATA:
		env_->CallVoidMethod(jObj_, callBack_Remote_Frame_MetaData_mID_);

		break;
	  case R_LOCAL_FRAME_COMING:
		env_->CallVoidMethod(jObj_, callBack_Local_Frame_Coming_mID_);

		break;
	  case R_REMOTE_FRAME_COMING:
		env_->CallVoidMethod(jObj_, callBack_Remote_Frame_Coming_mID_);

		break;

	  //要求指针类型参数，待完成
	  case R_LOCAL_RENDERER_HANDLE:
		env_->CallVoidMethod(jObj_, callBack_Local_Renderer_Handle_mID_);

		break;
	  case R_REMOTE_RENDERER_HANDLE:
		env_->CallVoidMethod(jObj_, callBack_Remote_Renderer_Handle_mID_);

		break;

	  //Debug Info
	  case R_DEBUG_INFO:
		/* 参数描述
		  string info
		  string content
		*/
		env_->CallVoidMethod(jObj_, callBack_Debug_Info_mID_, mystr2jstring(env_, data->ReadString(0)), mystr2jstring(env_, data->ReadString(1)));

		break;
	}

	if(data)
	   delete data;
	DetachCurrent();
}

//WH——libjingle Native接口函数

//WH——日志输出
jmethodID callBack_Test_mID_;
void callBack_Test_Method(std::string s) {

	JNIEnv* env_ = MyGetEnv();

	env_->CallVoidMethod(jObj_, callBack_Test_mID_, mystr2jstring(env_, s));

	DetachCurrent();
}

//注册 VideoCapture VideoRender VoiceEngine
JOWW(void, NativeWebRtcTransport_Register)(JNIEnv* env, jobject obj, jobject context) {

  webrtc::SetCaptureAndroidVM(g_vm, context);
  webrtc::SetRenderAndroidVM(g_vm);
  webrtc::VoiceEngine::SetAndroidObjects(g_vm, env, context);
}

JOWW(void, NativeWebRtcTransport_unRegister)(JNIEnv* jni, jclass) {

  webrtc::SetCaptureAndroidVM(NULL, NULL);
  webrtc::SetRenderAndroidVM(NULL);
  webrtc::VoiceEngine::SetAndroidObjects(NULL, NULL, NULL);
}

//创建 Local Remote——SurfaceView
JOWW(void, NativeWebRtcTransport_CreateSurfaceView)(JNIEnv* env, jobject obj, jobject local, jobject remote) {

  /*ThreadShareData* shareData = new ThreadShareData(0, 0, 2);

  shareData->WriteLong(reinterpret_cast<long>(env->NewGlobalRef(local)), 0);
  shareData->WriteLong(reinterpret_cast<long>(env->NewGlobalRef(remote)), 1);

  SendCommand((int)S_REGISTER_SV, shareData);*/

  sv_local = reinterpret_cast<void*>(env->NewGlobalRef(local));
  sv_remote = reinterpret_cast<void*>(env->NewGlobalRef(remote));

}

//暂废
JNIEnv* GetThreadEnv(bool thread) {

   if(thread)
	return worker_env;
   else
	return client_env;
}

//创建WorkerThread
JOWW(void, NativeWebRtcTransport_CreateWorkerThread)(JNIEnv* env, jobject obj) {     
  
  //暂废
  //worker_env = env;

  my_workerthread = new rtc::Thread();
  rtc::ThreadManager::Instance()->SetCurrentThread(my_workerthread);
  my_workerthread->Run();

}

//登录
JOWW(void, NativeWebRtcTransport_Login)(JNIEnv* env, jobject obj, jstring name, jstring passWord, jstring resource) {     

  //暂废
  //client_env = env;

  jObj_ = env->NewGlobalRef(obj);

  jclass native_webRtcTransport_class = env->GetObjectClass(obj);

  callBack_Test_mID_ = GetMethodID(env, native_webRtcTransport_class, "callBack_Test", "(Ljava/lang/String;)V");

  //注册 Java 回调函数
  RegisterCallbackmID(env, obj);

  callBack_Test_Method("JNILogin");
  Login(JavaToStdString(env, name), JavaToStdString(env, passWord), JavaToStdString(env, resource));

  env->DeleteGlobalRef(jObj_);

}

//登出
JOWW(void, NativeWebRtcTransport_Logout)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_LOGOUT, NULL);

}

//Xmpp
JOWW(void, NativeWebRtcTransport_sendXmpp)(JNIEnv* jni, jobject obj, jstring xmppData) {

  std::string xmppdata(JavaToStdString(jni, xmppData));

  ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
  shareData->WriteString(xmppdata, 0);

  SendCommand((int)S_XMPPP_ROTOCOL, shareData);

}

//P2P File
JOWW(void, NativeWebRtcTransport_sendFileRequest)(JNIEnv* jni, jobject obj, jstring toJid, jstring fileFullPath, jstring fileDesc) {

	std::string tojid(JavaToStdString(jni, toJid));
	std::string filefullpath(JavaToStdString(jni, fileFullPath));
	std::string filedesc(JavaToStdString(jni, fileDesc));

	ThreadShareData* shareData = new ThreadShareData(3, 0, 0);
	shareData->WriteString(tojid, 0);
	shareData->WriteString(filefullpath, 1);
	shareData->WriteString(filedesc, 2);

	SendCommand((int)S_FILE_REQUEST, shareData);

}

JOWW(void, NativeWebRtcTransport_sendFileAccept)(JNIEnv* jni, jobject obj, jstring sId, jstring fileName, jstring savePath) {

	std::string sid(JavaToStdString(jni, sId));
	std::string filename(JavaToStdString(jni, fileName));
	std::string savepath(JavaToStdString(jni, savePath));

	ThreadShareData* shareData = new ThreadShareData(3, 0, 0);

	shareData->WriteString(sid, 0);
	shareData->WriteString(filename, 1);
	shareData->WriteString(savepath, 2);

	SendCommand((int)S_FILE_ACCEPT, shareData);

}

JOWW(void, NativeWebRtcTransport_sendFileReject)(JNIEnv* jni, jobject obj, jstring sId) {

	std::string sid(JavaToStdString(jni, sId));

	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(sid, 0);

	SendCommand((int)S_FILE_REJECT, shareData);

}

JOWW(void, NativeWebRtcTransport_sendFileTermintate)(JNIEnv* jni, jobject obj, jstring sId) {

	std::string sid(JavaToStdString(jni, sId));

	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(sid, 0);

	SendCommand((int)S_FILE_TERMINTATE, shareData);

}

//Call
JOWW(void, NativeWebRtcTransport_sendCall)(JNIEnv* jni, jobject obj, jstring toJid, jint local_Width, jint local_Height, jint remote_Width, jint remote_Height) {

	callBack_Test_Method("JNIVCall");

	std::string tojid(JavaToStdString(jni, toJid));

	ThreadShareData* shareData = new ThreadShareData(1, 4, 0);
	shareData->WriteString(tojid, 0);

	shareData->WriteInt(local_Width, 0);
	shareData->WriteInt(local_Height, 1);
	shareData->WriteInt(remote_Width, 2);
	shareData->WriteInt(remote_Height, 3);

	SendCommand((int)S_CALL, shareData);

}

JOWW(void, NativeWebRtcTransport_sendCallAccept)(JNIEnv* jni, jobject obj, jint local_Width, jint local_Height, jint remote_Width, jint remote_Height) {

	ThreadShareData* shareData = new ThreadShareData(0, 4, 0);
	shareData->WriteInt(local_Width, 0);
	shareData->WriteInt(local_Height, 1);
	shareData->WriteInt(remote_Width, 2);
	shareData->WriteInt(remote_Height, 3);

	SendCommand((int)S_CALL_ACCEPT, shareData);

}

JOWW(void, NativeWebRtcTransport_sendCallReject)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_CALL_REJECT, NULL);

}

JOWW(void, NativeWebRtcTransport_sendScreencast_Start)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_START_SCREENCAST, NULL);

}

JOWW(void, NativeWebRtcTransport_sendCameraStart)(JNIEnv* jni, jobject obj) {

	callBack_Test_Method("JNICameraStart");
  SendCommand((int)S_START_CAMERA, NULL);

}

JOWW(void, NativeWebRtcTransport_sendCameraStop)(JNIEnv* jni, jobject obj) {

	callBack_Test_Method("JNICameraStop");
  SendCommand((int)S_STOP_CAPTURE, NULL);

}

//libjingle 未实现此命令
JOWW(void, NativeWebRtcTransport_sendCameraRestart)(JNIEnv* jni, jobject obj, jint width, jint height, jint interval, jint colorspace) {

	ThreadShareData* shareData = new ThreadShareData(0, 4, 0);
	shareData->WriteInt(width, 0);
	shareData->WriteInt(height, 1);
	shareData->WriteInt(interval, 2);
	shareData->WriteInt(colorspace, 3);

	SendCommand((int)S_RESTART_CAPTURE, shareData);

}

//libjingle 未实现此命令
JOWW(void, NativeWebRtcTransport_sendOpenLocalRenderer)(JNIEnv* jni, jobject obj, jint videoWidth, jint videoHeight, jint parentHandle) {

	ThreadShareData* shareData = new ThreadShareData(0, 3, 0);
	shareData->WriteInt(videoWidth, 0);
	shareData->WriteInt(videoHeight, 1);
	shareData->WriteInt(parentHandle, 2);

	SendCommand((int)S_OPEN_LOCAL_RENDERER, shareData);

}

JOWW(void, NativeWebRtcTransport_sendOpenRemoteRenderer)(JNIEnv* jni, jobject obj, jint videoWidth, jint videoHeight, jint parentHandle) {

	ThreadShareData* shareData = new ThreadShareData(0, 3, 0);
	shareData->WriteInt(videoWidth, 0);
	shareData->WriteInt(videoHeight, 1);
	shareData->WriteInt(parentHandle, 2);

	SendCommand((int)S_OPEN_REMOTE_RENDERER, shareData);

}

JOWW(void, NativeWebRtcTransport_sendMute)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_MUTE, NULL);

}

JOWW(void, NativeWebRtcTransport_sendUnMute)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_UNMUTE, NULL);

}

JOWW(void, NativeWebRtcTransport_sendVideoMute)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_VIDEO_MUTE, NULL);

}

JOWW(void, NativeWebRtcTransport_sendVideoUnmute)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_VIDEO_UNMUTE, NULL);

}

JOWW(void, NativeWebRtcTransport_sendSetRemoteVolume)(JNIEnv* jni, jobject obj, jstring volume) {

	std::string v(JavaToStdString(jni, volume));

  	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(v, 0);

	SendCommand((int)S_SET_REMOTE_VOLUME, shareData);

}

JOWW(void, NativeWebRtcTransport_sendRecord)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_RECORD, NULL);

}

JOWW(void, NativeWebRtcTransport_sendUnRecord)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_UNRECORD, NULL);

}

JOWW(void, NativeWebRtcTransport_sendHangUp)(JNIEnv* jni, jobject obj) {

  SendCommand((int)S_HANG_UP, NULL);

}

