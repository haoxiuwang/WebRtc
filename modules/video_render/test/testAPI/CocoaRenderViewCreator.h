#ifndef COCOARENDERVIEWCREATOR
#define COCOARENDERVIEWCREATOR

#include <iostream>

#include <sys/time.h>

#import "webrtc/modules/video_render/mac/cocoa_render_view.h"
#include "webrtc/common_types.h"
#include "webrtc/modules/interface/module_common_types.h"
#include "webrtc/modules/utility/interface/process_thread.h"
#include "webrtc/modules/video_render/include/video_render.h"
#include "webrtc/modules/video_render/include/video_render_defines.h"


namespace webrtc {

class CocoaRenderViewCreator
{
public:

    CocoaRenderViewCreator();

    ~CocoaRenderViewCreator();

    int WebRtcCreateWindow(CocoaRenderView*& cocoaRenderer, int winNum, int width, int height);

    void* init(int width, int height);

private:
    NSAutoreleasePool * pool;

};

}  // namespace webrtc

#endif  // COCOARENDERVIEWCREATOR
