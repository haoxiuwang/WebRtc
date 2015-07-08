#pragma once
#ifndef PUBLIC_CALL_BACK_FUNCTION_H
#define PUBLIC_CALL_BACK_FUNCTION_H

#include <jni.h>

#include <string>
//WH
#include <android/log.h>
#include <stdio.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)

#include "webrtc/base/thread.h"
#include "webrtc/base/messagehandler.h"

enum Enum_RecievedMessage
{

// 传输Xmpp协议
	/// <summary>
	/// 收到由libjingle传输层转发的Xmpp协议
	/// lpstr XmppProtocol;  int ByteLength
	/// </summary> 
	R_XMPP_PROTOCOL = 0,

//  传输Xmpp协议End

// 登陆状态

	/// <summary>
	/// 正在登陆
	/// </summary>
	R_LOGGING = 10,

	/// <summary>
	/// 登入成功，返回带资源的Jid
	/// lpwstr FullJid
	/// </summary> 
	R_LOGGED_IN,

	/// <summary>
	/// 从服务器登出
	/// </summary>
	R_LOGGED_OUT,

// 登陆状态End

// 文件传输

	/// <summary>
	/// 文件发送方已创建Session组件，返回SessionId
	/// lpwstr SessionId;  lpwstr Jid;  lpwstr FullFileName;
	/// </summary>       
	R_FILE_SESSION_ID = 50,

	/// <summary>
	/// 对方向己方发送文件
	/// lpwstr SessionId;  lpwstr Jid;  lpwstr FileDescription;
	/// </summary>
	R_FILE_REQUEST,

	/// <summary>
	/// 对方接受文件
	/// lpwstr SessionId;
	/// </summary>
	R_FILE_ACCEPT,

	/// <summary>
	/// 对方拒绝文件
	/// lpwstr SessionId;
	/// </summary>
	R_FILE_REJECT,

	/// <summary>
	/// 对方终止传输文件
	/// lpwstr SessionId;
	/// </summary>
	R_FILE_TERMINATE,

	/// <summary>
	/// 获取文件进度
	/// lpwstr SessionId;  int Progress
	/// </summary>
	R_FILE_PROGRESS,

// 文件传输End

// 视频音频

// 会话协议

	/// <summary>
	/// 对方发送Call请求
	/// lpwstr Jid;
	/// </summary>
	R_CALL = 100,

	/// <summary>
	/// 对方接受Call
	/// </summary>
	R_CALL_ACCEPT,

	/// <summary>
	/// 对方拒绝Call
	/// </summary>
	R_CALL_REJECT,

	/// <summary>
	/// 对方终止Call
	/// </summary>
	R_CALL_TERMINATE,

// 会话协议End

	/// <summary>
	/// 对方开启远程桌面
	/// </summary>
	R_SCREENCAST,

	/// <summary>
	/// 对方开启摄像头捕获
	/// </summary>
	R_CAMERA,

	/// <summary>
        ///  获得本地的视频帧
        ///  void* 帧buffer int width int height
        /// </summary>
        R_LOCAL_FRAME_METADATA,

        /// <summary>
        ///  获得对方的视频帧
        ///  void* 帧buffer int width int height
        /// </summary>
        R_REMOTE_FRAME_METADATA,

	/// <summary>
	/// 对方关掉视频捕获
	/// </summary>
	R_STOP_CAPTURE,

	/// <summary>
	/// 已创建本地视频Renderer
	/// 窗口句柄 void* handle 
	/// </summary>
	R_LOCAL_RENDERER_HANDLE,

	/// <summary>
	/// 已创建对方视频Renderer
	/// 窗口句柄 void* handle
	/// </summary>
	R_REMOTE_RENDERER_HANDLE,

        /// <summary>
        ///  获得本地的视频帧
        /// </summary>
        R_LOCAL_FRAME_COMING = 115,

        /// <summary>
        ///  获得对方的视频帧
        /// </summary>
        R_REMOTE_FRAME_COMING,


// 视频音频End

	///<summary>
	///调试信息	lpwstr info;	lpwstr content;
	///</summary>
	R_DEBUG_INFO = 130
};

enum Enum_SendMessage
{
// 传输Xmpp协议

	/// <summary>
	/// 利用libjingle传输层转发xmpp协议
	/// lpstr XmppProtocl
	/// </summary>
	S_XMPPP_ROTOCOL = 0,

// 传输Xmpp协议End

// 登陆状态

	/// <summary>
	/// 从服务器登出
	/// </summary>
	S_LOGOUT = 10,

// 登陆状态End

// 文件传输

	/// <summary>
	/// 发送文件
	/// lpstr RecieverJid;  lpstr FullFileName;  lpstr FileDescription;
	/// </summary>
	S_FILE_REQUEST = 50,

	/// <summary>
	/// 接受文件
	/// lpstr SessionId;  lpstr FileName;  lpstr SavePath;
	/// </summary>
	S_FILE_ACCEPT,

	/// <summary>
	/// 拒绝接受文件
	/// lpstr SessionId;
	/// </summary>
	S_FILE_REJECT,

	/// <summary>
	/// 终止传输文件
	/// lpstr SessionId;
	/// </summary>
	S_FILE_TERMINTATE,

// 文件传输End

// 视频音频

// 会话协议

	/// <summary>
	/// Call请求
	/// lpstr RecieveJid;  int localWidth;  int localHeight; int remoteWidth;  int remoteHeight
	/// </summary>
	S_CALL = 100,

	/// <summary>
	/// 接受Call
	/// int localWidth;  int localHeight; int remoteWidth;  int remoteHeight
	/// </summary>
	S_CALL_ACCEPT,

	/// <summary>
	/// 拒绝Call
	/// </summary>
	S_CALL_REJECT,

	/// <summary>
	/// 终止并删除会话
	/// </summary>
	S_HANG_UP,

// 会话协议End

	/// <summary>
	/// 停止己方的音频传输
	/// </summary>
	S_MUTE,

	/// <summary>
	/// 重新开始己方的音频传输
	/// </summary>
	S_UNMUTE,

	/// <summary>
	/// 控制对方传输过来的音频音量
	/// lpstr Volume
	/// </summary>
	S_SET_REMOTE_VOLUME,

	/// <summary>
	/// 停止己方的视频传输
	/// </summary>
	S_VIDEO_MUTE,

	/// <summary>
	/// 重新开始己方的视频传输
	/// </summary>
	S_VIDEO_UNMUTE,

	/// <summary>
	/// 开启远程桌面
	/// </summary>
	S_START_SCREENCAST,

	/// <summary>
	/// 开启摄像头捕获
	/// </summary>
	S_START_CAMERA,

	/// <summary>
	/// 停止捕获视频
	/// </summary>
	S_STOP_CAPTURE,

	/// <summary>
	/// 重设捕获的视频数据
	/// int Width;  int Height;  int Interval;  int ColorSpace; 
	/// </summary>
	S_RESTART_CAPTURE,

	/// <summary>
	/// 打开本地视频Renderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_LOCAL_RENDERER,

	/// <summary>
	/// 打开对方视频Renderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_REMOTE_RENDERER,

// 视频音频End

// 录音

	/// <summary>
	/// 开始录音
	/// </summary>
	S_RECORD = 150,

	/// <summary>
	/// 停止录音
	/// </summary>
	S_UNRECORD,

// 录音End

	///<summary>
	///调试信息	lpwstr info;	lpwstr content;
	///</summary>
	S_DEBUG_INFO,

//创建 Local Remote——SurfaceView
	/// <summary>
	/// Local_SurfaceView Remote_SurfaceView
	/// long local;  long remote;
	/// </summary>
        S_REGISTER_SV
};


class ThreadShareData
{
public:
	ThreadShareData(int string_count,int int_count, int long_count);
	~ThreadShareData();
	void WriteInt(int value,int pos);
	void WriteString(std::string strUtf8,int pos);
	void WriteLong(long value,int pos);
	int ReadInt(int pos);
	long ReadLong(long pos);
	std::string ReadString(int pos);

private:
	std::string *string_array_ = NULL;
	int* int_array_ = NULL;
	long* long_array_ = NULL;
};


/*
class ManagedDataReader
{
	public:
		ManagedDataReader(void** pointer);
		int ReadInt();
		std::string ReadString();
		std::wstring ReadUnicode();
		LPCSTR ReadString_s();
		void ReadToEnd();

	private:
		void** _pointer;
		int    _offset;
};

class ManagedDataWriter
{
	public:
		ManagedDataWriter(int m_count);
		void WriteInt(int value);
		void WriteString(LPCSTR strUtf8);
		void WriteAnciString(LPCSTR strUtf8);
		void WriteUnicodeToUtf8(LPWSTR strUnicode);
		void WritePointer(void* pointer);
		void** ToArray();

	private:
		void** pointer_;
		int    offset_;
};*/


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
extern void NewDayCallback(Enum_RecievedMessage command, ThreadShareData* data);
extern void callBack_Test_Method(std::string s);

#endif
