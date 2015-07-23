/*
*  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include "webrtc/modules/video_capture/windows/screen_capture_ds.h"

namespace webrtc
{
	namespace videocapturemodule
	{
		class rtc::Thread;
		class ScreenCaptureDS::ScreencastThread;
		ScreenCaptureDS::ScreenCaptureDS(const int32_t id) : VideoCaptureImpl(id),thread_(NULL),isStarted_(false){}

		ScreenCaptureDS::~ScreenCaptureDS()
		{
			if (thread_)
				delete thread_;
			thread_=NULL;
		}

		int32_t ScreenCaptureDS::Init(const int32_t id, const char* deviceUniqueIdUTF8)
		{
			return 0;
		}

		int32_t ScreenCaptureDS::StartCapture(const VideoCaptureCapability& capability)
		{
			if (!thread_)
			{
				thread_=new ScreencastThread(this);
			}

			if(!thread_)return 1;
			
			thread_->Start();			
			isStarted_=true;
			return 0;
		}

		int32_t ScreenCaptureDS::StopCapture()
		{
			if (!thread_)return 1;
			
			thread_->Stop();
			isStarted_=false;
			return 0;
		}
		bool ScreenCaptureDS::CaptureStarted()
		{
			return isStarted_;
		}
		int32_t ScreenCaptureDS::CaptureSettings(VideoCaptureCapability& settings)
		{
			settings = _requestedCapability;
			return 0;
		}
		bool ScreenCaptureDS::FakeProcessCaptureFrame()
		{
			HDC hDCScreen;
			HDC hDCMem;
			HBITMAP hBitmap;
			LPVOID data;
			__try{
				BITMAPINFO bmi_;
				hDCScreen=::CreateDC(L"DISPLAY",NULL,NULL, NULL);
				if(!hDCScreen)return false;
				hDCMem=::CreateCompatibleDC(hDCScreen);
				if(!hDCMem)return false;
				int nWidth=GetSystemMetrics(SM_CXSCREEN);
				int nHeight=GetSystemMetrics(SM_CYSCREEN);
				hBitmap=::CreateCompatibleBitmap(hDCScreen,nWidth,nHeight);
				if(!hBitmap)return false;
				HBITMAP oldhBitmap=(HBITMAP)::SelectObject(hDCMem,hBitmap);
				if(!oldhBitmap)return false;
				::BitBlt(hDCMem,0,0,nWidth,nHeight,hDCScreen,0,0,SRCCOPY);
				
				
				
				HCURSOR hcursor=::GetCursor();
				ICONINFO iconinfo;
				POINT p;
				
				::GetCursorPos(&p);
				if(GetIconInfo(hcursor,&iconinfo))
				{	
					p.x-=iconinfo.xHotspot;
					p.y-=iconinfo.yHotspot;
					if(iconinfo.hbmColor)DeleteObject(iconinfo.hbmColor);
					if(iconinfo.hbmMask)DeleteObject(iconinfo.hbmMask);
					::DrawIconEx(hDCMem,p.x,p.y,hcursor,0,0,0,NULL,DI_NORMAL|DI_COMPAT);
				}
				
				
				::SelectObject(hDCMem,oldhBitmap);

				BITMAP bm;
				::GetObject(hBitmap,sizeof(BITMAP),&bm);
				DWORD imageSize=bm.bmWidthBytes*bm.bmHeight;
				data=malloc(imageSize);//GlobalAlloc(GRTP,imageSize);
				if(!data)return false;
				bmi_.bmiHeader.biWidth = bm.bmWidth;
				bmi_.bmiHeader.biCompression=BI_RGB ;
				bmi_.bmiHeader.biHeight = bm.bmHeight;
				bmi_.bmiHeader.biBitCount=bm.bmBitsPixel;
				bmi_.bmiHeader.biPlanes=1;
				bmi_.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
				bmi_.bmiHeader.biSizeImage = imageSize;
				if(GetDIBits(hDCMem,hBitmap,0,bmi_.bmiHeader.biHeight,data,&bmi_,DIB_RGB_COLORS)==0)return false;

				VideoCaptureCapability fake_frameinfo;

				fake_frameinfo.width=bmi_.bmiHeader.biWidth;
				fake_frameinfo.height=-bmi_.bmiHeader.biHeight;
				fake_frameinfo.rawType=kVideoARGB;
				fake_frameinfo.codecType=webrtc::kVideoCodecUnknown;
				fake_frameinfo.maxFPS=0;
				fake_frameinfo.interlaced=false;
				fake_frameinfo.expectedCaptureDelay=0;


				this->IncomingFrame((uint8_t*)data, imageSize, fake_frameinfo);
				return true;
			}

			__finally{
				::free(data);
				::DeleteDC(hDCMem);
				::DeleteObject(hBitmap);
				::DeleteDC(hDCScreen);
			}


		}

		ScreenCaptureDS::ScreencastThread::ScreencastThread(ScreenCaptureDS* capturer)
			: capturer_(capturer)
		{	}

		ScreenCaptureDS::ScreencastThread::~ScreencastThread() {
			Stop();
		}

		// Override virtual method of parent Thread. Context: Worker Thread.
		void ScreenCaptureDS::ScreencastThread::Run() {
			// Read the first frame and start the message pump. The pump runs until
			// Stop() is called externally or Quit() is called by OnMessage().
			int waiting_time_ms = 100;
			if (capturer_ && capturer_->FakeProcessCaptureFrame()) {
				PostDelayed(waiting_time_ms, this);
				Thread::Run();
			}

		}

		// Override virtual method of parent MessageHandler. Context: Worker Thread.
		void ScreenCaptureDS::ScreencastThread::OnMessage(rtc::Message* /*pmsg*/) {
			int waiting_time_ms = 100;
			if (capturer_ && capturer_->FakeProcessCaptureFrame()) {
				PostDelayed(waiting_time_ms, this);
			} else {
				Quit();
			}
		}
	}  // namespace videocapturemodule
}  // namespace webrtc
