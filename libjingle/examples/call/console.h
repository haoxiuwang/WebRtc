#ifndef WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_H_
#define WEBRTC_LIBJINGLE_EXAMPLES_CALL_CONSOLE_H_

#include <stdio.h>

#include "webrtc/base/logging.h"
#include "webrtc/base/messagequeue.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/libjingle/examples/call/callclient.h"
#include "webrtc/libjingle/examples/call/PublicCallback.h"

class CallClient;

class Console : public rtc::MessageHandler,public sigslot::has_slots<>{
 public:
  Console(rtc::Thread *thread, CallClient *client);
  ~Console();

  // Starts reading lines from the console and giving them to the CallClient.
  virtual void Start() = 0;
  // Stops reading lines. Cannot be restarted.
  virtual void Stop() = 0;

  virtual void OnMessage(rtc::Message *msg) = 0;

  virtual void PrintLine(const char* format, ...) = 0;

  //edited
  rtc::Thread* GetClientThread();
  rtc::scoped_ptr<rtc::Thread> console_thread_;

 protected:
  enum {
    MSG_START,
    MSG_INPUT,
  };

  virtual void RunConsole() = 0;

  CallClient *client_;
  rtc::Thread *client_thread_;

};

//edited win32
extern Console* console_;
#endif // TALK_EXAMPLES_CALL_CONSOLE_H_
