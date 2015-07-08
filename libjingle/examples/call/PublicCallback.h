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

// ����XmppЭ��
	/// <summary>
	/// �յ���libjingle�����ת����XmppЭ��
	/// lpstr XmppProtocol;  int ByteLength
	/// </summary> 
	R_XMPP_PROTOCOL = 0,

//  ����XmppЭ��End

// ��½״̬

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

// ��½״̬End

// �ļ�����

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

// �ļ�����End

// ��Ƶ��Ƶ

// �ỰЭ��

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

// �ỰЭ��End

	/// <summary>
	/// �Է�����Զ������
	/// </summary>
	R_SCREENCAST,

	/// <summary>
	/// �Է���������ͷ����
	/// </summary>
	R_CAMERA,

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
        /// </summary>
        R_LOCAL_FRAME_COMING = 115,

        /// <summary>
        ///  ��öԷ�����Ƶ֡
        /// </summary>
        R_REMOTE_FRAME_COMING,


// ��Ƶ��ƵEnd

	///<summary>
	///������Ϣ	lpwstr info;	lpwstr content;
	///</summary>
	R_DEBUG_INFO = 130
};

enum Enum_SendMessage
{
// ����XmppЭ��

	/// <summary>
	/// ����libjingle�����ת��xmppЭ��
	/// lpstr XmppProtocl
	/// </summary>
	S_XMPPP_ROTOCOL = 0,

// ����XmppЭ��End

// ��½״̬

	/// <summary>
	/// �ӷ������ǳ�
	/// </summary>
	S_LOGOUT = 10,

// ��½״̬End

// �ļ�����

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
	S_FILE_TERMINTATE,

// �ļ�����End

// ��Ƶ��Ƶ

// �ỰЭ��

	/// <summary>
	/// Call����
	/// lpstr RecieveJid;  int localWidth;  int localHeight; int remoteWidth;  int remoteHeight
	/// </summary>
	S_CALL = 100,

	/// <summary>
	/// ����Call
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

// �ỰЭ��End

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

	/// <summary>
	/// ���貶�����Ƶ����
	/// int Width;  int Height;  int Interval;  int ColorSpace; 
	/// </summary>
	S_RESTART_CAPTURE,

	/// <summary>
	/// �򿪱�����ƵRenderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_LOCAL_RENDERER,

	/// <summary>
	/// �򿪶Է���ƵRenderer
	/// int Width;  int Height; void* parentHandle
	/// </summary>
	S_OPEN_REMOTE_RENDERER,

// ��Ƶ��ƵEnd

// ¼��

	/// <summary>
	/// ��ʼ¼��
	/// </summary>
	S_RECORD = 150,

	/// <summary>
	/// ֹͣ¼��
	/// </summary>
	S_UNRECORD,

// ¼��End

	///<summary>
	///������Ϣ	lpwstr info;	lpwstr content;
	///</summary>
	S_DEBUG_INFO,

//���� Local Remote����SurfaceView
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

//��ȡ���߳���ص�Env //�ݷ�
extern JNIEnv* GetThreadEnv(bool thread);

//JAVA_VM
extern JavaVM* g_vm;

//WH����Java CallBack�ӿں���
extern void NewDayCallback(Enum_RecievedMessage command, ThreadShareData* data);
extern void callBack_Test_Method(std::string s);

#endif
