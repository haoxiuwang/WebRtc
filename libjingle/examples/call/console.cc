#define _CRT_SECURE_NO_DEPRECATE 1

#include <assert.h>

//#ifdef POSIX
#include <signal.h>
#include <termios.h>
#include <unistd.h>
//#endif  // POSIX

#include "webrtc/libjingle/examples/call/console.h"


#ifdef POSIX
static void DoNothing(int unused) {}
#endif

Console* console_ = NULL;

Console::Console(rtc::Thread *thread, CallClient *client) :{
	  client_ = client;
	  client_thread_ = thread;
	  console_ = this;
	  console_thread_ = new talk_base::Thread();
}

Console::~Console() {}

//edited
rtc::Thread* Console::GetClientThread(){
return 	client_thread_;
}
