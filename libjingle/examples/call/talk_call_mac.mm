#include "webrtc/libjingle/examples/call/talk_call.h"
#include "webrtc/libjingle/examples/call/talk_call_mac.h"

void* ocArdCallback = NULL;

extern void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data);
void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data) {
	//empty
}

void Logout() {
	SendCommand((int)S_LOGOUT, NULL);
}

//Xmpp
void SendXmpp(int command, std::string xmppData) {
	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(xmppData, 0);

	SendCommand((int)S_XMPPP_ROTOCOL, shareData);
}

//P2P File
void SendFileRequest(std::string toJid, std::string fileFullPath, std::string fileDesc) {
	ThreadShareData* shareData = new ThreadShareData(3, 0, 0);
	shareData->WriteString(toJid, 0);
	shareData->WriteString(fileFullPath, 1);
	shareData->WriteString(fileDesc, 2);

	SendCommand((int)S_FILE_REQUEST, shareData);
}

void SendFileAccept(std::string sId, std::string fileName, std::string savePath) {
	ThreadShareData* shareData = new ThreadShareData(3, 0, 0);

	shareData->WriteString(sId, 0);
	shareData->WriteString(fileName, 1);
	shareData->WriteString(savePath, 2);

	SendCommand((int)S_FILE_ACCEPT, shareData);
}

void SendFileReject(std::string sId) {
	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(sId, 0);

	SendCommand((int)S_FILE_REJECT, shareData);
}

void SendFileTermintate(std::string sId) {
	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(sId, 0);

	SendCommand((int)S_FILE_TERMINTATE, shareData);
}

//Call
void SendCall(std::string toJid, int local_Width, int local_Height, int remote_Width, int remote_Height) {
	ThreadShareData* shareData = new ThreadShareData(1, 4, 0);
	shareData->WriteString(toJid, 0);

	shareData->WriteInt(local_Width, 0);
	shareData->WriteInt(local_Height, 1);
	shareData->WriteInt(remote_Width, 2);
	shareData->WriteInt(remote_Height, 3);

	SendCommand((int)S_CALL, shareData);
}

void SendCallAccept(int local_Width, int local_Height, int remote_Width, int remote_Height) {
	ThreadShareData* shareData = new ThreadShareData(0, 4, 0);
	shareData->WriteInt(local_Width, 0);
	shareData->WriteInt(local_Height, 1);
	shareData->WriteInt(remote_Width, 2);
	shareData->WriteInt(remote_Height, 3);

	SendCommand((int)S_CALL_ACCEPT, shareData);
}

void SendCallReject() {
	SendCommand((int)S_CALL_REJECT, NULL);
}

void SendScreencast_Start() {
	SendCommand((int)S_START_SCREENCAST, NULL);
}

void SendCameraStart() {
	SendCommand((int)S_START_CAMERA, NULL);
}

void SendCameraStop() {
	SendCommand((int)S_STOP_CAPTURE, NULL)
}

void SendCameraRestart(int width, int height, int interval, int colorspace) {
	ThreadShareData* shareData = new ThreadShareData(0, 4, 0);
	shareData->WriteInt(width, 0);
	shareData->WriteInt(height, 1);
	shareData->WriteInt(interval, 2);
	shareData->WriteInt(colorspace, 3);

	SendCommand((int)S_RESTART_CAPTURE, shareData);
}

void SendOpenLocalRenderer(int videoWidth, int videoHeight, int parentHandle) {
	ThreadShareData* shareData = new ThreadShareData(0, 3, 0);
	shareData->WriteInt(videoWidth, 0);
	shareData->WriteInt(videoHeight, 1);
	shareData->WriteInt(parentHandle, 2);

	SendCommand((int)S_OPEN_LOCAL_RENDERER, shareData);
}

void SendOpenRemoteRenderer(int videoWidth, int videoHeight, int parentHandle) {
	ThreadShareData* shareData = new ThreadShareData(0, 3, 0);
	shareData->WriteInt(videoWidth, 0);
	shareData->WriteInt(videoHeight, 1);
	shareData->WriteInt(parentHandle, 2);

	SendCommand((int)S_OPEN_REMOTE_RENDERER, shareData);
}

void SendMute() {
	SendCommand((int)S_MUTE, NULL);
}

void SendUnMute() {
	SendCommand((int)S_UNMUTE, NULL);
}

void SendVideoMute() {
	SendCommand((int)S_VIDEO_MUTE, NULL);
}

void SendVideoUnmute() {
	SendCommand((int)S_VIDEO_UNMUTE, NULL);
}

void SendSetRemoteVolume(std::string volume) {
	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(volume, 0);

	SendCommand((int)S_SET_REMOTE_VOLUME, shareData);
}

void SendRecord() {
	SendCommand((int)S_RECORD, NULL);
}

void SendUnRecord() {
	SendCommand((int)S_UNRECORD, NULL);
}

void SendHangUp() {
	SendCommand((int)S_HANG_UP, NULL);
}

//Added
void SetARDP2PCallback(void* callback){
	ocArdCallback = callback;
}

