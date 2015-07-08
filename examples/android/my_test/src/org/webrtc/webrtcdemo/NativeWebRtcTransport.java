package org.webrtc.webrtcdemo;

import android.content.Context;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

/**
 * Created by Administrator on 2015/5/23.
 */
public class NativeWebRtcTransport {

    private Context context;
    public NativeWebRtcTransport(Context c){
        this.context = c;
    }

    static {
        System.loadLibrary("mylibjingle-jni");
    }

    public enum RecievedMessage
    {
        // 传输Xmpp协议

        /// <summary>
        /// 收到由libjingle传输层转发的Xmpp协议
        /// lpstr XmppProtocol;  int ByteLength
        /// </summary>
        R_XMPP_PROTOCOL,
        // 传输Xmpp协议

        // 登陆状态

        /// <summary>
        /// 正在登陆
        /// </summary>
        R_LOGGING,

        /// <summary>
        /// 登入成功，返回带资源的Jid
        /// lpwstr FullJid
        /// </summary>
        R_LOGGED_IN,

        /// <summary>
        /// 从服务器登出
        /// </summary>
        R_LOGGED_OUT,
        // 登陆状态

        // 文件传输

        /// <summary>
        /// 文件发送方已创建Session组件，返回SessionId
        /// lpwstr SessionId;  lpwstr Jid;  lpwstr FullFileName;
        /// </summary>
        R_FILE_SESSION_ID,

        /// <summary>
        /// 对方向己方发送文件
        /// lpwstr SessionId;  lpwstr Jid;  lpwstr FileDescription;
        /// </summary>
        R_FILE_REQUEST,

        /// <summary>
        /// 对方接受文件
        /// lpwstr SessionId;
        /// </summary>
        R_FILE_ACCEPT,

        /// <summary>
        /// 对方拒绝文件
        /// lpwstr SessionId;
        /// </summary>
        R_FILE_REJECT,

        /// <summary>
        /// 对方终止传输文件
        /// lpwstr SessionId;
        /// </summary>
        R_FILE_TERMINATE,

        /// <summary>
        /// 获取文件进度
        /// lpwstr SessionId;  int Progress
        /// </summary>
        R_FILE_PROGRESS,
        // 文件传输

        // 视频音频

        // 会话协议

        /// <summary>
        /// 对方发送Call请求
        /// lpwstr Jid;
        /// </summary>
        R_CALL,

        /// <summary>
        /// 对方接受Call
        /// </summary>
        R_CALL_ACCEPT,

        /// <summary>
        /// 对方拒绝Call
        /// </summary>
        R_CALL_REJECT,

        /// <summary>
        /// 对方终止Call
        /// </summary>
        R_CALL_TERMINATE,
        // 会话协议

        /// <summary>
        /// 对方开启远程桌面
        /// </summary>
        R_SCREENCAST,

        /// <summary>
        /// 对方开启摄像头捕获
        /// </summary>
        R_CAMERA,

        /// <summary>
        ///  获得本地的视频帧
        ///  void* 帧buffer int width int height
        /// </summary>
        R_LOCAL_FRAME_METADATA,

        /// <summary>
        ///  获得对方的视频帧
        ///  void* 帧buffer int width int height
        /// </summary>
        R_REMOTE_FRAME_METADATA,

        /// <summary>
        /// 对方关掉视频捕获
        /// </summary>
        R_STOP_CAPTURE,

        /// <summary>
        /// 已了创建本地视频Renderer
        /// 窗口句柄 void* handle
        /// </summary>
        R_LOCAL_RENDERER_HANDLE,

        /// <summary>
        /// 已了创建对方视频Renderer
        /// 窗口句柄 void* handle
        /// </summary>
        R_REMOTE_RENDERER_HANDLE,

        /// <summary>
        ///  获得本地的视频帧
        /// </summary>
        R_LOCAL_FRAME_COMING,

        /// <summary>
        ///  获得对方的视频帧
        /// </summary>
        R_REMOTE_FRAME_COMING,
        //视频音频

        /// <summary>
        /// 调试信息  lpwstr info;  lpwstr content;
        /// </summary>
        R_DEBUG_INFO
    }

    public enum SendMessage
    {
        // 传输Xmpp协议

        /// <summary>
        /// 利用libjingle传输层转发xmpp协议
        /// lpstr XmppProtocl
        /// </summary>
        S_XMPPP_ROTOCOL,

        /// <summary>
        /// 传入stun服务器的IP地址及端口
        /// lpstr ip  int port
        /// </summary>
        S_STUN_ADDRESS,
        // 传输Xmpp协议

        // 登陆状态

        /// <summary>
        /// 从服务器登出
        /// </summary>
        S_LOGOUT,
        //登陆状态

        // 文件传输

        /// <summary>
        /// 发送文件
        /// lpstr RecieverJid;  lpstr FullFileName;  lpstr FileDescription;
        /// </summary>
        S_FILE_REQUEST,

        /// <summary>
        /// 接受文件
        /// lpstr SessionId;  lpstr FileName;  lpstr SavePath;
        /// </summary>
        S_FILE_ACCEPT,

        /// <summary>
        /// 拒绝接受文件
        /// lpstr SessionId;
        /// </summary>
        S_FILE_REJECT,

        /// <summary>
        /// 终止传输文件
        /// lpstr SessionId;
        /// </summary>
        S_FILE_TERMINTATE,
        //文件传输

        // 视频音频

        // 会话协议

        /// <summary>
        /// Call请求
        /// lpstr RecieveJid; int localWidth;  int localHeight; void* localpHandle;  int remoteWidth;  int remoteHeight; void* remotepHandle
        /// </summary>
        S_CALL,

        /// <summary>
        /// 接受Call
        /// int localWidth;  int localHeight; void* localpHandle;  int remoteWidth;  int remoteHeight; void* remotepHandle
        /// </summary>
        S_CALL_ACCEPT,

        /// <summary>
        /// 拒绝Call
        /// </summary>
        S_CALL_REJECT,

        /// <summary>
        /// 终止并删除会话
        /// </summary>
        S_HANG_UP,
        // 会话协议

        /// <summary>
        /// 停止己方的音频传输
        /// </summary>
        S_MUTE,

        /// <summary>
        /// 重新开始己方的音频传输
        /// </summary>
        S_UNMUTE,

        /// <summary>
        /// 控制对方传输过来的音频音量
        /// lpstr Volume
        /// </summary>
        S_SET_REMOTE_VOLUME,

        /// <summary>
        /// 停止己方的视频传输
        /// </summary>
        S_VIDEO_MUTE,

        /// <summary>
        /// 重新开始己方的视频传输
        /// </summary>
        S_VIDEO_UNMUTE,

        /// <summary>
        /// 开启远程桌面
        /// </summary>
        S_START_SCREENCAST,

        /// <summary>
        /// 开启摄像头捕获
        /// </summary>
        S_START_CAMERA,

        /// <summary>
        /// 停止捕获视频
        /// </summary>
        S_STOP_CAPTURE,

        /// <summary>
        /// 重设捕获的视频数据
        /// int Width;  int Height;  int Interval;  int ColorSpace;
        /// </summary>
        S_RESTART_CAPTURE,

        /// <summary>
        /// 打开本地视频Renderer
        /// int Width;  int Height; void* parentHandle
        /// </summary>
        S_OPEN_LOCAL_RENDERER,

        /// <summary>
        /// 打开对方视频Renderer
        /// int Width;  int Height; void* parentHandle
        /// </summary>
        S_OPEN_REMOTE_RENDERER,
        // 视频音频

        // 录音

        /// <summary>
        /// 开始录音
        /// </summary>
        S_RECORD,

        /// <summary>
        /// 停止录音
        /// </summary>
        S_UNRECORD,
        //录音

        /// <summary>
        /// 调试信息  lpwstr info;  lpwstr content;
        /// </summary>
        S_DEBUG_INFO,

        //创建 Local Remote——SurfaceView
        /// <summary>
        /// Local_SurfaceView Remote_SurfaceView
        /// long local;  long remote;
        /// </summary>
        S_REGISTER_SV
    }

    //WH——Java CallBack接口函数

    //CallBack_Xmpp
    public void callBack_Xmpp(byte[] xmppData){
        //Log.i("myXmppData", xmppData);
    }

    //CallBack_Connection State
    public void callBack_Logging(){
        Message message = Message.obtain();
        message.what = RecievedMessage.R_LOGGING.ordinal();

        ProcessNativeCallBack(message);
    }

    public void callBack_Logged_In(String userId){
        Message message = Message.obtain();
        message.what = RecievedMessage.R_LOGGED_IN.ordinal();

        ProcessNativeCallBack(message);
    }

    public void callBack_Logged_Out(){
        Message message = Message.obtain();
        message.what = RecievedMessage.R_LOGGED_OUT.ordinal();

        ProcessNativeCallBack(message);
    }

    //CallBack_P2P File
    public void callBack_File_SessionID(String sId, String jid, String file){}

    public void callBack_File_Request(String sId, String jid, String description){}

    public void callBack_File_Accept(String sId){}

    public void callBack_File_Reject(String sId){}

    public void callBack_File_Terminate(String sId){}

    public void callBack_File_Progress(String sId, int progress){}

    //CallBack_Call
    public void callBack_Call(String jid) {
        Message message = Message.obtain();
        message.what = RecievedMessage.R_CALL.ordinal();

        Bundle bundle = new Bundle();
        bundle.putString("jid", jid);
        message.setData(bundle);

        ProcessNativeCallBack(message);
    }

    public void callBack_Call_Accept(){
        Message message = Message.obtain();
        message.what = RecievedMessage.R_CALL_ACCEPT.ordinal();

        ProcessNativeCallBack(message);
    }

    public void callBack_Call_Reject(){
        Message message = Message.obtain();
        message.what = RecievedMessage.R_CALL_REJECT.ordinal();

        ProcessNativeCallBack(message);
    }

    public void callBack_Call_Terminate() {
        Message message = Message.obtain();
        message.what = RecievedMessage.R_CALL_TERMINATE.ordinal();

        ProcessNativeCallBack(message);
    }

    public void callBack_Screencast(){}

    public void callBack_Camera() {
        Message message = Message.obtain();
        message.what = RecievedMessage.R_CAMERA.ordinal();
        ProcessNativeCallBack(message);
    }

    public void callBack_Camera_Stop(){
        Message message = Message.obtain();
        message.what = RecievedMessage.R_STOP_CAPTURE.ordinal();

        ProcessNativeCallBack(message);
    }

    public void callBack_Local_Frame_MetaData(){}

    public void callBack_Remote_Frame_MetaData(){}

    public void callBack_Local_Frame_Coming(){}

    public void callBack_Remote_Frame_Coming(){}

    //要求指针类型参数，待完成
    public void callBack_Local_Renderer_Handle(){}

    public void callBack_Remote_Renderer_Handle(){}

    //CallBack_Debug Info
    public void callBack_Debug_Info(String info, String content){}

    //CallBack_Test
    public void callBack_Test(String s){
        Log.i("mylogchat", s);
    }

    //回调到界面线程进行处理
    private void ProcessNativeCallBack(Message message){
        ((MainActivity) context).myHandler.sendMessage(message);
    }

    //WH——libjingle Native接口函数

    //注册 VideoCapture VideoRender VoiceEngine
    public native void Register(Context context);
    public native void unRegister();

    //创建 Local Remote——SurfaceView----//暂时废弃
    public native void CreateSurfaceView(Object local, Object remote);

    //创建WorkerThread
    public native void CreateWorkerThread();

    //登录
    public native void Login(String name, String password, String resource);

    //登出
    public native void Logout();

    //Xmpp
    public native void sendXmpp(String xmppData);

    //P2P File
    public native void sendFileRequest(String toJid, String fileFullPath, String fileDesc);
    public native void sendFileAccept(String sId, String fileName, String savePath);
    public native void sendFileReject(String sId);
    public native void sendFileTermintate(String sId);

    //Call
    public native void sendCall(String toJid, int local_Width, int local_Height, int remote_Width, int remote_Height);
    public native void sendCallAccept(int local_Width, int local_Height, int remote_Width, int remote_Height);
    public native void sendCallReject();

    public native void sendScreencastStart();
    public native void sendCameraStart();
    public native void sendCameraStop();
    //libjingle 未实现此命令
    public native void sendCameraRestart(int width, int height, int interval, int colorspace);

    //libjingle 未实现此命令
    public native void sendOpenLocal_Renderer(int videoWidth, int videoHeight, int parentHandle);
    public native void sendOpenRemote_Renderer(int videoWidth, int videoHeight, int parentHandle);

    public native void sendMute();
    public native void sendUnMute();
    public native void sendVideo_Mute();
    public native void sendVideo_Unmute();

    public native void sendSetRemoteVolume(String volume);

    public native void sendRecord();
    public native void sendUnRecord();

    public native void sendHangUp();
}

