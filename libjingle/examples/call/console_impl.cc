#define _CRT_SECURE_NO_DEPRECATE 1

#if defined(WEBRTC_POSIX)
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#endif  // WEBRTC_POSIX

#include <cassert>
#include "webrtc/libjingle/examples/call/console_impl.h"

#ifdef WEBRTC_POSIX
static void DoNothing(int unused) {}
#endif

Console_Impl::Console_Impl(rtc::Thread *thread, CallClient *client) :
Console(thread,client),isTaskStart_(false),next_(0),stopped_(false)
{}

Console_Impl::~Console_Impl() {
	Stop();
}

void Console_Impl::Start() {
MyPrint("Console_Impl::Start");
	if (stopped_) {
		// stdin was closed in Stop(), so we can't restart.
		LOG(LS_ERROR) << "Cannot re-start";
		return;
	}
	if (console_thread_) {
		LOG(LS_WARNING) << "Already started";
		return;
	}

	console_thread_.reset(new rtc::Thread());
	console_thread_->Start();
	console_thread_->Post(this, MSG_START);
}

void Console_Impl::Stop() {
	if (console_thread_) {
#ifdef WIN32
		CloseHandle(GetStdHandle(STD_INPUT_HANDLE));
#else
		close(fileno(stdin));
		// This forces the read() in fgets() to return with errno = EINTR. fgets()
		// will retry the read() and fail, thus returning.
		pthread_kill(console_thread_->GetPThread(), SIGUSR1);
#endif
		console_thread_->Stop();
		console_thread_.reset();
		stopped_ = true;
	}
}

void Console_Impl::SetEcho(bool on) {
#ifdef WIN32
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if ((hIn == INVALID_HANDLE_VALUE) || (hIn == NULL))
		return;

	DWORD mode;
	if (!GetConsoleMode(hIn, &mode))
		return;

	if (on) {
		mode = mode | ENABLE_ECHO_INPUT;
	} else {
		mode = mode & ~ENABLE_ECHO_INPUT;
	}

	SetConsoleMode(hIn, mode);
#else
	const int fd = fileno(stdin);
	if (fd == -1)
		return;

	struct termios tcflags;
	if (tcgetattr(fd, &tcflags) == -1)
		return;

	if (on) {
		tcflags.c_lflag |= ECHO;
	} else {
		tcflags.c_lflag &= ~ECHO;
	}

	tcsetattr(fd, TCSANOW, &tcflags);
#endif
}

void Console_Impl::PrintLine(const char* format, ...) {
	/*
	va_list ap;
	va_start(ap, format);

	char buf[4096];
	int size = vsnprintf(buf, sizeof(buf), format, ap);
	assert(size >= 0);
	assert(size < static_cast<int>(sizeof(buf)));
	buf[size] = '\0';
	printf("%s\n", buf);
	fflush(stdout);

	va_end(ap);*/
}
void Console_Impl::WPrintLine(const wchar_t* format, ...) {
	/*
	va_list ap;
	va_start(ap, format);

	wchar_t buf[4096];
	int size = vsnprintf(buf, sizeof(buf), format, ap);
	assert(size >= 0);
	assert(size < static_cast<int>(sizeof(buf)));
	buf[size] = L'\0';
	wprintf(L"%s\n", buf);
	fflush(stdout);

	va_end(ap);*/
}

void Console_Impl::RunConsole() {
	char input_buffer[255];
	while (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
		int i = strlen(input_buffer);
		if (input_buffer[i-1] != '\0')
		{
			input_buffer[i-1] = '\0';
		}
		if(strlen(input_buffer)) 
			client_thread_->Post(this, MSG_INPUT,
			new rtc::TypedMessageData<std::string>(input_buffer));
	}
}

std::string GetWord(const std::vector<std::string>& words,size_t index, const std::string& def) 
{
	if (words.size() > index) {
		return words[index];
	} else {
		return def;
	}
}

void Console_Impl::SplitStanza(const std::string& str, std::vector<std::string>& ret_, std::string sep)
{
	if(sep.empty() || str.empty())
		return;

	int offset = 0;
	bool loop = true;
	while(loop)
	{
		std::string stanza = "";
		int position = str.find(sep,offset);

		if (position == str.npos)
		{
			if(offset < str.length() - 1){
				int count = str.length() - offset;
				stanza = str.substr(offset, count);
				//managedCallback((void *)str[offset]);
			}
			loop = false;
		}
		else
		{			
			stanza = str.substr(offset, position - offset);
		}

		if(!stanza.empty())
			ret_.push_back(stanza);

		offset = position + sep.length();
	}	
}

void MultiPlatformsCallback(Enum_RecievedMessage command, ThreadShareData* data) {

	Console_Impl* temp_console = reinterpret_cast<Console_Impl*>(console_);

	if(data == NULL)
		return;

	switch (command)
	{
	case R_XMPP_PROTOCOL:
		{			
			//this->PrintLine(data->ReadString(0).c_str());
		}
		break;
	case R_DEBUG_INFO:
		{
			temp_console->PrintLine(data->ReadString(0).c_str());
			temp_console->PrintLine(data->ReadString(1).c_str());
		}
		break;
	case R_LOGGED_IN:
	case R_FILE_ACCEPT:
	case R_FILE_REJECT:
		{
			temp_console->PrintLine(data->ReadString(0).c_str());
		}
		break;
	case R_LOGGED_OUT:
		{
			temp_console->PrintLine("Logged Out");
		}
		break;

	case R_FILE_SESSION_ID:
		{
			std::string sessionId = data->ReadString(0);
			std::string jid = data->ReadString(1);
			std::string fullFilename = data->ReadString(2);
			std::string str = sessionId + jid + fullFilename;

			temp_console->PrintLine(str.c_str());
		}
		break;
	case R_FILE_REQUEST:
		{
			std::string sessionId = data->ReadString(0);
			std::string jid = data->ReadString(1);
			std::string filedesc = data->ReadString(2);
			std::string str = sessionId + jid + filedesc;

			temp_console->PrintLine(str.c_str());
		}
		break;
	case R_FILE_TERMINATE:
		{
			temp_console->PrintLine("File Terminate!");
			temp_console->PrintLine(data->ReadString(0).c_str());
		}
		break;
	case R_CALL_TERMINATE:
		temp_console->PrintLine("对方终止Call");

		//message+=("you received session terminate from friend!");
		break;
	case R_CALL:

		temp_console->PrintLine("对方发送Call请求");

		/*client_thread_->Post(this, MSG_INPUT,
		new rtc::TypedMessageData<std::string>("accept"));*/
		/*message+=("you received vcall message from ");
		message+=cstr;
		message+=",please enter accept to get video started!";*/
		break;
	case R_CAMERA:
		/*message+=("you received video from friend! ssrc is ");
		message += cstr;*/
		break;
	case R_SCREENCAST:
		/*message+=("you received screencast from friend! ssrc is ");
		message += cstr;*/
		break;
	//default:
		//message=cstr;
	}

	if(data)
		delete data;
}

void Console_Impl::ParseLine(std::string line) {

	std::vector<std::string> words;
	SplitStanza(line,words,":babel:");

	const std::string com = GetWord(words, 0, "");
	int command = std::atoi(com.c_str());

	Enum_SendMessage message = (Enum_SendMessage)command;
    	ThreadShareData* data = NULL;

	switch(message)
	{
	case S_FILE_REQUEST:
	case S_FILE_ACCEPT:
		{
			data = new ThreadShareData(3, 0, 0);
			/*data->WriteAnciString(GetWord(words,1,"").c_str(), 0);
			data->WriteAnciString(GetWord(words,2,"").c_str(), 1);
			data->WriteAnciString(GetWord(words,3,"").c_str(), 2);*/
			data->WriteString(GetWord(words,1,""), 0);
			data->WriteString(GetWord(words,2,""), 1);
			data->WriteString(GetWord(words,3,""), 2);
		}
		break;
	case S_CALL:
	case S_JOIN_MUC:
	case S_LEAVE_MUC:
	case S_ADD_SESSION:
	case S_RM_SESSION:
		{
MyPrint("case S_CALL:");
			data = new ThreadShareData(1, 0, 0);
			//data->WriteAnciString(GetWord(words,1,"").c_str(), 0);
			std::string s = GetWord(words,1,"");
MyPrint(s);
			data->WriteString(s, 0);
		}
		break;
	case S_INVITE_MUC:
		{
			data = new ThreadShareData(2, 0, 0);
			//data->WriteAnciString(GetWord(words,1,"").c_str(), 0);
			//data->WriteAnciString(GetWord(words,2,"").c_str(), 1);
			data->WriteString(GetWord(words,1,""), 0);
			data->WriteString(GetWord(words,2,""), 1);
		}
		break;
	case S_CALL_ACCEPT:
	case S_START_CAMERA:
	case S_START_SCREENCAST:
	case S_STOP_CAPTURE:
	case S_HANG_UP:
	case S_MUTE:
	case S_UNMUTE:
	case S_VIDEO_MUTE:
	case S_VIDEO_UNMUTE:
	case S_START_PREVIEW_CAPTURE:
	case S_STOP_PREVIEW_CAPTURE:
		{
			client_->ParseLine(command, NULL);
		}
		break;
	}

MyPrint("client_->ParseLine");
	client_->ParseLine(command, data);
}

void Console_Impl::OnMessage(rtc::Message *msg) {
	switch (msg->message_id) {
	case MSG_START:
#ifdef WEBRTC_POSIX
		// Install a no-op signal so that we can abort RunConsole() by raising
		// SIGUSR1.
		struct sigaction act;
		act.sa_handler = &DoNothing;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		if (sigaction(SIGUSR1, &act, NULL) < 0) {
			LOG(LS_WARNING) << "Can't install signal";
		}
#endif
		RunConsole();
		break;
	case MSG_INPUT:
		rtc::TypedMessageData<std::string>* data =
			static_cast<rtc::TypedMessageData<std::string>*>(msg->pdata);
		//client_->ParseLine(data->data().GetMsg(),data->data().GetData());
		ParseLine(data->data());
		break;
	}}
