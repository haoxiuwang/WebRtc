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
        // ����XmppЭ��

        /// <summary>
        /// �յ���libjingle�����ת����XmppЭ��
        /// lpstr XmppProtocol;  int ByteLength
        /// </summary>
        R_XMPP_PROTOCOL,
        // ����XmppЭ��

        // ��½״̬

        /// <summary>
        /// ���ڵ�½
        /// </summary>
        R_LOGGING,

        /// <summary>
        /// ����ɹ������ش���Դ��Jid
        /// lpwstr FullJid
        /// </summary>
        R_LOGGED_IN,

        /// <summary>
        /// �ӷ������ǳ�
        /// </summary>
        R_LOGGED_OUT,
        // ��½״̬

        // �ļ�����

        /// <summary>
        /// �ļ����ͷ��Ѵ���Session���������SessionId
        /// lpwstr SessionId;  lpwstr Jid;  lpwstr FullFileName;
        /// </summary>
        R_FILE_SESSION_ID,

        /// <summary>
        /// �Է��򼺷������ļ�
        /// lpwstr SessionId;  lpwstr Jid;  lpwstr FileDescription;
        /// </summary>
        R_FILE_REQUEST,

        /// <summary>
        /// �Է������ļ�
        /// lpwstr SessionId;
        /// </summary>
        R_FILE_ACCEPT,

        /// <summary>
        /// �Է��ܾ��ļ�
        /// lpwstr SessionId;
        /// </summary>
        R_FILE_REJECT,

        /// <summary>
        /// �Է���ֹ�����ļ�
        /// lpwstr SessionId;
        /// </summary>
        R_FILE_TERMINATE,

        /// <summary>
        /// ��ȡ�ļ�����
        /// lpwstr SessionId;  int Progress
        /// </summary>
        R_FILE_PROGRESS,
        // �ļ�����

        // ��Ƶ��Ƶ

        // �ỰЭ��

        /// <summary>
        /// �Է�����Call����
        /// lpwstr Jid;
        /// </summary>
        R_CALL,

        /// <summary>
        /// �Է�����Call
        /// </summary>
        R_CALL_ACCEPT,

        /// <summary>
        /// �Է��ܾ�Call
        /// </summary>
        R_CALL_REJECT,

        /// <summary>
        /// �Է���ֹCall
        /// </summary>
        R_CALL_TERMINATE,
        // �ỰЭ��

        /// <summary>
        /// �Է�����Զ������
        /// </summary>
        R_SCREENCAST,

        /// <summary>
        /// �Է���������ͷ����
        /// </summary>
        R_CAMERA,

        /// <summary>
        ///  ��ñ��ص���Ƶ֡
        ///  void* ֡buffer int width int height
        /// </summary>
        R_LOCAL_FRAME_METADATA,

        /// <summary>
        ///  ��öԷ�����Ƶ֡
        ///  void* ֡buffer int width int height
        /// </summary>
        R_REMOTE_FRAME_METADATA,

        /// <summary>
        /// �Է��ص���Ƶ����
        /// </summary>
        R_STOP_CAPTURE,

        /// <summary>
        /// ���˴���������ƵRenderer
        /// ���ھ�� void* handle
        /// </summary>
        R_LOCAL_RENDERER_HANDLE,

        /// <summary>
        /// ���˴����Է���ƵRenderer
        /// ���ھ�� void* handle
        /// </summary>
        R_REMOTE_RENDERER_HANDLE,

        /// <summary>
        ///  ��ñ��ص���Ƶ֡
        /// </summary>
        R_LOCAL_FRAME_COMING,

        /// <summary>
        ///  ��öԷ�����Ƶ֡
        /// </summary>
        R_REMOTE_FRAME_COMING,
        //��Ƶ��Ƶ

        /// <summary>
        /// ������Ϣ  lpwstr info;  lpwstr content;
        /// </summary>
        R_DEBUG_INFO
    }

    public enum SendMessage
    {
        // ����XmppЭ��

        /// <summary>
        /// ����libjingle�����ת��xmppЭ��
        /// lpstr XmppProtocl
        /// </summary>
        S_XMPPP_ROTOCOL,

        /// <summary>
        /// ����stun��������IP��ַ���˿�
        /// lpstr ip  int port
        /// </summary>
        S_STUN_ADDRESS,
        // ����XmppЭ��

        // ��½״̬

        /// <summary>
        /// �ӷ������ǳ�
        /// </summary>
        S_LOGOUT,
        //��½״̬

        // �ļ�����

        /// <summary>
        /// �����ļ�
        /// lpstr RecieverJid;  lpstr FullFileName;  lpstr FileDescription;
        /// </summary>
        S_FILE_REQUEST,

        /// <summary>
        /// �����ļ�
        /// lpstr SessionId;  lpstr FileName;  lpstr SavePath;
        /// </summary>
        S_FILE_ACCEPT,

        /// <summary>
        /// �ܾ������ļ�
        /// lpstr SessionId;
        /// </summary>
        S_FILE_REJECT,

        /// <summary>
        /// ��ֹ�����ļ�
        /// lpstr SessionId;
        /// </summary>
        S_FILE_TERMINTATE,
        //�ļ�����

        // ��Ƶ��Ƶ

        // �ỰЭ��

        /// <summary>
        /// Call����
        /// lpstr RecieveJid; int localWidth;  int localHeight; void* localpHandle;  int remoteWidth;  int remoteHeight; void* remotepHandle
        /// </summary>
        S_CALL,

        /// <summary>
        /// ����Call
        /// int localWidth;  int localHeight; void* localpHandle;  int remoteWidth;  int remoteHeight; void* remotepHandle
        /// </summary>
        S_CALL_ACCEPT,

        /// <summary>
        /// �ܾ�Call
        /// </summary>
        S_CALL_REJECT,

        /// <summary>
        /// ��ֹ��ɾ���Ự
        /// </summary>
        S_HANG_UP,
        // �ỰЭ��

        /// <summary>
        /// ֹͣ��������Ƶ����
        /// </summary>
        S_MUTE,

        /// <summary>
        /// ���¿�ʼ��������Ƶ����
        /// </summary>
        S_UNMUTE,

        /// <summary>
        /// ���ƶԷ������������Ƶ����
        /// lpstr Volume
        /// </summary>
        S_SET_REMOTE_VOLUME,

        /// <summary>
        /// ֹͣ��������Ƶ����
        /// </summary>
        S_VIDEO_MUTE,

        /// <summary>
        /// ���¿�ʼ��������Ƶ����
        /// </summary>
        S_VIDEO_UNMUTE,

        /// <summary>
        /// ����Զ������
        /// </summary>
        S_START_SCREENCAST,

        /// <summary>
        /// ��������ͷ����
        /// </summary>
        S_START_CAMERA,

        /// <summary>
        /// ֹͣ������Ƶ
        /// </summary>
        S_STOP_CAPTURE,

        /// <summary>
        /// ���貶�����Ƶ����
        /// int Width;  int Height;  int Interval;  int ColorSpace;
        /// </summary>
        S_RESTART_CAPTURE,

        /// <summary>
        /// �򿪱�����ƵRenderer
        /// int Width;  int Height; void* parentHandle
        /// </summary>
        S_OPEN_LOCAL_RENDERER,

        /// <summary>
        /// �򿪶Է���ƵRenderer
        /// int Width;  int Height; void* parentHandle
        /// </summary>
        S_OPEN_REMOTE_RENDERER,
        // ��Ƶ��Ƶ

        // ¼��

        /// <summary>
        /// ��ʼ¼��
        /// </summary>
        S_RECORD,

        /// <summary>
        /// ֹͣ¼��
        /// </summary>
        S_UNRECORD,
        //¼��

        /// <summary>
        /// ������Ϣ  lpwstr info;  lpwstr content;
        /// </summary>
        S_DEBUG_INFO,

        //���� Local Remote����SurfaceView
        /// <summary>
        /// Local_SurfaceView Remote_SurfaceView
        /// long local;  long remote;
        /// </summary>
        S_REGISTER_SV
    }

    //WH����Java CallBack�ӿں���

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

    //Ҫ��ָ�����Ͳ����������
    public void callBack_Local_Renderer_Handle(){}

    public void callBack_Remote_Renderer_Handle(){}

    //CallBack_Debug Info
    public void callBack_Debug_Info(String info, String content){}

    //CallBack_Test
    public void callBack_Test(String s){
        Log.i("mylogchat", s);
    }

    //�ص��������߳̽��д���
    private void ProcessNativeCallBack(Message message){
        ((MainActivity) context).myHandler.sendMessage(message);
    }

    //WH����libjingle Native�ӿں���

    //ע�� VideoCapture VideoRender VoiceEngine
    public native void Register(Context context);
    public native void unRegister();

    //���� Local Remote����SurfaceView----//��ʱ����
    public native void CreateSurfaceView(Object local, Object remote);

    //����WorkerThread
    public native void CreateWorkerThread();

    //��¼
    public native void Login(String name, String password, String resource);

    //�ǳ�
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
    //libjingle δʵ�ִ�����
    public native void sendCameraRestart(int width, int height, int interval, int colorspace);

    //libjingle δʵ�ִ�����
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

