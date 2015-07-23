#pragma once
#ifndef PUBLIC_CALL_BACK_FUNCTION_H_
#define PUBLIC_CALL_BACK_FUNCTION_H_

#include <string>
#include <stdio.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)

#include "webrtc/base/thread.h"
#include "webrtc/base/messagehandler.h"

enum Enum_RecievedMessage
{

#pragma region 传输Xmpp协议
	/// <summary>
	/// 收到由libjingle传输层转发的Xmpp协议
	/// lpstr XmppProtocol;  int ByteLength
	/// </summary> 
	R_XMPP_PROTOCOL = 0,

#pragma endregion  传输Xmpp协议

#pragma region 登陆状态

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

#pragma endregion 登陆状态

#pragma region 文件传输

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

#pragma endregion 文件传输

#pragma region 视频音频

#pragma region 会话协议

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

#pragma endregion 会话协议

#pragma region Capture及Frame
	/// <summary>
	/// 对方开启远程桌面
	/// </summary>
	R_SCREENCAST,

	/// <summary>
	/// 对方开启摄像头捕获
	/// </summary>
	R_CAMERA,

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
	///  void* 帧buffer int width int height
	/// </summary>
	R_LOCAL_FRAME_METADATA,

	/// <summary>
	///  获得对方的视频帧
	///  void* 帧buffer int width int height
	/// </summary>
	R_REMOTE_FRAME_METADATA,

	/// <summary>
	///  获得本地的视频帧
	/// </summary>
	R_LOCAL_FRAME_COMING = 115,

	/// <summary>
	///  获得对方的视频帧
	/// </summary>
	R_REMOTE_FRAME_COMING,

#pragma endregion Capture及Frame

#pragma endregion 视频音频

	///<summary>
	///调试信息	lpwstr info;	lpwstr content;
	///</summary>
	R_DEBUG_INFO = 130
};

enum Enum_SendMessage
{

#pragma region 传输Xmpp协议

	/// <summary>
	/// 利用libjingle传输层转发xmpp协议
	/// lpstr XmppProtocl
	/// </summary>
	S_XMPP_PROTOCOL = 0,

#pragma endregion 传输Xmpp协议

#pragma region 传输STUN服务器地址
	/// <summary>
	/// 传输STUN服务器IP地址和端口
	/// lpstr stun_ip_addr; int stun_port;
	/// </summary>
	S_STUN_ADDRESS = 5,

#pragma endregion 传输STUN服务器地址

#pragma region 登陆状态

	/// <summary>
	/// 从服务器登出
	/// </summary>
	S_LOGOUT = 10,

#pragma endregion 登陆状态

#pragma region 其他

	/// <summary>
	/// 查看好友列表
	/// </summary>
	S_ROSTER = 20,

#pragma endregion 其他

#pragma region 文件传输

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
	S_FILE_TERMINATE,

#pragma endregion 文件传输

#pragma region 视频音频

#pragma region 纯音频

	/// <summary>
	/// Call请求
	/// lpstr RecieveJid; 
	/// </summary>
	S_AUDIO = 90,
	/// <summary>
	/// Call接受
	/// </summary>
	S_AUDIO_ACCEPT,

#pragma endregion 纯音频

#pragma region 会话协议

	/// <summary>
	/// Vcall请求
	/// lpstr RecieveJid;  int localWidth;  int localHeight; int remoteWidth;  int remoteHeight
	/// </summary>
	S_CALL = 100,

	/// <summary>
	/// 接受Vcall
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

#pragma endregion 会话协议

#pragma region Mute及音量控制
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

#pragma endregion Mute及音量控制

#pragma region Capture及Renderer
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

	//unused
	/// <summary>
	/// 重设捕获的视频数据
	/// int Width;  int Height;  int Interval;  int ColorSpace; 
	/// </summary>
	S_RESTART_CAPTURE,

	//unused
	/// <summary>
	/// 打开本地视频Renderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_LOCAL_RENDERER,

	//unused
	/// <summary>
	/// 打开对方视频Renderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_REMOTE_RENDERER,

	//unused
	/// <summary>
	/// 创建 Local Remote——SurfaceView
	/// long local;  long remote;
	/// </summary>
	S_REGISTER_SV,

#pragma endregion Capture及Renderer

#pragma region 多会话

	/// <summary>
	/// AddSession
	/// string jid
	/// </summary>
	S_ADD_SESSION,

	/// <summary>
	/// RemoveSession
	/// string roomid
	/// </summary>
	S_RM_SESSION,
#pragma endregion 多会话


#pragma endregion 视频音频

#pragma region 录音

	/// <summary>
	/// 开始录音
	/// </summary>
	S_RECORD = 130,

	/// <summary>
	/// 停止录音
	/// </summary>
	S_UNRECORD,

#pragma endregion 录音

#pragma region MUC
	/// <summary>
	/// 创建多人聊天室
	/// string room_jid_str
	/// </summary>
	S_JOIN_MUC = 150,

	/// <summary>
	/// 邀请对方加入多人聊天室
	/// string jid; string roomid
	/// </summary>
	S_INVITE_MUC,

	/// <summary>
	/// 离开多人聊天室
	/// string roomid
	/// </summary>
	S_LEAVE_MUC,

#pragma endregion MUC

};

class ThreadShareData
{
public:
	ThreadShareData(int string_count,int int_count, int long_count);
	~ThreadShareData();
	void WriteInt(int value,int pos);
	void WriteString(std::string strUtf8,int pos);
	void WriteAnciString(LPCSTR strAnci, int pos);
	void WriteLong(long value,int pos);
	int ReadInt(int pos);
	long ReadLong(long pos);
	std::string ReadString(int pos);

private:
	std::string *string_array_ = NULL;
	int* int_array_ = NULL;
	long* long_array_ = NULL;
};

//edtied 各平台各自实现
extern void MyPrint(std::string str);

//edited MultiPlatformsCallBack，各平台各自实现
extern void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data);

#endif //PUBLIC_CALL_BACK_FUNCTION_H_
