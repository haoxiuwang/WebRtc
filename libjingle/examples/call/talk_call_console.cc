#if defined(WEBRTC_MAC) && !defined(WEBRTC_IOS)
//mac console renderer
#define WEBRTC_MACCONSOLE_RENDERER
#endif

#include "webrtc/libjingle/examples/call/talk_call.h"

int main(int argc, char **argv) {

  DEFINE_bool(consoletestserver, false, "Use test server.");
  DEFINE_string(consoleoauth, "", "OAuth2 access token.");

  bool test_server = FLAG_consoletestserver;
  std::string oauth_token = FLAG_consoleoauth;

  buzz::Jid jid;
  std::string username;
  std::string password;

  if (username.empty()) {
	  MyPrint("JID: ");
	  std::cin >> username;
  }
  if (username.find('@') == std::string::npos) {
	  username.append("@idreamhongkong.com");
  }
  jid = buzz::Jid(username);
  if (!jid.IsValid() || jid.node() == "") {
	  MyPrint("Invalid JID. JIDs should be in the form user@domain\n");
	  return 1;
  }
  if (password == "" && !test_server && oauth_token.empty()) {
	  //Console_Imp::SetEcho(false);
	  MyPrint("Password: ");
	  std::cin >> password;
	  //Console_Imp::SetEcho(true);
	  MyPrint("\n");
  }

  Login(username, password, "call");

  return 0;
}
