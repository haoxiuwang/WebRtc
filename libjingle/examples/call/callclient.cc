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

#include "webrtc/libjingle/examples/call/callclient.h"

//edited
#include <sstream>

#include <string>

#include "webrtc/libjingle/examples/call/friendinvitesendtask.h"
#include "webrtc/libjingle/examples/call/muc.h"
#include "webrtc/libjingle/examples/call/mucinviterecvtask.h"
#include "webrtc/libjingle/examples/call/mucinvitesendtask.h"
#include "webrtc/libjingle/examples/call/presencepushtask.h"
#include "talk/media/base/mediacommon.h"
#include "talk/media/base/mediaengine.h"
#include "talk/media/base/rtpdataengine.h"
#include "talk/media/base/screencastid.h"
#ifdef HAVE_SCTP
#include "talk/media/sctp/sctpdataengine.h"
#endif
#include "talk/media/base/videorenderer.h"
#include "talk/media/devices/devicemanager.h"
#include "webrtc/base/helpers.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/network.h"
#include "webrtc/base/socketaddress.h"
#include "webrtc/base/stringencode.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/windowpickerfactory.h"
#include "webrtc/libjingle/session/media/mediamessages.h"
#include "webrtc/libjingle/session/media/mediasessionclient.h"
#include "webrtc/libjingle/session/sessionmanager.h"
#include "webrtc/libjingle/xmpp/constants.h"
#include "webrtc/libjingle/xmpp/hangoutpubsubclient.h"
#include "webrtc/libjingle/xmpp/mucroomconfigtask.h"
#include "webrtc/libjingle/xmpp/mucroomlookuptask.h"
#include "webrtc/libjingle/xmpp/pingtask.h"
#include "webrtc/libjingle/xmpp/presenceouttask.h"
#include "webrtc/p2p/client/basicportallocator.h"
#include "webrtc/libjingle/session/sessionmanagertask.h"

//edited multi-platforms videorender

//mac ios apprtc_renderer
//#define WEBRTC_APPRTC_RENDERER



#if defined(WIN32)
#include "talk/media/devices/gdivideorenderer.h"
#elif defined(ANDROID)
#include "talk/media/devices/androidvideorenderer.h"
#include "webrtc/libjingle/examples/call/talk_call_android.h"
#elif defined(LINUX) && defined(HAVE_GTK)
#include "talk/media/devices/gtkvideorenderer.h"
#elif defined(WEBRTC_APPRTC_RENDERER)
#include "talk/media/devices/apprtcvideorenderer.h"
#elif defined(WEBRTC_IOS)
#include "talk/media/devices/iosvideorenderer.h"
#elif defined(WEBRTC_MACCONSOLE_RENDERER)
#include "talk/media/devices/macconsolevideorenderer.h"
#elif defined(WEBRTC_MAC)
#include "talk/media/devices/macvideorenderer.h"
#endif
//edited multi-platforms videorender end


namespace {

// Must be period >= timeout.
const uint32 kPingPeriodMillis = 10000;
const uint32 kPingTimeoutMillis = 10000;

const char* DescribeStatus(buzz::PresenceStatus::Show show,
                           const std::string& desc) {
  switch (show) {
  case buzz::PresenceStatus::SHOW_XA:      return desc.c_str();
  case buzz::PresenceStatus::SHOW_ONLINE:  return "online";
  case buzz::PresenceStatus::SHOW_AWAY:    return "away";
  case buzz::PresenceStatus::SHOW_DND:     return "do not disturb";
  case buzz::PresenceStatus::SHOW_CHAT:    return "ready to chat";
  default:                                 return "offline";
  }
}

}  // namespace

const char* CALL_COMMANDS =
"Available commands:\n"
"\n"
"  hangup            Ends the call.\n"
"  hold              Puts the current call on hold\n"
"  calls             Lists the current calls and their sessions\n"
"  switch [call_id]  Switch to the specified call\n"
"  addsession [jid]  Add a new session to the current call.\n"
"  rmsession [sid]   Remove specified session.\n"
"  mute              Stops sending voice.\n"
"  unmute            Re-starts sending voice.\n"
"  vmute             Stops sending video.\n"
"  vunmute           Re-starts sending video.\n"
"  dtmf              Sends a DTMF tone.\n"
"  stats             Print voice stats for the current call.\n"
"  quit              Quits the application.\n"
"";

// TODO: Make present and record really work.
const char* HANGOUT_COMMANDS =
"Available MUC commands:\n"
"\n"
"  present    Starts presenting (just signalling; not actually presenting.)\n"
"  unpresent  Stops presenting (just signalling; not actually presenting.)\n"
"  record     Starts recording (just signalling; not actually recording.)\n"
"  unrecord   Stops recording (just signalling; not actually recording.)\n"
"  rmute [nick] Remote mute another participant.\n"
"  block [nick] Block another participant.\n"
"  screencast [fps] Starts screencast. \n"
"  unscreencast Stops screencast. \n"
"  quit       Quits the application.\n"
"";

const char* RECEIVE_COMMANDS =
"Available commands:\n"
"\n"
"  accept [bw] Accepts the incoming call and switches to it.\n"
"  reject  Rejects the incoming call and stays with the current call.\n"
"  quit    Quits the application.\n"
"";

const char* CONSOLE_COMMANDS =
"Available commands:\n"
"\n"
"  roster              Prints the online friends from your roster.\n"
"  friend user         Request to add a user to your roster.\n"
"  call [jid] [bw]     Initiates a call to the user[/room] with the\n"
"                      given JID and with optional bandwidth.\n"
"  vcall [jid] [bw]    Initiates a video call to the user[/room] with\n"
"                      the given JID and with optional bandwidth.\n"
"  calls               Lists the current calls\n"
"  switch [call_id]    Switch to the specified call\n"
"  join [room_jid]     Joins a multi-user-chat with room JID.\n"
"  ljoin [room_name]   Joins a MUC by looking up JID from room name.\n"
"  invite user [room]  Invites a friend to a multi-user-chat.\n"
"  leave [room]        Leaves a multi-user-chat.\n"
"  nick [nick]         Sets the nick.\n"
"  priority [int]      Sets the priority.\n"
"  getdevs             Prints the available media devices.\n"
"  quit                Quits the application.\n"
"";

//edited
void CallClient::ParseLine(int command, ThreadShareData* data) 
{
	if(command == S_REGISTER_SV){
		//此命令暂不使用，已有代替路线
		//sv_local = reinterpret_cast<void*>(data->ReadLong(0));
		//sv_remote = reinterpret_cast<void*>(data->ReadLong(1));
	}
	else if (command == S_ROSTER) {
		PrintRoster();
	}
	else if(command == S_XMPP_PROTOCOL){
		xmpp_client_->engine()->SendRaw(data->ReadString(0));
	}
	else if(command == S_STUN_ADDRESS){
		stun_ip_addr = data->ReadString(0);
		stun_port = data->ReadInt(0);
	}
	else if(command == S_LOGOUT){
		Quit();
	}

	#pragma region P2P File
	
	else if(command == S_FILE_REQUEST){
		buzz::Jid toJid(data->ReadString(0));
		std::string fileFullPath = data->ReadString(1);
		std::string fileDesc = data->ReadString(2);

#if defined(WIN32)
		InitiateFileShare(toJid, fileFullPath, fileDesc);
#endif
	}
	else if(command == S_FILE_ACCEPT){
		std::string sId = data->ReadString(0);
		std::string fileName = data->ReadString(1);
		std::string savePath = data->ReadString(2);

#if defined(WIN32)
		AcceptOrRejectFileShare(true,sId,fileName,savePath);
#endif
	}
	else if(command == S_FILE_REJECT){
		std::string sId = data->ReadString(0);

#if defined(WIN32)
		AcceptOrRejectFileShare(false,sId,"","");
#endif
	}
	else if(command == S_FILE_TERMINATE){
		std::string sId = data->ReadString(0);

#if defined(WIN32)
		CallClient::FileProcessor* processor = GetFileProcessor(sId);
		processor->GetSession()->Terminate();
#endif
	}

#pragma endregion

	#pragma region Muc

	else if (command == S_JOIN_MUC) {
		JoinMuc(data->ReadString(0));
	} 
	else if (command == S_INVITE_MUC) {
		std::string jid = data->ReadString(0);
		std::string room = data->ReadString(1);
		InviteToMuc(jid,room);
	} 
	else if (command == S_LEAVE_MUC) {
		LeaveMuc(data->ReadString(0));
	}
	else if (command == S_ADD_SESSION) {
		cricket::CallOptions options;
		options.recv_video = call_->has_video();
		options.video_bandwidth = cricket::kAutoBandwidth;
		options.data_channel_type = data_channel_type_;
		options.AddSendStream(cricket::MEDIA_TYPE_VIDEO, "", "");

		if (!InitiateAdditionalSession(data->ReadString(0), options)) {
			console_->PrintLine("Failed to initiate additional session.");
		}
	} 
	else if (command == S_RM_SESSION) {
		TerminateAndRemoveSession(call_, data->ReadString(0), false);
	}

#pragma endregion

	#pragma region Call

	else if(command == S_AUDIO){
		std::string to = data->ReadString(0);
		cricket::CallOptions options;
		options.data_channel_type = data_channel_type_;

		if (!PlaceCall(to, options)) {
			console_->PrintLine("Failed to initiate call.");
		}
	}
	else if(command == S_CALL){
		std::string to = data->ReadString(0);
		if(is_dll_test_)
		{
			local_width_ = data->ReadInt(0);
			local_height_ = data->ReadInt(1);
			remote_width_ = data->ReadInt(2);
			remote_height_ = data->ReadInt(3);
		}
		else {
			local_width_ = 600;
			local_height_ = 400;
			remote_width_ = 600;
			remote_height_ =400;
		}

		cricket::CallOptions options;
		options.recv_video = true;
		options.video_bandwidth = cricket::kAutoBandwidth;
		options.data_channel_type = data_channel_type_;

		if (!PlaceCall(to, options)) {
			console_->PrintLine("Failed to initiate call.");
		}
	}
	else if(call_){

		#pragma region AcceptOrReject

		if(incoming_call_)
		{
			if(command == S_AUDIO_ACCEPT){
				cricket::CallOptions options;
				options.data_channel_type = data_channel_type_;
				Accept(options);
			}
			else if(command == S_CALL_ACCEPT){
				if(is_dll_test_)
				{
					local_width_ = data->ReadInt(0);
					local_height_ = data->ReadInt(1);
					remote_width_ = data->ReadInt(2);
					remote_height_ = data->ReadInt(3);
				}
				else {
					local_width_ = 600;
					local_height_ = 400;
					remote_width_ = 600;
					remote_height_ =400;
				}

				cricket::CallOptions options;
				options.video_bandwidth = cricket::kAutoBandwidth;
				options.recv_video = true;
				options.data_channel_type = data_channel_type_;
				options.is_muc = true;
				Accept(options);
			}
			else if(command == S_CALL_REJECT){
				Reject();
			}
		}
		#pragma endregion

		else
		{
			if(command == S_HANG_UP){
				TerminateAndRemoveSession(call_, call_->sessions()[0]->id());
			}
			else if(command == S_START_CAMERA){
				if(video_state_ != State_Video)
				{
					if(video_state_ == State_Screencast)
					{
						StopVideoCapture(false);
					}

					this->StartVideoCapture(false);
				}
			}
			else if(command == S_START_SCREENCAST){
				if(video_state_ != State_Screencast)
				{
					if(video_state_ == State_Video)
					{
						StopVideoCapture(false);
						
					}
					this->StartVideoCapture(true);
				}
			}
			else if(command == S_STOP_CAPTURE){
				if(video_state_ != State_None){
					StopVideoCapture(true);
				}
			}
			else if(command == S_RESTART_CAPTURE){
				//empty
			}
			else if (command == S_MUTE) {
				call_->Mute(true);
				if (InMuc()) 
				{
					hangout_pubsub_client_->PublishAudioMuteState(true);
				}
			} 
			else if (command == S_UNMUTE) {
				call_->Mute(false);
				if (InMuc()) {
					hangout_pubsub_client_->PublishAudioMuteState(false);
				}
			} 
			else if (command == S_SET_REMOTE_VOLUME) {
				if (InMuc()) {
					const std::string nick = data->ReadString(0);
					hangout_pubsub_client_->RemoteMute(nick);
				}
			}
			else if (command == S_VIDEO_MUTE) {
				call_->MuteVideo(true);
				if (InMuc()) {
					hangout_pubsub_client_->PublishVideoMuteState(true);
				}
			} 
			else if (command == S_VIDEO_UNMUTE) {
				call_->MuteVideo(false);
				if (InMuc()) {
					hangout_pubsub_client_->PublishVideoMuteState(false);
				}
			}
			#pragma endregion

			else if (command == S_RECORD) {
				if (InMuc()) {
					hangout_pubsub_client_->PublishRecordingState(true);
				}
			}
			else if (command == S_UNRECORD) {
				if (InMuc()) {
					hangout_pubsub_client_->PublishRecordingState(false);
				}
			}
		}
	}

	if(data)
           delete data;
}

CallClient::CallClient(buzz::XmppClient* xmpp_client,
    const std::string& caps_node, const std::string& version, bool isdlltest)
    : xmpp_client_(xmpp_client),
      worker_thread_(NULL),
      media_engine_(NULL),
      data_engine_(NULL),
      media_client_(NULL),
      call_(NULL),
      hangout_pubsub_client_(NULL),
      incoming_call_(false),
      auto_accept_(false),
      pmuc_domain_("conference.idreamhongkong.com"),
      render_(true),
      data_channel_type_(cricket::DCT_NONE),
      multisession_enabled_(true),
      local_renderer_(NULL),
      static_views_accumulated_count_(0),
      screencast_ssrc_(0),
      roster_(new RosterMap),
      portallocator_flags_(0),
      allow_local_ips_(false),
      signaling_protocol_(cricket::PROTOCOL_HYBRID),
      transport_protocol_(cricket::ICEPROTO_HYBRID),
      sdes_policy_(cricket::SEC_DISABLED),
      dtls_policy_(cricket::SEC_DISABLED),
      ssl_identity_(),
      show_roster_messages_(false),
      remote_video_state_(State_None),
      video_state_(State_None),
	  local_width_(0),
	  local_height_(0),
	  remote_width_(0),
	  remote_height_(0),
	  stun_ip_addr(""),
	  stun_port(0)
{
	xmpp_client_->SignalStateChange.connect(this, &CallClient::OnStateChange);
	my_status_.set_caps_node(caps_node);
	my_status_.set_version(version);
	is_dll_test_ = isdlltest;
}

CallClient::~CallClient() {
  delete media_client_;
  delete roster_;
  delete worker_thread_;
}

const std::string CallClient::strerror(buzz::XmppEngine::Error err) {
  switch (err) {
    case buzz::XmppEngine::ERROR_NONE:
      return "";
    case buzz::XmppEngine::ERROR_XML:
      return "Malformed XML or encoding error";
    case buzz::XmppEngine::ERROR_STREAM:
      return "XMPP stream error";
    case buzz::XmppEngine::ERROR_VERSION:
      return "XMPP version error";
    case buzz::XmppEngine::ERROR_UNAUTHORIZED:
      return "User is not authorized (Check your username and password)";
    case buzz::XmppEngine::ERROR_TLS:
      return "TLS could not be negotiated";
    case buzz::XmppEngine::ERROR_AUTH:
      return "Authentication could not be negotiated";
    case buzz::XmppEngine::ERROR_BIND:
      return "Resource or session binding could not be negotiated";
    case buzz::XmppEngine::ERROR_CONNECTION_CLOSED:
      return "Connection closed by output handler.";
    case buzz::XmppEngine::ERROR_DOCUMENT_CLOSED:
      return "Closed by </stream:stream>";
    case buzz::XmppEngine::ERROR_SOCKET:
      return "Socket error";
    default:
      return "Unknown error";
  }
}

void CallClient::OnCallDestroy(cricket::Call* call) {
  RemoveCallsStaticRenderedViews(call);
  if (call == call_) 
  {
    console_->PrintLine("call destroyed");
    call_ = NULL;
    delete hangout_pubsub_client_;
    hangout_pubsub_client_ = NULL;
  }
}

//edited
void CallClient::OnStateChange(buzz::XmppEngine::State state) {
	switch (state) 
	{
	case buzz::XmppEngine::STATE_START:
		console_->PrintLine("connecting...");
		break;
	case buzz::XmppEngine::STATE_OPENING:
		MultiPlatformsCallback(R_LOGGING, NULL);
		break;
	case buzz::XmppEngine::STATE_OPEN:
		{
			ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
			shareData->WriteString(xmpp_client_->jid().Str(), 0);

			InitMedia();
			InitPresence();

			MultiPlatformsCallback(R_LOGGED_IN, shareData);
		}
		break;
	case buzz::XmppEngine::STATE_CLOSED:
		{
			buzz::XmppEngine::Error error = xmpp_client_->GetError(NULL);
			console_->PrintLine("logged out... %s", strerror(error).c_str());

			MultiPlatformsCallback(R_LOGGED_OUT, NULL);
			//Quit();
		}
		break;
	default:
		break;
	}
}

void CallClient::InitMedia() {
  // The worker thread must be started here since initialization of
  // the ChannelManager will generate messages that need to be
  // dispatched by it.
#if defined(ANDROID)
  worker_thread_ = my_workerthread;
#else
  worker_thread_ = new rtc::Thread();
  worker_thread_->Start();
#endif

  // TODO: It looks like we are leaking many objects. E.g.
  // |network_manager_| is never deleted.
  network_manager_ = new rtc::BasicNetworkManager();

  if(stun_ip_addr == ""){
	  stun_ip_addr = "idreamhongkong.com";
	  stun_port = 3478;
  }

  // TODO: Decide if the relay address should be specified here.
  rtc::SocketAddress stun_addr(stun_ip_addr, stun_port);
  cricket::ServerAddresses stun_servers;
  stun_servers.insert(stun_addr);
  port_allocator_ =  new cricket::BasicPortAllocator(
      network_manager_, stun_servers, rtc::SocketAddress(),
      rtc::SocketAddress(), rtc::SocketAddress());

  if (portallocator_flags_ != 0) {
    port_allocator_->set_flags(portallocator_flags_);
  }
  session_manager_ = new cricket::SessionManager(
      port_allocator_, worker_thread_);
  session_manager_->set_secure(dtls_policy_);
  session_manager_->set_identity(ssl_identity_.get());
  session_manager_->set_transport_protocol(transport_protocol_);
  session_manager_->SignalRequestSignaling.connect(
      this, &CallClient::OnRequestSignaling);
  session_manager_->SignalSessionCreate.connect(
      this, &CallClient::OnSessionCreate);
  session_manager_->OnSignalingReady();

  session_manager_task_ =
      new cricket::SessionManagerTask(xmpp_client_, session_manager_);
  session_manager_task_->EnableOutgoingMessages();
  session_manager_task_->Start();

  if (!media_engine_) {
    media_engine_ = cricket::MediaEngineFactory::Create();
  }

  if (!data_engine_) {
    if (data_channel_type_ == cricket::DCT_SCTP) {
#ifdef HAVE_SCTP
      data_engine_ = new cricket::SctpDataEngine();
#else
      LOG(LS_WARNING) << "SCTP Data Engine not supported.";
      data_channel_type_ = cricket::DCT_NONE;
      data_engine_ = new cricket::RtpDataEngine();
#endif
    } else {
      // Even if we have DCT_NONE, we still have a data engine, just
      // to make sure it isn't NULL.
      data_engine_ = new cricket::RtpDataEngine();
    }
  }
  media_client_ = new cricket::MediaSessionClient(
      xmpp_client_->jid(),
      session_manager_,
      media_engine_,
      data_engine_,
      cricket::DeviceManagerFactory::Create());

  media_client_->SignalCallCreate.connect(this, &CallClient::OnCallCreate);
  media_client_->SignalCallDestroy.connect(this, &CallClient::OnCallDestroy);
  media_client_->SignalDevicesChange.connect(this,
                                             &CallClient::OnDevicesChange);
  media_client_->set_secure(sdes_policy_);
  media_client_->set_multisession_enabled(multisession_enabled_);

#if defined(WIN32)
  //edited file share
  tunnel_client_ = new cricket::TunnelSessionClient(
	  xmpp_client_->jid(),
	  session_manager_);

  tunnel_client_->SignalIncomingTunnel.connect(this,&CallClient::OnIncomingTunnel);
#endif
}

void CallClient::OnRequestSignaling() {
  session_manager_->OnSignalingReady();
}

void CallClient::OnSessionCreate(cricket::Session* session, bool initiate) {
  session->set_current_protocol(signaling_protocol_);
}

void CallClient::OnCallCreate(cricket::Call* call) {
  call->SignalSessionState.connect(this, &CallClient::OnSessionState);
  call->SignalMediaStreamsUpdate.connect(
      this, &CallClient::OnMediaStreamsUpdate);
}

//edited
bool CallClient::StartVideoCapture(bool isScreencast)
{
	if(!call_)return false;
	cricket::Session* session = GetFirstSession();
	if (!session)return false;
	
	if(call_->has_video())
	{
		uint32 ssrc = call_->GetSsrc(session,true);

		if(call_->StartVideoCapture(session, ssrc, isScreencast) && call_->getcapturer())
		{
			console_->PrintLine("startvideocap succeed");

			cricket::StreamParams stream;
			stream.ssrcs.push_back(ssrc);
			if(isScreencast)
			{
				video_state_= State_Screencast;
				stream.id = "screencast";
			}
			else{
				video_state_=State_Video;
				stream.id = "video";
			}

            stream.type = "start";

			if(local_renderer_ == NULL)
				local_renderer_ = CreatVideoRenderer(true, local_width_, local_height_);

			if(local_renderer_ != NULL)
				call_->SetLocalRenderer(local_renderer_);

			cricket::VideoContentDescription* video = call_->CreateVideoStreamUpdate(stream);
			call_->SendVideoStreamUpdate(session, video);

			return true;
		}
	}
	return false;
}

//edited crossplatforms version testing
bool CallClient::StopVideoCapture(bool removeRenderer){
	if(video_state_ != State_None || video_state_ != State_Error )
	{
		if(call_->StopVideoCapture(removeRenderer)){
			if(removeRenderer && local_renderer_){
				delete local_renderer_;
				local_renderer_ = NULL;
			}

			if(!call_)return false;
			cricket::Session* session = GetFirstSession();
			if (!session)return false;
			video_state_ = State_None;
			cricket::StreamParams stream;
			stream.id = video_state_ == VideoState::State_Video ? "video" : "screencast";
			stream.type = "stop";
			cricket::VideoContentDescription* video = call_->CreateVideoStreamUpdate(stream);
			call_->SendVideoStreamUpdate(session, video);
			video_state_ = State_None;

			return true;
		}
		video_state_ = State_Error;
	}
	return false;
}

//edited android mac version ok
/*bool CallClient::StopVideoCapture(bool removeRenderer){
		if(video_state_ != State_None || video_state_ != State_Error )
		{
			if(call_->StopVideoCapture(removeRenderer)){
				if(removeRenderer && local_renderer_){
                    delete local_renderer_;
					local_renderer_ = NULL;
                }

				if(!call_)return false;
				cricket::Session* session = GetFirstSession();
				if (!session)return false;
				video_state_ = State_None;
				uint32 ssrc = call_->GetSsrc(session,true);
				cricket::StreamParams stream;
				stream.ssrcs.push_back(ssrc);
				stream.id = "videonone";
				cricket::VideoContentDescription* video = call_->CreateVideoStreamUpdate(stream);
				call_->SendVideoStreamUpdate(session, video);

				return true;
			}
			video_state_ = State_Error;
		}
		return false;
}*/


//edited 创建多平台Renderer
cricket::VideoRenderer* CallClient::CreatVideoRenderer(bool isLocal, int width, int height){
		cricket::VideoRenderer* renderer_ = NULL;

#if defined(WIN32)
		renderer_ = is_dll_test_ ? new cricket::Win32VideoRenderer(width, height, isLocal) :
			new cricket::GdiVideoRenderer(width, height);
		renderer_->SetSize(width,height, 0);
#elif defined(ANDROID)
		callBack_Test_Method("if(sv_local || sv_remote)");
		if(sv_local || sv_remote)
			renderer_ = new cricket::AndroidVideoRenderer(0, 0, isLocal ? sv_local : sv_remote, webrtc::VideoRenderType::kRenderAndroid);
#elif defined(LINUX) && defined(HAVE_GTK)
		renderer_ = new cricket::GtkVideoRenderer(width, height);
#elif defined(WEBRTC_APPRTC_RENDERER)
		renderer_ = new cricket::ApprtcVideoRenderer(width, height, isLocal);
#elif defined(WEBRTC_IOS)
		renderer_ = new cricket::IOSVideoRenderer(width, height, isLocal);
#elif defined(WEBRTC_MACCONSOLE_RENDERER)
		renderer_ = new cricket::MacConsoleVideoRenderer(width, height, isLocal);
#elif defined(WEBRTC_MAC)
		renderer_ = new cricket::MacVideoRenderer(width, height, isLocal);
#endif

	return renderer_;
}

//edited CallBack
void CallClient::OnSessionState(cricket::Call* call,
                                cricket::Session* session,
                                cricket::Session::State state) {
  if (state == cricket::Session::STATE_RECEIVEDINITIATE) {
    buzz::Jid jid(session->remote_name());
    if (call_ == call && multisession_enabled_) {
      // We've received an initiate for an existing call. This is actually a
      // new session for that call.
      console_->PrintLine("Incoming session from '%s'", jid.Str().c_str(), " and auto accepted");
      AddSession(session);

      cricket::CallOptions options;
	  options.video_bandwidth = cricket::kAutoBandwidth;
      options.recv_video = call_->has_video();
      options.data_channel_type = data_channel_type_;
	  options.is_muc = true;

      call_->AcceptSession(session, options);

      if (call_->has_video()) {
        RenderAllStreams(call, session, true);
      }
    } 
	else {
      console_->PrintLine("Incoming call from '%s'", jid.Str().c_str());
      call_ = call;
      AddSession(session);
      incoming_call_ = true;

	  //edited 此调用暂时保留，待以后测试后删除（应该只在开始Capture后创建renderer）
	  if (call->has_video() && local_renderer_ == NULL) 
		  local_renderer_ = CreatVideoRenderer(true, local_width_, local_height_);

      if (auto_accept_) {
        cricket::CallOptions options;
        options.recv_video = true;
        options.data_channel_type = data_channel_type_;
        Accept(options);
      }
    }

    ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
    shareData->WriteString(jid.Str(), 0);

    MultiPlatformsCallback(R_CALL, shareData);
  } 
  else if (state == cricket::Session::STATE_SENTINITIATE) {
   //edited 此调用暂时保留，待以后测试后删除（应该只在开始Capture后创建renderer）
   if (call->has_video() && local_renderer_ == NULL)
	   local_renderer_ = CreatVideoRenderer(true, local_width_, local_height_);

    console_->PrintLine("calling...");
  } 
  else if (state == cricket::Session::STATE_RECEIVEDACCEPT) {
	//对方接受Call
    MultiPlatformsCallback(R_CALL_ACCEPT,NULL);
    SetupAcceptedCall();
  } 
  else if (state == cricket::Session::STATE_RECEIVEDREJECT) {
	//对方拒绝Call
    MultiPlatformsCallback(R_CALL_REJECT,NULL);
  } 
  else if (state == cricket::Session::STATE_INPROGRESS) {
    call->SignalSpeakerMonitor.connect(this, &CallClient::OnSpeakerChanged);
    call->StartSpeakerMonitor(session);
  } 
  else if (state == cricket::Session::STATE_RECEIVEDTERMINATE) {
    TerminateAndRemoveSession(call, session->id());
	//对方终止Call
    MultiPlatformsCallback(R_CALL_TERMINATE,NULL);
  }
}

void CallClient::OnSpeakerChanged(cricket::Call* call,
                                  cricket::Session* session,
                                  const cricket::StreamParams& speaker) {
  if (!speaker.has_ssrcs()) 
		console_->PrintLine("Session %s has no current speaker.", session->id().c_str());
  else if (speaker.id.empty()) {
		console_->PrintLine("Session %s speaker change to unknown (%u).",
                        session->id().c_str(), speaker.first_ssrc());
  } 
  else {
		console_->PrintLine("Session %s speaker changed to %s (%u).",
                        session->id().c_str(), speaker.id.c_str(),
                        speaker.first_ssrc());
  }
}

void SetMediaCaps(int media_caps, buzz::PresenceStatus* status) {
  status->set_voice_capability((media_caps & cricket::AUDIO_RECV) != 0);
  status->set_video_capability((media_caps & cricket::VIDEO_RECV) != 0);
  status->set_camera_capability((media_caps & cricket::VIDEO_SEND) != 0);
}

void SetCaps(int media_caps, buzz::PresenceStatus* status) {
  status->set_know_capabilities(true);
  status->set_pmuc_capability(true);
  SetMediaCaps(media_caps, status);
}

void SetAvailable(const buzz::Jid& jid, buzz::PresenceStatus* status) {
  status->set_jid(jid);
  status->set_available(true);
  status->set_show(buzz::PresenceStatus::SHOW_ONLINE);
}

void CallClient::InitPresence() {
  presence_push_ = new buzz::PresencePushTask(xmpp_client_, this);
  presence_push_->SignalStatusUpdate.connect(this, &CallClient::OnStatusUpdate);
  presence_push_->SignalMucJoined.connect(this, &CallClient::OnMucJoined);
  presence_push_->SignalMucLeft.connect(this, &CallClient::OnMucLeft);
  presence_push_->SignalMucStatusUpdate.connect(this, &CallClient::OnMucStatusUpdate);
  presence_push_->Start();

  presence_out_ = new buzz::PresenceOutTask(xmpp_client_);
  SetAvailable(xmpp_client_->jid(), &my_status_);
  SetCaps(media_client_->GetCapabilities(), &my_status_);
  SendStatus(my_status_);
  presence_out_->Start();

  muc_invite_recv_ = new buzz::MucInviteRecvTask(xmpp_client_);
  muc_invite_recv_->SignalInviteReceived.connect(this, &CallClient::OnMucInviteReceived);
  muc_invite_recv_->Start();

  muc_invite_send_ = new buzz::MucInviteSendTask(xmpp_client_);
  muc_invite_send_->Start();

  friend_invite_send_ = new buzz::FriendInviteSendTask(xmpp_client_);
  friend_invite_send_->Start();

  StartXmppPing();
}

void CallClient::StartXmppPing() {
  buzz::PingTask* ping = new buzz::PingTask(
      xmpp_client_, rtc::Thread::Current(),
      kPingPeriodMillis, kPingTimeoutMillis);
  ping->SignalTimeout.connect(this, &CallClient::OnPingTimeout);
  ping->Start();
}

void CallClient::OnPingTimeout() {
  LOG(LS_WARNING) << "XMPP Ping timeout. Will keep trying...";
  StartXmppPing();

  // Or should we do this instead?
  // Quit();

  MultiPlatformsCallback(R_LOGGED_OUT, NULL);
}

void CallClient::SendStatus(const buzz::PresenceStatus& status) {
  presence_out_->Send(status);
}

void CallClient::OnStatusUpdate(const buzz::PresenceStatus& status) {
  RosterItem item;
  item.jid = status.jid();
  item.show = status.show();
  item.status = status.status();

  std::string key = item.jid.Str();

  if (status.available() && status.voice_capability()) {
    if(show_roster_messages_) {
		console_->PrintLine("Adding to roster: %s", key.c_str());
    }
    (*roster_)[key] = item;
    // TODO: Make some of these constants.
  } 
  else {
    if(show_roster_messages_) {
		console_->PrintLine("Removing from roster: %s", key.c_str());
    }
    RosterMap::iterator iter = roster_->find(key);
    if (iter != roster_->end())
      roster_->erase(iter);
  }
}

void CallClient::PrintRoster() {
  console_->PrintLine("Roster contains %d callable", roster_->size());

  RosterMap::iterator iter = roster_->begin();
  while (iter != roster_->end()) {
    console_->PrintLine("%s - %s",
                        iter->second.jid.BareJid().Str().c_str(),
                        DescribeStatus(iter->second.show, iter->second.status));
    iter++;
  }
}

void CallClient::SendChat(const std::string& to, const std::string msg) {
  buzz::XmlElement* stanza = new buzz::XmlElement(buzz::QN_MESSAGE);
  stanza->AddAttr(buzz::QN_TO, to);
  stanza->AddAttr(buzz::QN_ID, rtc::CreateRandomString(16));
  stanza->AddAttr(buzz::QN_TYPE, "chat");
  buzz::XmlElement* body = new buzz::XmlElement(buzz::QN_BODY);
  body->SetBodyText(msg);
  stanza->AddElement(body);

  xmpp_client_->SendStanza(stanza);
  delete stanza;
}

void CallClient::SendData(const std::string& streamid,
                          const std::string& text) {
  // TODO(mylesj): Support sending data over sessions other than the first.
  cricket::Session* session = GetFirstSession();
  if(!call_ || !session) {
	  console_->PrintLine("Must be in a call to send data.");
      return;
  }
  if (!call_->has_data()) {
	  console_->PrintLine("This call doesn't have a data channel.");
	  return;
  }

  const cricket::DataContentDescription* data =
      cricket::GetFirstDataContentDescription(session->local_description());
  if (!data) {
    console_->PrintLine("This call doesn't have a data content.");
    return;
  }

  const cricket::StreamParams* stream = cricket::GetStreamByIds(data->streams(), "", streamid);
  if (!stream) {
    LOG(LS_WARNING) << "Could not send data: no such stream: "
                    << streamid << ".";
    return;
  }

  cricket::SendDataParams params;
  params.ssrc = stream->first_ssrc();
  rtc::Buffer payload(text.data(), text.length());
  cricket::SendDataResult result;
  bool sent = call_->SendData(session, params, payload, &result);
  if (!sent) {
    if (result == cricket::SDR_BLOCK) {
      LOG(LS_WARNING) << "Could not send data because it would block.";
    } else {
      LOG(LS_WARNING) << "Could not send data for unknown reason.";
    }
  }
}

void CallClient::InviteFriend(const std::string& name) {
  buzz::Jid jid(name);
  if (!jid.IsValid() || jid.node() == "") {
    console_->PrintLine("Invalid JID. JIDs should be in the form user@domain.");
    return;
  }
  // Note: for some reason the Buzz backend does not forward our presence
  // subscription requests to the end user when that user is another call
  // client as opposed to a Smurf user. Thus, in that scenario, you must
  // run the friend command as the other user too to create the linkage
  // (and you won't be notified to do so).
  friend_invite_send_->Send(jid);
  console_->PrintLine("Requesting to befriend %s.", name.c_str());
}

bool CallClient::FindJid(const std::string& name, buzz::Jid* found_jid,
                         cricket::CallOptions* options) {
  bool found = false;
  options->is_muc = false;
  buzz::Jid callto_jid(name);
  if (name.length() == 0 && mucs_.size() > 0) {
    // if no name, and in a MUC, establish audio with the MUC
    *found_jid = mucs_.begin()->first;
    found = true;
    options->is_muc = true;
  } 
  else if (name[0] == '+') {
    // if the first character is a +, assume it's a phone number
    *found_jid = callto_jid;
    found = true;
  } 
  else {
    // otherwise, it's a friend
    for (RosterMap::iterator iter = roster_->begin();
         iter != roster_->end(); ++iter) {
      if (iter->second.jid.BareEquals(callto_jid)) {
        found = true;
        *found_jid = iter->second.jid;
        break;
      }
    }

    if (!found) {
      if (mucs_.count(callto_jid) == 1 && mucs_[callto_jid]->state() == buzz::Muc::MUC_JOINED) {
        found = true;
        *found_jid = callto_jid;
        options->is_muc = true;
      }
    }
  }

  if (found) {
    console_->PrintLine("Found %s '%s'",
                        options->is_muc ? "room" : "online friend",
                        found_jid->Str().c_str());
  } else {
    console_->PrintLine("Could not find online friend '%s'", name.c_str());
  }

  return found;
}

void CallClient::OnDataReceived(cricket::Call*,
                                const cricket::ReceiveDataParams& params,
                                const rtc::Buffer& payload) {
  // TODO(mylesj): Support receiving data on sessions other than the first.
  cricket::Session* session = GetFirstSession();
  if (!session)
    return;

  const std::vector<cricket::StreamParams>* data_streams =
      call_->GetDataRecvStreams(session);
  const cricket::StreamParams* stream =
      data_streams ? GetStreamBySsrc(*data_streams, params.ssrc) : nullptr;
  std::string text(payload.data(), payload.length());
  if (stream) {
    console_->PrintLine(
        "Received data from '%s' on stream '%s' (ssrc=%u): %s",
        stream->groupid.c_str(), stream->id.c_str(),
        params.ssrc, text.c_str());
  } else {
    console_->PrintLine(
        "Received data (ssrc=%u): %s",
        params.ssrc, text.c_str());
  }
}

bool CallClient::PlaceCall(const std::string& name,
                           cricket::CallOptions options) {
  buzz::Jid jid(name);

  if (!call_) {
    call_ = media_client_->CreateCall();
    AddSession(call_->InitiateSession(jid, media_client_->jid(), options));
  }
  media_client_->SetFocus(call_);
  if (call_->has_video() && render_ && !options.is_muc) {
    // TODO(pthatcher): Hookup local_render_ to the local capturer.
  }
  if (options.is_muc) {
    const std::string& nick = mucs_[jid]->local_jid().resource();
    hangout_pubsub_client_ =
        new buzz::HangoutPubSubClient(xmpp_client_, jid, nick);
    hangout_pubsub_client_->SignalPresenterStateChange.connect(
        this, &CallClient::OnPresenterStateChange);
    hangout_pubsub_client_->SignalAudioMuteStateChange.connect(
        this, &CallClient::OnAudioMuteStateChange);
    hangout_pubsub_client_->SignalRecordingStateChange.connect(
        this, &CallClient::OnRecordingStateChange);
    hangout_pubsub_client_->SignalRemoteMute.connect(
        this, &CallClient::OnRemoteMuted);
    hangout_pubsub_client_->SignalMediaBlock.connect(
        this, &CallClient::OnMediaBlocked);
    hangout_pubsub_client_->SignalRequestError.connect(
        this, &CallClient::OnHangoutRequestError);
    hangout_pubsub_client_->SignalPublishAudioMuteError.connect(
        this, &CallClient::OnHangoutPublishAudioMuteError);
    hangout_pubsub_client_->SignalPublishPresenterError.connect(
        this, &CallClient::OnHangoutPublishPresenterError);
    hangout_pubsub_client_->SignalPublishRecordingError.connect(
        this, &CallClient::OnHangoutPublishRecordingError);
    hangout_pubsub_client_->SignalRemoteMuteError.connect(
        this, &CallClient::OnHangoutRemoteMuteError);
    hangout_pubsub_client_->RequestAll();
  }

  return true;
}

bool CallClient::InitiateAdditionalSession(const std::string& name,
                                           cricket::CallOptions options) {
  // Can't add a session if there is no call yet.
  if (!call_)
    return false;

  buzz::Jid jid(name);
  /*if (!FindJid(name, &jid, &options))
    return false;*/

  std::vector<cricket::Session*>& call_sessions = sessions_[call_->id()];
  call_sessions.push_back(
      call_->InitiateSession(jid,
                             buzz::Jid(call_sessions[0]->remote_name()),
                             options));

  return true;
}

//edited
void CallClient::TerminateAndRemoveSession(cricket::Call* call,
	const std::string& id, bool stopCapture) {
		if(stopCapture)
			StopVideoCapture(true);

		std::vector<cricket::Session*>& call_sessions = sessions_[call->id()];
		for (std::vector<cricket::Session*>::iterator iter = call_sessions.begin();
			iter != call_sessions.end(); ++iter) {
				if ((*iter)->id() == id) {
					RenderAllStreams(call, *iter, false);
					/*ManagedDataWriter writer1(2);
					writer1.WriteString("video TerminateAndRemoveSession");
					writer1.WriteString("success!");
					NewDayCallback(R_DEBUG_INFO,writer1.ToArray());*/
					call_->TerminateSession(*iter);
					call_sessions.erase(iter);
					break;
				}
		}
}

void CallClient::PrintCalls() {
  const std::map<uint32, cricket::Call*>& calls = media_client_->calls();
  for (std::map<uint32, cricket::Call*>::const_iterator i = calls.begin();
       i != calls.end(); ++i) {
    console_->PrintLine("Call (id:%d), is %s",
                        i->first,
                        i->second == call_ ? "active" : "on hold");
    std::vector<cricket::Session *>& sessions = sessions_[call_->id()];
    for (std::vector<cricket::Session *>::const_iterator j = sessions.begin();
         j != sessions.end(); ++j) {
      console_->PrintLine("|--Session (id:%s), to %s", (*j)->id().c_str(),
                          (*j)->remote_name().c_str());

      std::vector<cricket::StreamParams>::const_iterator k;
      const std::vector<cricket::StreamParams>* streams =
          i->second->GetAudioRecvStreams(*j);
      if (streams)
        for (k = streams->begin(); k != streams->end(); ++k) {
          console_->PrintLine("|----Audio Stream: %s", k->ToString().c_str());
        }
      streams = i->second->GetVideoRecvStreams(*j);
      if (streams)
        for (k = streams->begin(); k != streams->end(); ++k) {
          console_->PrintLine("|----Video Stream: %s", k->ToString().c_str());
        }
      streams = i->second->GetDataRecvStreams(*j);
      if (streams)
        for (k = streams->begin(); k != streams->end(); ++k) {
          console_->PrintLine("|----Data Stream: %s", k->ToString().c_str());
        }
    }
  }
}

void CallClient::SwitchToCall(uint32 call_id) {
  const std::map<uint32, cricket::Call*>& calls = media_client_->calls();
  std::map<uint32, cricket::Call*>::const_iterator call_iter =
      calls.find(call_id);
  if (call_iter != calls.end()) {
    media_client_->SetFocus(call_iter->second);
    call_ = call_iter->second;
  } else {
    console_->PrintLine("Unable to find call: %d", call_id);
  }
}

void CallClient::OnPresenterStateChange(
    const std::string& nick, bool was_presenting, bool is_presenting) {
  if (!was_presenting && is_presenting) {
    console_->PrintLine("%s now presenting.", nick.c_str());
  } else if (was_presenting && !is_presenting) {
    console_->PrintLine("%s no longer presenting.", nick.c_str());
  } else if (was_presenting && is_presenting) {
    console_->PrintLine("%s still presenting.", nick.c_str());
  } else if (!was_presenting && !is_presenting) {
    console_->PrintLine("%s still not presenting.", nick.c_str());
  }
}

void CallClient::OnAudioMuteStateChange(
    const std::string& nick, bool was_muted, bool is_muted) {
  if (!was_muted && is_muted) {
    console_->PrintLine("%s now muted.", nick.c_str());
  } else if (was_muted && !is_muted) {
    console_->PrintLine("%s no longer muted.", nick.c_str());
  }
}

void CallClient::OnRecordingStateChange(
    const std::string& nick, bool was_recording, bool is_recording) {
  if (!was_recording && is_recording) {
    console_->PrintLine("%s now recording.", nick.c_str());
  } else if (was_recording && !is_recording) {
    console_->PrintLine("%s no longer recording.", nick.c_str());
  }
}

void CallClient::OnRemoteMuted(const std::string& mutee_nick,
                               const std::string& muter_nick,
                               bool should_mute_locally) {
  if (should_mute_locally) {
    call_->Mute(true);
    console_->PrintLine("Remote muted by %s.", muter_nick.c_str());
  } else {
    console_->PrintLine("%s remote muted by %s.",
                        mutee_nick.c_str(), muter_nick.c_str());
  }
}

void CallClient::OnMediaBlocked(const std::string& blockee_nick,
                                const std::string& blocker_nick) {
  console_->PrintLine("%s blocked by %s.",
                      blockee_nick.c_str(), blocker_nick.c_str());
}

void CallClient::OnHangoutRequestError(const std::string& node,
                                       const buzz::XmlElement* stanza) {
  console_->PrintLine("Failed request pub sub items for node %s.",
                      node.c_str());
}

void CallClient::OnHangoutPublishAudioMuteError(
    const std::string& task_id, const buzz::XmlElement* stanza) {
  console_->PrintLine("Failed to publish audio mute state.");
}

void CallClient::OnHangoutPublishPresenterError(
    const std::string& task_id, const buzz::XmlElement* stanza) {
  console_->PrintLine("Failed to publish presenting state.");
}

void CallClient::OnHangoutPublishRecordingError(
    const std::string& task_id, const buzz::XmlElement* stanza) {
  console_->PrintLine("Failed to publish recording state.");
}

void CallClient::OnHangoutRemoteMuteError(const std::string& task_id,
                                          const std::string& mutee_nick,
                                          const buzz::XmlElement* stanza) {
  console_->PrintLine("Failed to remote mute.");
}

void CallClient::Accept(const cricket::CallOptions& options) {
  ASSERT(call_ && incoming_call_);
  ASSERT(sessions_[call_->id()].size() == 1);
  cricket::Session* session = GetFirstSession();
  call_->AcceptSession(session, options);
  media_client_->SetFocus(call_);

  if (call_->has_video()) {
    // TODO(pthatcher): Hookup local_render_ to the local capturer.
    RenderAllStreams(call_, session, true);
  }
  SetupAcceptedCall();
  incoming_call_ = false;
}

void CallClient::SetupAcceptedCall() {
  if (call_->has_data()) {
    call_->SignalDataReceived.connect(this, &CallClient::OnDataReceived);
  }
}

void CallClient::Reject() {
  ASSERT(call_ && incoming_call_);
  call_->RejectSession(call_->sessions()[0]);
  incoming_call_ = false;
}

void CallClient::Quit() {
  rtc::Thread::Current()->Quit();
}

void CallClient::SetNick(const std::string& muc_nick) {
  my_status_.set_nick(muc_nick);

  // TODO: We might want to re-send presence, but right
  // now, it appears to be ignored by the MUC.
  //
  // presence_out_->Send(my_status_); for (MucMap::const_iterator itr
  // = mucs_.begin(); itr != mucs_.end(); ++itr) {
  // presence_out_->SendDirected(itr->second->local_jid(),
  // my_status_); }

  console_->PrintLine("Nick set to '%s'.", muc_nick.c_str());
}

void CallClient::LookupAndJoinMuc(const std::string& room_name) {
  // The room_name can't be empty for lookup task.
  if (room_name.empty()) {
    console_->PrintLine("Please provide a room name or room jid.");
    return;
  }

  std::string room = room_name;
  std::string domain = xmpp_client_->jid().domain();
  if (room_name.find("@") != std::string::npos) {
    // Assume the room_name is a fully qualified room name.
    // We'll find the room name string and domain name string from it.
    room = room_name.substr(0, room_name.find("@"));
    domain = room_name.substr(room_name.find("@") + 1);
  }

  buzz::MucRoomLookupTask* lookup_query_task =
      buzz::MucRoomLookupTask::CreateLookupTaskForRoomName(
          xmpp_client_, buzz::Jid(buzz::STR_GOOGLE_MUC_LOOKUP_JID), room,
          domain);
  lookup_query_task->SignalResult.connect(this,
      &CallClient::OnRoomLookupResponse);
  lookup_query_task->SignalError.connect(this,
      &CallClient::OnRoomLookupError);
  lookup_query_task->Start();
}

void CallClient::JoinMuc(const std::string& room_jid_str) {
  if (room_jid_str.empty()) {
    buzz::Jid room_jid = GenerateRandomMucJid();
    console_->PrintLine("Generated a random room jid: %s",
                        room_jid.Str().c_str());
    JoinMuc(room_jid);
  } else {
    JoinMuc(buzz::Jid(room_jid_str));
  }
}

void CallClient::JoinMuc(const buzz::Jid& room_jid) {
  if (!room_jid.IsValid()) {
    console_->PrintLine("Unable to make valid muc endpoint for %s",
                        room_jid.Str().c_str());
    return;
  }

  std::string room_nick = room_jid.resource();
  if (room_nick.empty()) {
    room_nick = (xmpp_client_->jid().node()
                 + "_" + xmpp_client_->jid().resource());
  }

  MucMap::iterator elem = mucs_.find(room_jid);
  if (elem != mucs_.end()) {
		console_->PrintLine("This MUC already exists.");
    return;
  }

  buzz::Muc* muc = new buzz::Muc(room_jid.BareJid(), room_nick);
  mucs_[muc->jid()] = muc;
  presence_out_->SendDirected(muc->local_jid(), my_status_);
}

void CallClient::OnRoomLookupResponse(buzz::MucRoomLookupTask* task,
                                      const buzz::MucRoomInfo& room) {
  // The server requires the room be "configured" before being used.
  // We only need to configure it if we create it, but rooms are
  // auto-created at lookup, so there's currently no way to know if we
  // created it.  So, we configure it every time, just in case.
  // Luckily, it appears to be safe to configure a room that's already
  // configured.  Our current flow is:
  // 1. Lookup/auto-create
  // 2. Configure
  // 3. Join
  // TODO: In the future, once the server supports it, we
  // should:
  // 1. Lookup
  // 2. Create and Configure if necessary
  // 3. Join
  std::vector<std::string> room_features;
  room_features.push_back(buzz::STR_MUC_ROOM_FEATURE_ENTERPRISE);
  buzz::MucRoomConfigTask* room_config_task = new buzz::MucRoomConfigTask(
      xmpp_client_, room.jid, room.full_name(), room_features);
  room_config_task->SignalResult.connect(this,
      &CallClient::OnRoomConfigResult);
  room_config_task->SignalError.connect(this,
      &CallClient::OnRoomConfigError);
  room_config_task->Start();
}

void CallClient::OnRoomLookupError(buzz::IqTask* task,
                                   const buzz::XmlElement* stanza) {
  if (stanza == NULL) {
    console_->PrintLine("Room lookup failed.");
  } else {
    console_->PrintLine("Room lookup error: ", stanza->Str().c_str());
  }
}

void CallClient::OnRoomConfigResult(buzz::MucRoomConfigTask* task) {
  JoinMuc(task->room_jid());
}

void CallClient::OnRoomConfigError(buzz::IqTask* task,
                                   const buzz::XmlElement* stanza) {
  console_->PrintLine("Room config failed.");
  // We join the muc anyway, because if the room is already
  // configured, the configure will fail, but we still want to join.
  // Idealy, we'd know why the room config failed and only do this on
  // "already configured" errors.  But right now all we get back is
  // "not-allowed".
  buzz::MucRoomConfigTask* config_task =
      static_cast<buzz::MucRoomConfigTask*>(task);
  JoinMuc(config_task->room_jid());
}

void CallClient::OnMucInviteReceived(const buzz::Jid& inviter,
    const buzz::Jid& room,
    const std::vector<buzz::AvailableMediaEntry>& avail) {
  console_->PrintLine("Invited to join %s by %s.", room.Str().c_str(),
      inviter.Str().c_str());
  console_->PrintLine("Available media:");
  if (avail.size() > 0) {
    for (std::vector<buzz::AvailableMediaEntry>::const_iterator i =
            avail.begin();
        i != avail.end();
        ++i) {
      console_->PrintLine("  %s, %s",
                          buzz::AvailableMediaEntry::TypeAsString(i->type),
                          buzz::AvailableMediaEntry::StatusAsString(i->status));
    }
  } else {
    console_->PrintLine("  None");
  }
  // We automatically join the room.
  JoinMuc(room);
}

void CallClient::OnMucJoined(const buzz::Jid& endpoint) {
  MucMap::iterator elem = mucs_.find(endpoint);
  ASSERT(elem != mucs_.end() &&
         elem->second->state() == buzz::Muc::MUC_JOINING);

  buzz::Muc* muc = elem->second;
  muc->set_state(buzz::Muc::MUC_JOINED);
  console_->PrintLine("Joined \"%s\"", muc->jid().Str().c_str());
}

void CallClient::OnMucStatusUpdate(const buzz::Jid& jid,
    const buzz::MucPresenceStatus& status) {
  // Look up this muc.
  MucMap::iterator elem = mucs_.find(jid);
  ASSERT(elem != mucs_.end());

  buzz::Muc* muc = elem->second;

  if (status.jid().IsBare() || status.jid() == muc->local_jid()) {
    // We are only interested in status about other users.
    return;
  }

  if (status.available()) {
    muc->members()[status.jid().resource()] = status;
  } else {
    muc->members().erase(status.jid().resource());
  }
}

bool CallClient::InMuc() {
  const buzz::Jid* muc_jid = FirstMucJid();
  if (!muc_jid) return false;
  return muc_jid->IsValid();
}

const buzz::Jid* CallClient::FirstMucJid() {
  if (mucs_.empty()) return NULL;
  return &(mucs_.begin()->first);
}

void CallClient::LeaveMuc(const std::string& room) {
  buzz::Jid room_jid;
  const buzz::Jid* muc_jid = FirstMucJid();
  if (room.length() > 0) {
    room_jid = buzz::Jid(room);
  } else if (mucs_.size() > 0) {
    // leave the first MUC if no JID specified
    if (muc_jid) {
      room_jid = *(muc_jid);
    }
  }

  if (!room_jid.IsValid()) {
    console_->PrintLine("Invalid MUC JID.");
    return;
  }

  MucMap::iterator elem = mucs_.find(room_jid);
  if (elem == mucs_.end()) {
    console_->PrintLine("No such MUC.");
    return;
  }

  buzz::Muc* muc = elem->second;
  muc->set_state(buzz::Muc::MUC_LEAVING);

  buzz::PresenceStatus status;
  status.set_jid(my_status_.jid());
  status.set_available(false);
  status.set_priority(0);
  presence_out_->SendDirected(muc->local_jid(), status);
}

void CallClient::OnMucLeft(const buzz::Jid& endpoint, int error) {
  // We could be kicked from a room from any state.  We would hope this
  // happens While in the MUC_LEAVING state
  MucMap::iterator elem = mucs_.find(endpoint);
  if (elem == mucs_.end())
    return;

  buzz::Muc* muc = elem->second;
  if (muc->state() == buzz::Muc::MUC_JOINING) {
    console_->PrintLine("Failed to join \"%s\", code=%d",
                        muc->jid().Str().c_str(), error);
  } else if (muc->state() == buzz::Muc::MUC_JOINED) {
    console_->PrintLine("Kicked from \"%s\"",
                        muc->jid().Str().c_str());
  }

  delete muc;
  mucs_.erase(elem);
}

void CallClient::InviteToMuc(const std::string& given_user,
                             const std::string& room) {
  std::string user = given_user;

  // First find the room.
  const buzz::Muc* found_muc;
  if (room.length() == 0) {
    if (mucs_.size() == 0) {
		console_->PrintLine("Not in a room yet; can't invite.");
      return;
    }
    // Invite to the first muc
    found_muc = mucs_.begin()->second;
  } else {
    MucMap::iterator elem = mucs_.find(buzz::Jid(room));
    if (elem == mucs_.end()) {
		console_->PrintLine("Not in room %s.", room.c_str());
      return;
    }
    found_muc = elem->second;
  }

  buzz::Jid invite_to = found_muc->jid();

  // Now find the user. We invite all of their resources.
  bool found_user = false;
  buzz::Jid user_jid(user);
  for (RosterMap::iterator iter = roster_->begin();
       iter != roster_->end(); ++iter) {
    if (iter->second.jid.BareEquals(user_jid)) {
      buzz::Jid invitee = iter->second.jid;
      muc_invite_send_->Send(invite_to, invitee);
      found_user = true;
    }
  }
  if (!found_user) {
    buzz::Jid invitee = user_jid;
    muc_invite_send_->Send(invite_to, invitee);
  }
}

void CallClient::GetDevices() {
  std::vector<std::string> names;
  media_client_->GetAudioInputDevices(&names);
  console_->PrintLine("Audio input devices:");
  PrintDevices(names);
  media_client_->GetAudioOutputDevices(&names);
  console_->PrintLine("Audio output devices:");
  PrintDevices(names);
  media_client_->GetVideoCaptureDevices(&names);
  console_->PrintLine("Video capture devices:");
  PrintDevices(names);
}

void CallClient::PrintDevices(const std::vector<std::string>& names) {
  for (size_t i = 0; i < names.size(); ++i) {
    console_->PrintLine("%d: %s", static_cast<int>(i), names[i].c_str());
  }
}

void CallClient::OnDevicesChange() {
  console_->PrintLine("Devices changed.");
  SetMediaCaps(media_client_->GetCapabilities(), &my_status_);
  SendStatus(my_status_);
}

void CallClient::SetVolume(const std::string& level) {
  media_client_->SetOutputVolume(strtol(level.c_str(), NULL, 10));
}

//edited crossplatforms version testing
void CallClient::OnMediaStreamsUpdate(cricket::Call* call,
	cricket::Session* session,
	const cricket::MediaStreams& added,
	const cricket::MediaStreams& removed) {
		if (call && call->has_video()) {
			for (std::vector<cricket::StreamParams>::const_iterator 
				it = removed.video().begin(); it != removed.video().end(); ++it) {
					if(remote_video_state_ != VideoState::State_None){
						RemoveStaticRenderedView(it->first_ssrc());
						call->SetVideoRenderer(session, it->first_ssrc(), NULL);
						remote_video_state_ = VideoState::State_None;

						MultiPlatformsCallback(R_STOP_CAPTURE, NULL);
					}
			}

			RenderStreams(call, session, added.video(), true);
			SendViewRequest(call, session);
		}
}

//edited android mac version ok
/*void CallClient::OnMediaStreamsUpdate(cricket::Call* call,
	cricket::Session* session,
	const cricket::MediaStreams& added,
	const cricket::MediaStreams& removed) {
		if (call && call->has_video()) {
			for (std::vector<cricket::StreamParams>::const_iterator it = removed.video().begin(); it != removed.video().end(); ++it) {
				RemoveStaticRenderedView(it->first_ssrc());
				call->SetVideoRenderer(session, it->first_ssrc(), NULL);
			}


			RenderStreams(call, session, added.video(), true);
			SendViewRequest(call, session);
		}
}*/


void CallClient::RenderAllStreams(cricket::Call* call,
                                  cricket::Session* session,
                                  bool enable) {
  const std::vector<cricket::StreamParams>* video_streams =
      call->GetVideoRecvStreams(session);
  if (video_streams) {
    RenderStreams(call, session, *video_streams, enable);
  }
}

void CallClient::RenderStreams(
    cricket::Call* call,
    cricket::Session* session,
    const std::vector<cricket::StreamParams>& video_streams,
    bool enable) {
  std::vector<cricket::StreamParams>::const_iterator stream;
  for (stream = video_streams.begin(); stream != video_streams.end();
       ++stream) {
    RenderStream(call, session, *stream, enable);
  }
}

//edited crossplatforms version testing
void CallClient::RenderStream(cricket::Call* call,
                              cricket::Session* session,
                              const cricket::StreamParams& stream,
                              bool enable) {
  if (!stream.has_ssrcs()) {
    // Nothing to see here; move along.
    return;
  }

  uint32 ssrc = stream.first_ssrc();
  StaticRenderedViews::iterator iter =
      static_rendered_views_.find(std::make_pair(session, ssrc));

  if (enable && stream.id.length() > 0 && 
	  ((stream.id == "video" && remote_video_state_ != VideoState::State_Video) || (stream.id == "screencast" && remote_video_state_ != VideoState::State_Screencast))) 
  {
    if (iter == static_rendered_views_.end()) {
      // TODO(pthatcher): Make dimensions and positions more configurable.
      int offset = (50 * static_views_accumulated_count_) % 300;
      AddStaticRenderedView(session, ssrc, remote_width_, remote_height_, 30,
					offset, offset);

      iter = static_rendered_views_.find(std::make_pair(session, ssrc));
    }
    call->SetVideoRenderer(session, ssrc, iter->second.renderer);
	remote_video_state_ = stream.id == "video" ? VideoState::State_Video : VideoState::State_Screencast;

	MultiPlatformsCallback("video" ? R_CAMERA : R_SCREENCAST, NULL);
  } 
  else if (iter != static_rendered_views_.end() && remote_video_state_ != VideoState::State_None) 
  {
      call->SetVideoRenderer(session, ssrc, NULL);
	  remote_video_state_ = VideoState::State_None;
      RemoveStaticRenderedView(ssrc);

	  MultiPlatformsCallback(R_STOP_CAPTURE, NULL);
  }

}

//edited android mac version ok
/*void CallClient::RenderStream(cricket::Call* call,
	cricket::Session* session,
	const cricket::StreamParams& stream,
	bool enable) {
		if (!stream.has_ssrcs()) {
			// Nothing to see here; move along.
			return;
		}

		uint32 ssrc = stream.first_ssrc();
		StaticRenderedViews::iterator iter =
			static_rendered_views_.find(std::make_pair(session, ssrc));

		if (enable && stream.id.length() > 0) 
		{
			if (iter == static_rendered_views_.end()) {
				// TODO(pthatcher): Make dimensions and positions more configurable.
				int offset = (50 * static_views_accumulated_count_) % 300;
				AddStaticRenderedView(session, ssrc, remote_width_, remote_height_, 30,
					offset, offset);

				iter = static_rendered_views_.find(std::make_pair(session, ssrc));
			}
			call->SetVideoRenderer(session, ssrc, iter->second.renderer);

		} else if (iter != static_rendered_views_.end()) {
			call->SetVideoRenderer(session, ssrc, NULL);
			RemoveStaticRenderedView(ssrc);
		}

}*/

// TODO: Would these methods to add and remove views make
// more sense in call.cc?  Would other clients use them?
void CallClient::AddStaticRenderedView(
    cricket::Session* session,
    uint32 ssrc, int width, int height, int framerate,
    int x_offset, int y_offset) {

  StaticRenderedView rendered_view(
      cricket::StaticVideoView(
          cricket::StreamSelector(ssrc), width, height, framerate), CreatVideoRenderer(false, width, height));

  rendered_view.renderer->SetSize(width, height, 0);
  static_rendered_views_.insert(std::make_pair(std::make_pair(session, ssrc),
                                               rendered_view));

  ++static_views_accumulated_count_;
  console_->PrintLine("Added renderer for ssrc %d", ssrc);
}

bool CallClient::RemoveStaticRenderedView(uint32 ssrc) {
  for (StaticRenderedViews::iterator it = static_rendered_views_.begin();
       it != static_rendered_views_.end(); ++it) {
    if (it->second.view.selector.ssrc == ssrc) {
      delete it->second.renderer;
      static_rendered_views_.erase(it);
      console_->PrintLine("Removed renderer for ssrc %d", ssrc);
      return true;
    }
  }
  return false;
}

void CallClient::RemoveCallsStaticRenderedViews(cricket::Call* call) {
  std::vector<cricket::Session*>& sessions = sessions_[call->id()];
  std::set<cricket::Session*> call_sessions(sessions.begin(), sessions.end());
  for (StaticRenderedViews::iterator it = static_rendered_views_.begin();
       it != static_rendered_views_.end(); ) {
    if (call_sessions.find(it->first.first) != call_sessions.end()) {
      delete it->second.renderer;
      static_rendered_views_.erase(it++);
    } else {
      ++it;
    }
  }
}

void CallClient::SendViewRequest(cricket::Call* call,
                                 cricket::Session* session) {
  cricket::ViewRequest request;
  for (StaticRenderedViews::iterator it = static_rendered_views_.begin();
       it != static_rendered_views_.end(); ++it) {
    if (it->first.first == session) {
      request.static_video_views.push_back(it->second.view);
    }
  }
  call->SendViewRequest(session, request);
}

buzz::Jid CallClient::GenerateRandomMucJid() {
  // Generate a GUID of the form XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX,
  // for an eventual JID of private-chat-<GUID>@groupchat.google.com.
  char guid[37], guid_room[256];
  for (size_t i = 0; i < ARRAY_SIZE(guid) - 1;) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      guid[i++] = '-';
    } else {
      sprintf(guid + i, "%04x", rand());
      i += 4;
    }
  }

  rtc::sprintfn(guid_room,
                      ARRAY_SIZE(guid_room),
                      "private-chat-%s@%s",
                      guid,
                      pmuc_domain_.c_str());
  return buzz::Jid(guid_room);
}

bool CallClient::SelectFirstDesktopScreencastId(
    cricket::ScreencastId* screencastid) {
  if (!rtc::WindowPickerFactory::IsSupported()) {
    LOG(LS_WARNING) << "Window picker not suported on this OS.";
    return false;
  }

  rtc::WindowPicker* picker =
      rtc::WindowPickerFactory::CreateWindowPicker();
  if (!picker) {
    LOG(LS_WARNING) << "Could not create a window picker.";
    return false;
  }

  rtc::DesktopDescriptionList desktops;
  if (!picker->GetDesktopList(&desktops) || desktops.empty()) {
    LOG(LS_WARNING) << "Could not get a list of desktops.";
    return false;
  }

  *screencastid = cricket::ScreencastId(desktops[0].id());
  return true;
}

void CallClient::PrintStats() const {
  const cricket::VoiceMediaInfo& vmi = call_->last_voice_media_info();

  for (std::vector<cricket::VoiceSenderInfo>::const_iterator it =
       vmi.senders.begin(); it != vmi.senders.end(); ++it) {
    console_->PrintLine("Sender: ssrc=%u codec='%s' bytes=%d packets=%d "
                        "rtt=%d jitter=%d",
                        it->ssrc(), it->codec_name.c_str(), it->bytes_sent,
                        it->packets_sent, it->rtt_ms, it->jitter_ms);
  }

  for (std::vector<cricket::VoiceReceiverInfo>::const_iterator it =
       vmi.receivers.begin(); it != vmi.receivers.end(); ++it) {
    console_->PrintLine("Receiver: ssrc=%u bytes=%d packets=%d "
                        "jitter=%d loss=%.2f",
                        it->ssrc(), it->bytes_rcvd, it->packets_rcvd,
                        it->jitter_ms, it->fraction_lost);
  }
}


#pragma region File Share
//edited file share

#if defined(WIN32)

std::string GetFileName(std::string path){
	int len = path.length();
	int pos = path.find_last_of('\\',path.length()-1);
	if(pos == path.npos)
		pos = path.find_last_of('/',path.length()-1);
	return path.substr(pos+1,len-pos-1);
}

BOOL WINAPI SendFileCallback(LPWSTR lpFileOrPath ,WIN32_FIND_DATAW lpFileInfo, void* pUserData)
{
	LPSTR path=WcharToUtf8(lpFileOrPath);
	if ((lpFileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)  
	{  
		EnumerateFuncData* data = static_cast<EnumerateFuncData*>(pUserData);
		data->client->InitiateFileShare(data->to,path,path);
	}
	return true;
} 

void CallClient::InitiateFileShare(buzz::Jid to,std::string file_fullpath,std::string description){
	cricket::TunnelSession *tunnel = tunnel_client_->CreateTunnel(to, description);
	CallClient::FileProcessor *processor = new CallClient::FileProcessor(true,file_fullpath,this,tunnel->GetSession());

	processors_.push_back(processor);

	if(processor->ProcessStream(tunnel->GetStream()))
	{
		tunnel->GetSession()->SignalState.connect(this, &CallClient::OnFileSessionState);

		ThreadShareData* shareData = new ThreadShareData(3, 0, 0);
		shareData->WriteString(tunnel->GetSession()->id(), 0);
		shareData->WriteString(to.Str(), 1);
		shareData->WriteString(file_fullpath, 2);

		MultiPlatformsCallback(R_FILE_SESSION_ID, shareData);
	}
	else
	{
		/*ManagedDataWriter writer(2);
		writer.WriteString("error");
		writer.WriteString("initiatefileshare processstream!");
		MultiPlatformsCallback(R_DEBUG_INFO,writer.ToArray());*/

		//tunnel->GetSession()->Terminate();
	}
}

void CallClient::OnFileSessionState(cricket::BaseSession* session,cricket::BaseSession::State state) {
	ThreadShareData* shareData = new ThreadShareData(1, 0, 0);
	shareData->WriteString(session->id());

	switch (state) {
	case cricket::Session::STATE_RECEIVEDREJECT:
		MultiPlatformsCallback(R_FILE_REJECT, shareData);
		break;
	case cricket::Session::STATE_RECEIVEDACCEPT:
		MultiPlatformsCallback(R_FILE_ACCEPT, shareData);
		break;

	case cricket::Session::STATE_SENTTERMINATE:
	case cricket::Session::STATE_RECEIVEDTERMINATE:
		MultiPlatformsCallback(R_FILE_TERMINATE, shareData);
		break;
	}
}

void CallClient::OnIncomingTunnel(cricket::TunnelSessionClient* client, buzz::Jid jid,
	std::string description, cricket::Session* session){
	//description=GetFileName(description);

	CallClient::FileProcessor *processor = new CallClient::FileProcessor(false,"",this,session);
	processors_.push_back(processor);

	session->SignalState.connect(this, &CallClient::OnFileSessionState);
	std::string str = "";
	if (auto_accept_) 
	{
		if(!processor->ProcessStream(tunnel_client_->AcceptTunnel(session)))
		{
			/*ManagedDataWriter writer(2);
			writer.WriteString("error");
			writer.WriteString("OnIncomingTunnel ProcessStream return false!");
			NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
		}
		/*tunnel_client_->DeclineTunnel(session);*/

		str = "auto accept " + description;
	}
	else{
		str = "You recieved one file request: " + description + ", from your friend " + jid.Str() + ",id is: " + session->id() 
			+ "! To accept or reject, please write down the Id after the command faccept or freject! for example:faccept 45685!";
	}
	this->SignalOuput(str);

	ThreadShareData* shareData = new ThreadShareData(3, 0, 0);
	shareData->WriteString(session->id(), 0);
	shareData->WriteString(jid.Str(), 1);
	shareData->WriteString(description, 2);

	MultiPlatformsCallback(R_FILE_REQUEST, shareData);
}

void CallClient::AcceptOrRejectFileShare(bool to_accept,std::string session_id,std::string filename,std::string path){
	CallClient::FileProcessor* processor = GetFileProcessor(session_id);
	if(to_accept)
	{
		/*std::string str=processor->file_name_;
		processor->file_name_=path.length() ? path + str : "c:/" + str;	*/
		processor->SetSession(path + filename);
		if(!processor->ProcessStream(tunnel_client_->AcceptTunnel(processor->GetSession())))
		{
			/*ManagedDataWriter writer(2);
			writer.WriteString("error");
			writer.WriteString("AcceptOrRejectFileShare ProcessStream return false!");
			MultiPlatformsCallback(R_DEBUG_INFO,writer.ToArray());*/

			//tunnel_client_->DeclineTunnel(processor->GetSession());
		}
	}
	else
		tunnel_client_->DeclineTunnel(processor->GetSession());
}

void CallClient::OnFileStreamClosed(CallClient::FileProcessor *prosessor){

	for (int i = 0;i<processors_.size();i++)
	{
		if(processors_[i]==prosessor){
			processors_.erase(processors_.begin()+i);
		}
	}
	delete prosessor;
}

CallClient::FileProcessor* CallClient::GetFileProcessor(std::string sid){
	for (int i = 0;i<processors_.size();i++)
	{
		if(processors_[i]->GetSession()->id().compare(sid) == 0)
			return  processors_[i];
	}
}

CallClient::FileProcessor::FileProcessor(bool send, std::string filename, CallClient* call,cricket::Session* session){
	sending_ = send;
	parent_ = call;
	file_.reset(new talk_base::FileStream);
	buffer_len_ = 0;
	session_ = session;
	file_name_ = filename;
}

bool CallClient::FileProcessor::ProcessStream(talk_base::StreamInterface* stream){
	int err;

	//ManagedDataWriter writer(2);
	if (!file_->Open(file_name_.c_str(), sending_ ? "rb" : "wb", &err))
	{
		/*writer.WriteString("error File open failed!");
		writer.WriteString(file_name_.c_str());
		NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
		return false;
	}
	/*file_len_ = filelength(fileno(file));*/

	stream->SignalEvent.connect(this, &FileProcessor::OnStreamEvent);
	if (stream->GetState() == talk_base::SS_CLOSED) {
		//std::cerr << "Failed to establish P2P tunnel" << std::endl;
		/*writer.WriteString("error Failed to establish P2P tunnel");
		writer.WriteString(file_name_.c_str());
		NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
		return false;
	}
	if (stream->GetState() == talk_base::SS_OPEN) {
		OnStreamEvent(stream,
			talk_base::SE_OPEN | talk_base::SE_READ | talk_base::SE_WRITE, 0);
	}		
	/*writer.WriteString("succeed File opened!");
	writer.WriteString(file_name_.c_str());
	NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
	return true;
}

void CallClient::FileProcessor::OnStreamEvent(talk_base::StreamInterface* stream, int events,
	int error) {
	if (events & talk_base::SE_CLOSE) {
		if (error == 0) {
			//std::cout << "Tunnel closed normally" << std::endl;
		} else {
			//std::cout << "Tunnel closed with error: " << error << std::endl;	
			/*ManagedDataWriter writer(2);
			writer.WriteString("error OnStreamEvent");
			writer.WriteString("Tunnel closed with error");
			NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
		}
		Cleanup(stream,false);
		return;
	}

	if (events & talk_base::SE_OPEN) {
		/*std::cout << "Tunnel connected" << std::endl;*/
		/*ManagedDataWriter writer(2);
		writer.WriteString("succeed OnStreamEvent");
		writer.WriteString("Tunnel connected");
		NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
	}

	talk_base::StreamResult result;
	size_t count;

	//sender write buffer from file
	if (sending_ && (events & talk_base::SE_WRITE)) {
		/*LOG(LS_VERBOSE) << "Tunnel SE_WRITE";*/

		/*ManagedDataWriter writer(2);
		writer.WriteString("sender write buffer from file");
		writer.WriteString(file_name_.c_str());
		NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/

		while (true) {
			size_t write_pos = 0;
			while (write_pos < buffer_len_) {
				result = stream->Write(buffer_ + write_pos, buffer_len_ - write_pos,
					&count, &error);
				if (result == talk_base::SR_SUCCESS) {
					write_pos += count;

					size_t pos;
					file_->GetPosition(&pos);

					ThreadShareData* shareData = new ThreadShareData(1, 1, 0);
					shareData->WriteString(session_->id(), 0);
					shareData->WriteInt(pos, 0);

					MultiPlatformsCallback(R_FILE_PROGRESS, shareData);
					continue;
				}
				if (result == talk_base::SR_BLOCK) {
					buffer_len_ -= write_pos;
					memmove(buffer_, buffer_ + write_pos, buffer_len_);
					LOG(LS_VERBOSE) << "Tunnel write block";
					return;
				}
				if (result == talk_base::SR_EOS) {
					/*std::cout << "Tunnel closed unexpectedly on write" << std::endl;*/
					/*ManagedDataWriter writer(2);
					writer.WriteString("error OnStreamEvent");
					writer.WriteString("Tunnel closed unexpectedly on write");
					NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
				} else {
					//std::cout << "Tunnel write error: " << error << std::endl;
					/*ManagedDataWriter writer(2);
					writer.WriteString("error OnStreamEvent");
					writer.WriteString( "Tunnel write error: " + error );
					NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
				}
				Cleanup(stream,false);
				return;
			}
			buffer_len_ = 0;

			while (buffer_len_ < sizeof(buffer_)) {
				result = file_->Read(buffer_ + buffer_len_,
					sizeof(buffer_) - buffer_len_,
					&count, &error);
				if (result == talk_base::SR_SUCCESS) {
					buffer_len_ += count;
					continue;
				}
				if (result == talk_base::SR_EOS) {
					if (buffer_len_ > 0)
						break;
					//std::cout << "End of file" << std::endl;
					// A hack until we have friendly shutdown
					Cleanup(stream, true);
					return;
				} else if (result == talk_base::SR_BLOCK) {
					//std::cout << "File blocked unexpectedly on read" << std::endl;
					/*ManagedDataWriter writer(2);
					writer.WriteString("error OnStreamEvent");
					writer.WriteString( "File blocked unexpectedly on read ");
					NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
				} else {
					//std::cout << "File read error: " << error << std::endl;
					/*ManagedDataWriter writer(2);
					writer.WriteString("error OnStreamEvent");
					writer.WriteString( "File read error:" + error );
					NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/
				}
				Cleanup(stream, false);
				return;
			}
		}
	}

	//receiver read buffer to file
	if (!sending_ && (events & talk_base::SE_READ)) {
		/*ManagedDataWriter writer(2);
		writer.WriteString("receiver read buffer to file");
		writer.WriteString(file_name_.c_str());
		NewDayCallback(R_DEBUG_INFO,writer.ToArray());*/

		//LOG(LS_VERBOSE) << "Tunnel SE_READ";
		while (true) {
			buffer_len_ = 0;
			while (buffer_len_ < sizeof(buffer_)) {

				result = stream->Read(buffer_ + buffer_len_,
					sizeof(buffer_) - buffer_len_,
					&count, &error);
				if (result == talk_base::SR_SUCCESS) {
					buffer_len_ += count;
					continue;
				}
				if (result == talk_base::SR_BLOCK) {
					if (buffer_len_ > 0)
						break;
					//LOG(LS_VERBOSE) << "Tunnel read block";
					return;
				}
				if (result == talk_base::SR_EOS) {
					//std::cout << "Tunnel closed unexpectedly on read" << std::endl;
				} else {
					//std::cout << "Tunnel read error: " << error << std::endl;
				}
				Cleanup(stream, false);
				return;
			}
			size_t write_pos = 0;
			while (write_pos < buffer_len_) {
				result = file_->Write(buffer_ + write_pos, buffer_len_ - write_pos,
					&count, &error);
				if (result == talk_base::SR_SUCCESS) {
					write_pos += count;

					size_t pos;
					file_->GetPosition(&pos);

					ThreadShareData* shareData = new ThreadShareData(1, 1, 0);
					shareData->WriteString(session_->id(), 0);
					shareData->WriteInt(pos, 0);

					MultiPlatformsCallback(R_FILE_PROGRESS, shareData);
					continue;
				}
				if (result == talk_base::SR_EOS) {
					//std::cout << "File closed unexpectedly on write" << std::endl;
				} else if (result == talk_base::SR_BLOCK) {
					//std::cout << "File blocked unexpectedly on write" << std::endl;
				} else {
					//std::cout << "File write error: " << error << std::endl;
				}
				Cleanup(stream, false);
				return;
			}
		}
	}
}

void CallClient::FileProcessor::Cleanup(talk_base::StreamInterface* stream, bool delay = false) {
	//LOG(LS_VERBOSE) << "Closing";
	stream->Close();
	file_.reset();

	parent_->OnFileStreamClosed(this);

	/*if (!server_) {
	if (delay)
	talk_base::Thread::Current()->PostDelayed(2000, NULL, MSG_DONE);
	else
	talk_base::Thread::Current()->Post(NULL, MSG_DONE);
	}*/
}

void CallClient::FileProcessor::SetSession(cricket::Session *session){
	session_= session;
}

#elif defined(ANDROID)
// android file share
#elif defined(LINUX)
// linux file share
#elif defined(OSX)
// mac or ios file share
#endif

//edited file share end
#pragma endregion

#pragma region Call
//edited call

uint32 CallClient::get_ssrc(cricket::Session* session){

	const std::vector<cricket::StreamParams> video_streams =
		*call_->GetVideoRecvStreams(session);
	std::vector<cricket::StreamParams>::const_iterator stream= video_streams.begin();
	cricket::StreamParams streamparams=*stream;
	return streamparams.first_ssrc();
}

//edited call end
#pragma endregion


