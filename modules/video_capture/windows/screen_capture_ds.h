/*
*  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef WEBRTC_MODULES_VIDEO_CAPTURE_MAIN_SOURCE_WINDOWS_SCREEN_CAPTURE_DS_H_
#define WEBRTC_MODULES_VIDEO_CAPTURE_MAIN_SOURCE_WINDOWS_SCREEN_CAPTURE_DS_H_

#include "webrtc/modules/video_capture/video_capture_impl.h"
#include "webrtc/base/thread.h"

namespace webrtc
{
	namespace videocapturemodule
	{
		class ScreenCaptureDS: public VideoCaptureImpl
		{
		public:
			ScreenCaptureDS(const int32_t id);

			virtual int32_t Init(const int32_t id, const char* deviceUniqueIdUTF8);

			/*************************************************************************
			*
			*   Start/Stop
			*
			*************************************************************************/
			virtual int32_t
				StartCapture(const VideoCaptureCapability& capability);
			virtual int32_t StopCapture();

			/**************************************************************************
			*
			*   Properties of the set device
			*
			**************************************************************************/

			virtual bool CaptureStarted();
			virtual int32_t CaptureSettings(VideoCaptureCapability& settings);
			bool FakeProcessCaptureFrame();

			class ScreencastThread
				: public rtc::Thread, public rtc::MessageHandler {
			public:
				explicit  ScreencastThread(ScreenCaptureDS* capturer);

				~ScreencastThread() ;

				// Override virtual method of parent Thread. Context: Worker Thread.
				void Run();

				// Override virtual method of parent MessageHandler. Context: Worker Thread.
				void OnMessage(rtc::Message* /*pmsg*/) ;

				// Check if Run() is finished.


			private:
				ScreenCaptureDS* capturer_;
			};


			~ScreenCaptureDS();
		private:

			ScreencastThread* thread_;
			bool isStarted_;

		};
	}  // namespace videocapturemodule
}  // namespace webrtc
#endif // WEBRTC_MODULES_VIDEO_CAPTURE_MAIN_SOURCE_WINDOWS_SCREEN_CAPTURE_DS_H_
