/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

package org.webrtc.webrtcdemo;

//以下代码完成后复制到Ubuntu——webrtc对应文件下
public class NativeWebRtcTransport {
  static {
    System.loadLibrary("webrtcdemo-jni");
  }

  public native void Login(String name, String password);
  public native void SendCommand(int command, String data);

  //WH——libjingle回调入口
  public void webrtcCallback(int com, String data){

  }
}