package org.webrtc.webrtcdemo;

import android.os.Message;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import android.os.Handler;

import org.webrtc.videoengine.ViEAndroidGLES20;
import org.webrtc.videoengine.ViERenderer;
import org.webrtc.videoengine.VideoCaptureAndroid;
import org.webrtc.videoengine.ViESurfaceRenderer;
import android.widget.LinearLayout;
import android.widget.TextView;


public class MainActivity extends Activity implements View.OnClickListener {

    private NativeWebRtcTransport nativeWebRtc = null;
    public Thread loginThread;
    public SurfaceView svLocal;
    public SurfaceView svRemote;
    public TextView vcallState;
    public Handler myHandler;

    // Remote and local stream displays.
    public LinearLayout llRemoteSurface;
    public LinearLayout llLocalSurface;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        vcallState = (TextView)findViewById(R.id.VcallState);

        llLocalSurface = (LinearLayout)this.findViewById(R.id.llLocalView);
        llRemoteSurface = (LinearLayout)this.findViewById(R.id.llRemoteView);

        nativeWebRtc = new NativeWebRtcTransport(this);
        MyWorkerThread();
	CreateLRSurfaceView();

        ProcessCallBack();

        Button b_login = (Button)findViewById(R.id.Login);
        Button b_sendvcall = (Button)findViewById(R.id.SendVCall);
        Button b_startcamera = (Button)findViewById(R.id.StartCamera);
        Button b_stopcamera = (Button)findViewById(R.id.StopCamera);
        Button b_accept = (Button)findViewById(R.id.Accept);
        Button b_reject = (Button)findViewById(R.id.Reject);
        Button b_mute = (Button)findViewById(R.id.Mute);
        Button b_unmute = (Button)findViewById(R.id.UnMute);
        Button b_vmute = (Button)findViewById(R.id.VMute);
        Button b_vunmute = (Button)findViewById(R.id.VUnMute);
        Button b_hangup = (Button)findViewById(R.id.HangUp);


        b_login.setOnClickListener(this);
        b_sendvcall.setOnClickListener(this);
        b_startcamera.setOnClickListener(this);
        b_stopcamera.setOnClickListener(this);
        b_accept.setOnClickListener(this);
        b_reject.setOnClickListener(this);
        b_mute.setOnClickListener(this);
        b_unmute.setOnClickListener(this);
        b_vmute.setOnClickListener(this);
        b_vunmute.setOnClickListener(this);
        b_hangup.setOnClickListener(this);
    }

    private void ProcessCallBack(){
        myHandler = new Handler() {
            public void handleMessage(Message msg) {
                NativeWebRtcTransport.RecievedMessage rm = NativeWebRtcTransport.RecievedMessage.values()[msg.what];
                switch(rm)
                {
                    //Xmpp
                    case R_XMPP_PROTOCOL:
		                /* 参数描述
		                   string xmppData
		                */
                        break;

                    //Connection State
                    case R_LOGGING:
                        vcallState.setText("登陆中…！");
                        break;
                    case R_LOGGED_IN:
		                /* 参数描述
		                   int ConnectionState
		                   string userJid
		                */
                        vcallState.setText("登陆完成！");
                        break;
                    case R_LOGGED_OUT:
                        vcallState.setText("已登出！");
                        break;

                    //P2P File
                    case R_FILE_SESSION_ID:
		                /* 参数描述
		                   string sId
		                   string jid
		                   string file
		                */
                        break;
                    case R_FILE_REQUEST:
                        /* 参数描述
		                   string sId
		                   string jid
		                   string description
		                */
                        break;
                    case R_FILE_ACCEPT:
		                /* 参数描述
		                   string sId
		                */
                        break;
                    case R_FILE_REJECT:
		                /* 参数描述
		                   string sId
		                */
                        break;
                    case R_FILE_TERMINATE:
		                /* 参数描述
		                   string sId
		                */
                        break;
                    case R_FILE_PROGRESS:
		                /* 参数描述
		                     string sId
		                    int progress
		                */
                        break;

                    //Call
                    case R_CALL:
                        /* 参数描述
		                     string jid
		                */
                        vcallState.setText("有人请求通话！");
                        break;
                    case R_CALL_ACCEPT:
                        vcallState.setText("对方已接受！");
                        break;
                    case R_CALL_REJECT:
                        vcallState.setText("对方已拒绝！");
                        break;
                    case R_CALL_TERMINATE:
                        vcallState.setText("远程结束通话！");
                        svRemote.setVisibility(View.GONE);
                        break;
                    case R_SCREENCAST:
                        break;
                    case R_CAMERA:
                        vcallState.setText("远程开启摄像头！");
                        break;
                    case R_STOP_CAPTURE:
                        vcallState.setText("远程关闭摄像头！");
                        svRemote.setVisibility(View.GONE);
                        break;
                    case R_LOCAL_FRAME_METADATA:
                        break;
                    case R_REMOTE_FRAME_METADATA:
                        break;
                    case R_LOCAL_FRAME_COMING:
                        break;
                    case R_REMOTE_FRAME_COMING:
                        break;

                    //要求指针类型参数，待完成
                    case R_LOCAL_RENDERER_HANDLE:
                        break;
                    case R_REMOTE_RENDERER_HANDLE:
                        break;

                    //Debug Info
                    case R_DEBUG_INFO:
		                /* 参数描述
		                   string info
		                   string content
		                */
                        break;
                }
                super.handleMessage(msg);
            }
        };
    }

    private void MyWorkerThread(){
        Thread workerthread = new Thread(new Runnable() {
            @Override
            public void run() {
                nativeWebRtc.CreateWorkerThread();
            }
        });
        workerthread.start();
    }


    private void CreateLRSurfaceView(){
	svLocal = new SurfaceView(MainActivity.this);
        //svLocal = ViERenderer.CreateRenderer(MainActivity.this, true);

	//svLocal.setVisibility(View.GONE);
        VideoCaptureAndroid.setLocalPreview(svLocal.getHolder());
        llLocalSurface.addView(svLocal);

	svRemote = new SurfaceView(MainActivity.this);
        //svRemote = ViERenderer.CreateRenderer(MainActivity.this, true);
        
        svRemote.setVisibility(View.GONE);
        ViESurfaceRenderer.setRemotePreview(svRemote);
        llRemoteSurface.addView(svRemote);

	//nativeWebRtc.CreateSurfaceView(svLocal, svRemote);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.Login:
                loginThread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        nativeWebRtc.Register(MainActivity.this);
                        nativeWebRtc.Login("wanghao@idreamhongkong.com", "wanghao", "call");
                    }
                });
                loginThread.start();
                break;
            case R.id.SendVCall:
                nativeWebRtc.sendCall("wanghaoxiu@idreamhongkong.com/call", 100, 100, 100, 100);
                vcallState.setText("发送请求！");
                break;
            case R.id.StartCamera:
                nativeWebRtc.sendCameraStart();
                break;
            case R.id.StopCamera:
                nativeWebRtc.sendCameraStop();
                break;
            case R.id.Accept:
                nativeWebRtc.sendCallAccept(100, 100, 100, 100);
                vcallState.setText("已接受，通话中...");
                break;
            case R.id.Reject:
                nativeWebRtc.sendCallReject();
                vcallState.setText("拒绝请求！");
                break;
            case R.id.Mute:
                nativeWebRtc.sendMute();
                break;
            case R.id.UnMute:
                nativeWebRtc.sendUnMute();
                break;
            case R.id.VMute:
                nativeWebRtc.sendVideo_Mute();
                break;
            case R.id.VUnMute:
                nativeWebRtc.sendVideo_Unmute();
                break;
            case R.id.HangUp:
                nativeWebRtc.sendHangUp();

                svRemote.setVisibility(View.GONE);
                vcallState.setText("已挂断！");
                break;
        }
    }

    @Override
    public void onDestroy(){
        llLocalSurface.removeView(svLocal);
        llRemoteSurface.removeView(svRemote);
        nativeWebRtc.unRegister();
        super.onDestroy();
    }
}
