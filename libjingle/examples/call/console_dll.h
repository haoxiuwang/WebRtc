#pragma once
#ifndef TALK_EXAMPLES_CALL_CONSOLE_IMP_H_
#define TALK_EXAMPLES_CALL_CONSOLE_IMP_H_

#include "webrtc/base/logging.h"
#include "webrtc/base/messagequeue.h"
#include "webrtc/libjingle/examples/call/console.h"
#include "webrtc/libjingle/examples/call/callclient.h"
#include "webrtc/libjingle/examples/call/PublicCallback.h"

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

private:
	rtc::scoped_ptr<rtc::Thread> console_thread_;
};

#endif//TALK_EXAMPLES_CALL_CONSOLE_IMP_H_
