/*
 * libjingle
 * Copyright 2004--2005, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _CRT_SECURE_NO_DEPRECATE 1

#if defined(WEBRTC_POSIX)
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#endif  // WEBRTC_POSIX
#include <cassert>
#include "webrtc/libjingle/examples/call/console_dll.h"
#include "webrtc/libjingle/examples/call/CommonUtilities.h"

#ifdef WEBRTC_POSIX
static void DoNothing(int unused) {}
#endif

Console_Dll::Console_Dll(rtc::Thread *thread, CallClient *client) :
  Console(thread,client){
}

Console_Dll::~Console_Dll() {
  Stop();
}

void Console_Dll::Start() {}

void Console_Dll::Stop() {}

void Console_Dll::SetEcho(bool on) {}

void Console_Dll::PrintLine(const char* format, ...) {}

void Console_Dll::RunConsole() {}

void Console_Dll::SendCommand(int command, ThreadShareData* shareData) {
	this->GetClientThread()->Post(this, MSG_INPUT, new CommandData(command,shareData));
}

void Console_Dll::OnMessage(rtc::Message *msg) {
CommandData* cd = (CommandData*)msg->pdata;

  switch (msg->message_id) {
    case MSG_INPUT:
    client_->ParseLine(cd->command(),cd->data());
    break;
  }
delete cd;
}
