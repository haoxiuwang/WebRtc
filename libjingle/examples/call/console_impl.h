#pragma once
#ifndef WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_IMPL_H_
#define WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_IMPL_H_

#include "webrtc/libjingle/examples/call/console.h"

class Console_Impl : public Console{
public:
	Console_Impl(rtc::Thread *thread, CallClient *client);
	~Console_Impl();

	// Starts reading lines from the console and giving them to the CallClient.
	void Start();
	// Stops reading lines. Cannot be restarted.
	void Stop();

	void OnMessage(rtc::Message *msg);

	void PrintLine(const char* format, ...);
	void WPrintLine(const wchar_t* format, ...);
	static void SetEcho(bool on);

protected:
	 void ParseLine(std::string line);
	 LPCSTR Getstr(const std::vector<std::string>& words,size_t index, const std::string& def);
	 void Client_ParseLine(int command,const std::vector<std::string>& words,size_t index);
	 void SplitStanza(const std::string& str, std::vector<std::string>& ret_, std::string sep);
	 void RunConsole();
	 /*void ParseLine(std::string &str);*/

private:
	bool isTaskStart_;
	Enum_SendMessage current_msg_;
	int next_;

};

#endif//WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_IMPL_H_