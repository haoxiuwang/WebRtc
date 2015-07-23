
#ifndef WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_MAC_H_
#define WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_MAC_H_

#include <string>

#if defined(WEBRTC_IOS)
#import "webrtc/modules/video_render/ios/video_render_ios_view.h"
#elif defined(WEBRTC_MAC)
#import "webrtc/modules/video_render/mac/cocoa_render_view.h"
#endif

//class CocoaRenderView;
//class VideoRenderIosView;

namespace cricket {

#if defined(WEBRTC_IOS)
	extern VideoRenderIosView* IOSLocalView_;
	extern VideoRenderIosView* IOSRemoteView_;
#elif defined(WEBRTC_MAC)
	extern CocoaRenderView* CocoaLocalView_;
	extern CocoaRenderView* CocoaRemoteView_;
#endif

}

extern void Logout();

//Xmpp
extern void SendXmpp(std::string usr, std::string pw);

//P2P File
extern void SendFileRequest(std::string toJid, std::string fileFullPath, std::string fileDesc);

extern void SendFileAccept(std::string sId, std::string fileName, std::string savePath);

extern void SendFileReject(std::string sId);

extern void SendFileTermintate(std::string sId);

//Call
extern void SendCall(std::string toJid, int local_Width, int local_Height, int remote_Width, int remote_Height);

extern void SendCallAccept(int local_Width, int local_Height, int remote_Width, int remote_Height);

extern void SendCallReject();

extern void SendScreencast_Start();

extern void SendCameraStart();

extern void SendCameraStop();

extern void SendCameraRestart(int width, int height, int interval, int colorspace);

extern void SendOpenLocalRenderer(int videoWidth, int videoHeight, int parentHandle);

extern void SendOpenRemoteRenderer(int videoWidth, int videoHeight, int parentHandle);

extern void SendMute();

extern void SendUnMute();

extern void SendVideoMute();

extern void SendVideoUnmute();

extern void SendSetRemoteVolume(std::string volume);

extern void SendRecord();

extern void SendUnRecord();

extern void SendHangUp();

//Added
extern void* ocArdCallback;
extern void SetARDP2PCallback(void* callback);

#endif  // WEBRTC_LIBJINGLE_EXAMPLES_CALL_TALK_CALL_MAC_H_
