
#ifndef WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_WIN_H_
#define WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_WIN_H_
#include "talk/base/win32.h"
#include "webrtc/libjingle/examples/call/CommonUtilities.h"

typedef void (WINAPI *WIN32Callback)(Enum_RecievedMessage,void**);
extern WIN32Callback Win32Callback;

extern "C" _declspec(dllexport) void __stdcall Login(void** data);
extern "C" _declspec(dllexport) void __stdcall SendCommand(int command,void** data);
extern "C" _declspec(dllexport) void __stdcall RegisterCallback(WIN32Callback callback);

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
};

#endif  // WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_WIN_H_
