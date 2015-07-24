#ifndef WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_H_
#define WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_H_


#include <stdio.h>
#include <string.h>
#include <time.h>

#include <iomanip>
#include <iostream>
#include <vector>

#include "webrtc/base/flags.h"
#include "webrtc/base/logging.h"
#ifdef OSX
#include "webrtc/base/maccocoasocketserver.h"
#endif
#include "webrtc/libjingle/examples/call/callclient.h"
//#include "webrtc/libjingle/examples/call/mediaenginefactory.h"
#include "talk/session/media/srtpfilter.h"
#include "webrtc/base/pathutils.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/stream.h"
#include "webrtc/base/win32socketserver.h"
#include "webrtc/libjingle/session/media/mediasessionclient.h"
#include "webrtc/libjingle/xmpp/xmppauth.h"
#include "webrtc/libjingle/xmpp/xmppclientsettings.h"
#include "webrtc/libjingle/xmpp/xmpppump.h"
#include "webrtc/libjingle/xmpp/xmppsocket.h"
#include "webrtc/p2p/base/constants.h"

//edited crossplataforms SendCommand
extern void SendCommand(int command, ThreadShareData* data);

//edited crossplatforms Login
extern void Login(std::string usr, std::string pw, std::string res);
#endif //WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_H_