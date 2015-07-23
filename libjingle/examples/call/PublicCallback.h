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

#pragma region ����XmppЭ��
	/// <summary>
	/// �յ���libjingle�����ת����XmppЭ��
	/// lpstr XmppProtocol;  int ByteLength
	/// </summary> 
	R_XMPP_PROTOCOL = 0,

#pragma endregion  ����XmppЭ��

#pragma region ��½״̬

	/// <summary>
	/// ���ڵ�½
	/// </summary>
	R_LOGGING = 10,

	/// <summary>
	/// ����ɹ������ش���Դ��Jid
	/// lpwstr FullJid
	/// </summary> 
	R_LOGGED_IN,

	/// <summary>
	/// �ӷ������ǳ�
	/// </summary>
	R_LOGGED_OUT,

#pragma endregion ��½״̬

#pragma region �ļ�����

	/// <summary>
	/// �ļ����ͷ��Ѵ���Session���������SessionId
	/// lpwstr SessionId;  lpwstr Jid;  lpwstr FullFileName;
	/// </summary>       
	R_FILE_SESSION_ID = 50,

	/// <summary>
	/// �Է��򼺷������ļ�
	/// lpwstr SessionId;  lpwstr Jid;  lpwstr FileDescription;
	/// </summary>
	R_FILE_REQUEST,

	/// <summary>
	/// �Է������ļ�
	/// lpwstr SessionId;
	/// </summary>
	R_FILE_ACCEPT,

	/// <summary>
	/// �Է��ܾ��ļ�
	/// lpwstr SessionId;
	/// </summary>
	R_FILE_REJECT,

	/// <summary>
	/// �Է���ֹ�����ļ�
	/// lpwstr SessionId;
	/// </summary>
	R_FILE_TERMINATE,

	/// <summary>
	/// ��ȡ�ļ�����
	/// lpwstr SessionId;  int Progress
	/// </summary>
	R_FILE_PROGRESS,

#pragma endregion �ļ�����

#pragma region ��Ƶ��Ƶ

#pragma region �ỰЭ��

	/// <summary>
	/// �Է�����Call����
	/// lpwstr Jid;
	/// </summary>
	R_CALL = 100,

	/// <summary>
	/// �Է�����Call
	/// </summary>
	R_CALL_ACCEPT,

	/// <summary>
	/// �Է��ܾ�Call
	/// </summary>
	R_CALL_REJECT,

	/// <summary>
	/// �Է���ֹCall
	/// </summary>
	R_CALL_TERMINATE,

#pragma endregion �ỰЭ��

#pragma region Capture��Frame
	/// <summary>
	/// �Է�����Զ������
	/// </summary>
	R_SCREENCAST,

	/// <summary>
	/// �Է���������ͷ����
	/// </summary>
	R_CAMERA,

	/// <summary>
	/// �Է��ص���Ƶ����
	/// </summary>
	R_STOP_CAPTURE,

	/// <summary>
	/// �Ѵ���������ƵRenderer
	/// ���ھ�� void* handle 
	/// </summary>
	R_LOCAL_RENDERER_HANDLE,

	/// <summary>
	/// �Ѵ����Է���ƵRenderer
	/// ���ھ�� void* handle
	/// </summary>
	R_REMOTE_RENDERER_HANDLE,

	/// <summary>
	///  ��ñ��ص���Ƶ֡
	///  void* ֡buffer int width int height
	/// </summary>
	R_LOCAL_FRAME_METADATA,

	/// <summary>
	///  ��öԷ�����Ƶ֡
	///  void* ֡buffer int width int height
	/// </summary>
	R_REMOTE_FRAME_METADATA,

	/// <summary>
	///  ��ñ��ص���Ƶ֡
	/// </summary>
	R_LOCAL_FRAME_COMING = 115,

	/// <summary>
	///  ��öԷ�����Ƶ֡
	/// </summary>
	R_REMOTE_FRAME_COMING,

#pragma endregion Capture��Frame

#pragma endregion ��Ƶ��Ƶ

	///<summary>
	///������Ϣ	lpwstr info;	lpwstr content;
	///</summary>
	R_DEBUG_INFO = 130
};

enum Enum_SendMessage
{

#pragma region ����XmppЭ��

	/// <summary>
	/// ����libjingle�����ת��xmppЭ��
	/// lpstr XmppProtocl
	/// </summary>
	S_XMPP_PROTOCOL = 0,

#pragma endregion ����XmppЭ��

#pragma region ����STUN��������ַ
	/// <summary>
	/// ����STUN������IP��ַ�Ͷ˿�
	/// lpstr stun_ip_addr; int stun_port;
	/// </summary>
	S_STUN_ADDRESS = 5,

#pragma endregion ����STUN��������ַ

#pragma region ��½״̬

	/// <summary>
	/// �ӷ������ǳ�
	/// </summary>
	S_LOGOUT = 10,

#pragma endregion ��½״̬

#pragma region ����

	/// <summary>
	/// �鿴�����б�
	/// </summary>
	S_ROSTER = 20,

#pragma endregion ����

#pragma region �ļ�����

	/// <summary>
	/// �����ļ�
	/// lpstr RecieverJid;  lpstr FullFileName;  lpstr FileDescription;
	/// </summary>
	S_FILE_REQUEST = 50,

	/// <summary>
	/// �����ļ�
	/// lpstr SessionId;  lpstr FileName;  lpstr SavePath;
	/// </summary>
	S_FILE_ACCEPT,

	/// <summary>
	/// �ܾ������ļ�
	/// lpstr SessionId;
	/// </summary>
	S_FILE_REJECT,

	/// <summary>
	/// ��ֹ�����ļ�
	/// lpstr SessionId;
	/// </summary>
	S_FILE_TERMINATE,

#pragma endregion �ļ�����

#pragma region ��Ƶ��Ƶ

#pragma region ����Ƶ

	/// <summary>
	/// Call����
	/// lpstr RecieveJid; 
	/// </summary>
	S_AUDIO = 90,
	/// <summary>
	/// Call����
	/// </summary>
	S_AUDIO_ACCEPT,

#pragma endregion ����Ƶ

#pragma region �ỰЭ��

	/// <summary>
	/// Vcall����
	/// lpstr RecieveJid;  int localWidth;  int localHeight; int remoteWidth;  int remoteHeight
	/// </summary>
	S_CALL = 100,

	/// <summary>
	/// ����Vcall
	/// int localWidth;  int localHeight; int remoteWidth;  int remoteHeight
	/// </summary>
	S_CALL_ACCEPT,

	/// <summary>
	/// �ܾ�Call
	/// </summary>
	S_CALL_REJECT,

	/// <summary>
	/// ��ֹ��ɾ���Ự
	/// </summary>
	S_HANG_UP,

#pragma endregion �ỰЭ��

#pragma region Mute����������
	/// <summary>
	/// ֹͣ��������Ƶ����
	/// </summary>
	S_MUTE,

	/// <summary>
	/// ���¿�ʼ��������Ƶ����
	/// </summary>
	S_UNMUTE,

	/// <summary>
	/// ���ƶԷ������������Ƶ����
	/// lpstr Volume
	/// </summary>
	S_SET_REMOTE_VOLUME,

	/// <summary>
	/// ֹͣ��������Ƶ����
	/// </summary>
	S_VIDEO_MUTE,

	/// <summary>
	/// ���¿�ʼ��������Ƶ����
	/// </summary>
	S_VIDEO_UNMUTE,

#pragma endregion Mute����������

#pragma region Capture��Renderer
	/// <summary>
	/// ����Զ������
	/// </summary>
	S_START_SCREENCAST,

	/// <summary>
	/// ��������ͷ����
	/// </summary>
	S_START_CAMERA,

	/// <summary>
	/// ֹͣ������Ƶ
	/// </summary>
	S_STOP_CAPTURE,

	//unused
	/// <summary>
	/// ���貶�����Ƶ����
	/// int Width;  int Height;  int Interval;  int ColorSpace; 
	/// </summary>
	S_RESTART_CAPTURE,

	//unused
	/// <summary>
	/// �򿪱�����ƵRenderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_LOCAL_RENDERER,

	//unused
	/// <summary>
	/// �򿪶Է���ƵRenderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_REMOTE_RENDERER,

	//unused
	/// <summary>
	/// ���� Local Remote����SurfaceView
	/// long local;  long remote;
	/// </summary>
	S_REGISTER_SV,

#pragma endregion Capture��Renderer

#pragma region ��Ự

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
#pragma endregion ��Ự


#pragma endregion ��Ƶ��Ƶ

#pragma region ¼��

	/// <summary>
	/// ��ʼ¼��
	/// </summary>
	S_RECORD = 130,

	/// <summary>
	/// ֹͣ¼��
	/// </summary>
	S_UNRECORD,

#pragma endregion ¼��

#pragma region MUC
	/// <summary>
	/// ��������������
	/// string room_jid_str
	/// </summary>
	S_JOIN_MUC = 150,

	/// <summary>
	/// ����Է��������������
	/// string jid; string roomid
	/// </summary>
	S_INVITE_MUC,

	/// <summary>
	/// �뿪����������
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

//edtied ��ƽ̨����ʵ��
extern void MyPrint(std::string str);

//edited MultiPlatformsCallBack����ƽ̨����ʵ��
extern void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data);

#endif //PUBLIC_CALL_BACK_FUNCTION_H_
