#include "webrtc/libjingle/examples/call/PublicCallback.h"

void MyPrint(std::string str) {
#if defined(TALK_CALL_CONSOLE_) || defined(WEBRTC_IOS)
	printf("%s", chars);
	fflush(stdout);
#endif
}

ThreadShareData::ThreadShareData(int string_count, int int_count, int long_count) {
	if(string_count > 0)
		string_array_ = new std::string[string_count];
	if(int_count > 0)
		int_array_ = new int[int_count];
	if(long_count > 0)
		long_array_ = new long[long_count];
}

ThreadShareData::~ThreadShareData() {
	delete[] int_array_;
	int_array_ = NULL;

	delete[] long_array_;
	long_array_ = NULL;

	delete[] string_array_;
	string_array_ = NULL;
}

void ThreadShareData::WriteInt(int value, int index) {
	*(int_array_ + index) = value;
}

void ThreadShareData::WriteString(std::string strUtf8, int index) {
	*(string_array_ + index) = strUtf8;
}

void ThreadShareData::WriteAnciString(LPCSTR strAnci, int index){
	LPWSTR wstr = AnsiToWchar(strAnci);
	WriteString(std::string(WcharToUtf8(wstr)), index);
	::HeapFree(GetProcessHeap(),0,wstr);
}

void ThreadShareData::WriteLong(long value, int index) {
	*(long_array_ + index) = value;
}

int ThreadShareData::ReadInt(int index)
{
	return *(int_array_ + index);
}

long ThreadShareData::ReadLong(long index) 
{
	return *(long_array_ + index);
}

std::string ThreadShareData::ReadString(int index) 
{	
	return *(string_array_ + index);
}

#endif
