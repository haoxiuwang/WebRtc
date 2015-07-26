#define _CRT_SECURE_NO_DEPRECATE 1

#include <assert.h>

//#ifdef WEBRTC_POSIX
#include <signal.h>
#include <termios.h>
#include <unistd.h>
//#endif  // WEBRTC_POSIX

#include "webrtc/libjingle/examples/call/console.h"


#ifdef WEBRTC_POSIX
static void DoNothing(int unused) {}
#endif

Console* console_ = NULL;

Console::Console(rtc::Thread *thread, CallClient *client) {
	  client_ = client;
	  client_thread_ = thread;
	  console_ = this;
}

Console::~Console() {}

//edited
rtc::Thread* Console::GetClientThread(){
return 	client_thread_;
}
