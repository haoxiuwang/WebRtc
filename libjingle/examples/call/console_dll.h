#pragma once
#ifndef WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_DLL_H_
#define WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_DLL_H_

#include "webrtc/libjingle/examples/call/console.h"

class CommandData : public rtc::MessageData 
{
public:
	explicit CommandData(int com,ThreadShareData* data) : command_(com),data_(data) { }
	int command() { return command_; }
	ThreadShareData* data(){ return data_; }
private:
	int command_;
	ThreadShareData* data_;
};

class Console_Dll : public Console{
public:
	Console_Dll(rtc::Thread *thread, CallClient *client);
	~Console_Dll();

	// Starts reading lines from the console and giving them to the CallClient.
	void Start();
	// Stops reading lines. Cannot be restarted.
	void Stop();

	void OnMessage(rtc::Message *msg);

	void PrintLine(const char* format, ...);
	
	static void SetEcho(bool on);
	void SendCommand(int command, ThreadShareData* data);

protected:
	void RunConsole();
	void ParseLine(std::string &str);
};

#endif//WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_DLL_H_
