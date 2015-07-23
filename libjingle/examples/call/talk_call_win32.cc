#include "webrtc/libjingle/examples/call/talk_call.h"
#include "webrtc/libjingle/examples/call/talk_call_win32.h"

WIN32Callback Win32Callback = NULL;

void __stdcall Login(void** data) {
	ManagedDataReader reader(data);
	std::string username = reader.ReadString();
	std::string pasword = reader.ReadString();
	std::string res = reader.ReadString();
	reader.ReadToEnd();

	Login(username, pasword, res);
}

void __stdcall SendCommand(int command,void** data){
	ManagedDataReader reader(data);
	ThreadShareData* shareData = NULL;

	if(data != NULL){
		if(command == S_XMPP_PROTOCOL || command == S_FILE_REJECT || command == S_FILE_TERMINATE 
			|| command == S_JOIN_MUC || command == S_LEAVE_MUC || command == S_AUDIO 
			|| command == S_SET_REMOTE_VOLUME || command == S_ADD_SESSION || command == S_RM_SESSION)
		{
			shareData = new ThreadShareData(1, 0, 0);
			shareData->WriteString(reader.ReadString(), 0);
		}
		else if(command == S_STUN_ADDRESS)
		{
			shareData = new ThreadShareData(1, 1, 0);
			shareData->WriteString(reader.ReadString(), 0);
			shareData->WriteInt(reader.ReadInt(), 0);
		}
		else if(command == S_FILE_REQUEST || command == S_FILE_ACCEPT)
		{
			shareData = new ThreadShareData(3, 0, 0);
			shareData->WriteString(reader.ReadString(), 0);
			shareData->WriteString(reader.ReadString(), 1);
			shareData->WriteString(reader.ReadString(), 2);
		}
		else if (command == S_INVITE_MUC) 
		{
			shareData = new ThreadShareData(2, 0, 0);
			shareData->WriteString(reader.ReadString(), 0);
			shareData->WriteString(reader.ReadString(), 1);
		} 
		else if(command == S_CALL)
		{
			shareData = new ThreadShareData(1, 4, 0);
			shareData->WriteString(reader.ReadString(), 0);
			shareData->WriteInt(reader.ReadInt(), 0);
			shareData->WriteInt(reader.ReadInt(), 1);
			shareData->WriteInt(reader.ReadInt(), 2);
			shareData->WriteInt(reader.ReadInt(), 3);
		} 
		else if(command == S_CALL_ACCEPT)
		{
			shareData = new ThreadShareData(0, 4, 0);
			shareData->WriteInt(reader.ReadInt(), 0);
			shareData->WriteInt(reader.ReadInt(), 1);
			shareData->WriteInt(reader.ReadInt(), 2);
			shareData->WriteInt(reader.ReadInt(), 3);
		}
		SendCommand(command, shareData);
	}
	else
		SendCommand(command, NULL);

	reader.ReadToEnd();
}

void __stdcall RegisterCallback(WIN32Callback callback){
	Win32Callback = callback;
}

extern void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data);
void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data) {
	if(data != NULL){
		if(command == R_XMPP_PROTOCOL){
			ManagedDataWriter writer(2);
			writer.WritePointer((void*)data->ReadLong(0));
			writer.WriteInt(data->ReadInt(0));
		}
		else if(command == R_LOGGED_IN || command == R_FILE_ACCEPT || command == R_FILE_REJECT ||
			command == R_FILE_TERMINATE || command == R_CALL){
				ManagedDataWriter writer(1);
				writer.WriteString(data->ReadString(0).c_str());
		}
		else if(command == R_FILE_SESSION_ID || command == R_FILE_REQUEST){
			ManagedDataWriter writer(3);
			writer.WriteString(data->ReadString(0).c_str());
			writer.WriteString(data->ReadString(1).c_str());
			writer.WriteString(data->ReadString(2).c_str());
		}
		else if(command == R_FILE_PROGRESS){
			ManagedDataWriter writer(2);
			writer.WriteString(data->ReadString(0).c_str());
			writer.WriteInt(data->ReadInt(0));
		}
		else if(command == R_LOCAL_FRAME_METADATA || command == R_REMOTE_FRAME_METADATA){
			ManagedDataWriter writer(3);
			writer.WritePointer((void*)data->ReadLong(0));
			writer.WriteInt(data->ReadInt(0));
			writer.WriteInt(data->ReadInt(1));
		}
		else if(command == R_LOCAL_RENDERER_HANDLE || command == R_REMOTE_RENDERER_HANDLE){
			ManagedDataWriter writer(1);
			writer.WritePointer((void*)data->ReadLong(0));
		}
		else if(command == R_DEBUG_INFO){
			ManagedDataWriter writer(2);
			writer.WriteString(data->ReadString(0).c_str());
			writer.WriteString(data->ReadString(1).c_str());
		}

		Win32Callback(command, writer.ToArray());
		delete data;
	}
	else
		Win32Callback(command, NULL);
}

ManagedDataReader::ManagedDataReader(void** pointer): _offset(0), _pointer(pointer){}

int ManagedDataReader::ReadInt()
{
	int i = (int)*(_pointer + _offset);
	_pointer++;
	return i;
}

std::string ManagedDataReader::ReadString()
{
	std::string str((char*)*(_pointer + _offset));
	::HeapFree(::GetProcessHeap(),0,*(_pointer + _offset));
	_offset++;
	return str;
}

std::wstring ManagedDataReader::ReadUnicode()
{
	std::wstring str((wchar_t*)*(_pointer + _offset));
	::HeapFree(::GetProcessHeap(),0,*(_pointer + _offset));
	_offset++;
	return str;
}

LPCSTR ManagedDataReader::ReadString_s()
{
	return this->ReadString().c_str();
}

void ManagedDataReader::ReadToEnd()
{	
	if(_pointer)
	{
		try
		{
			::HeapFree(::GetProcessHeap(),0,_pointer);
			_pointer=NULL;
		}
		catch (void* e)
		{
			return;
		}
	}
}

ManagedDataWriter::ManagedDataWriter(int m_count): offset_(0), pointer_(NULL)
{
	pointer_=(void**)::HeapAlloc(::GetProcessHeap(),0,sizeof(int)*m_count);
}

void ManagedDataWriter::WriteString(LPCSTR strUtf8)
{
	*(pointer_+ offset_) = Utf8ToWchar(strUtf8);
	offset_++;
}

void ManagedDataWriter::WriteAnciString(LPCSTR strAnci)
{
	LPWSTR wstr = AnsiToWchar(strAnci);
	*(pointer_+ offset_) = WcharToUtf8(wstr);
	::HeapFree(GetProcessHeap(),0,wstr);
	offset_++;
}

void ManagedDataWriter::WriteUnicodeToUtf8(LPWSTR strUnicode)
{
	*(pointer_+ offset_) = WcharToUtf8(strUnicode);
	offset_++;
}

void ManagedDataWriter::WriteInt(int value)
{
	*(pointer_+offset_) = (void*)value;
	offset_++;
}

void ManagedDataWriter::WritePointer(void* pointer)
{
	WriteInt((int)pointer);
}

void** ManagedDataWriter::ToArray()
{
	return pointer_;
}
