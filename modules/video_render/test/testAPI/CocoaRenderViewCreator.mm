#include "webrtc/modules/video_render/test/testAPI/CocoaRenderViewCreator.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#import <QTKit/QTKit.h>


namespace webrtc {

CocoaRenderViewCreator::CocoaRenderViewCreator(){}

CocoaRenderViewCreator::~CocoaRenderViewCreator(){

[pool release];

}

int CocoaRenderViewCreator::WebRtcCreateWindow(CocoaRenderView*& cocoaRenderer, int winNum, int width, int height)
{
    // In Cocoa, rendering is not done directly to a window like in Windows and Linux.
    // It is rendererd to a Subclass of NSOpenGLView

    // create cocoa container window
    NSRect outWindowFrame = NSMakeRect(200, 800, width + 20, height + 20);
    NSWindow* outWindow = [[NSWindow alloc] initWithContentRect:outWindowFrame 
                                                      styleMask:NSTitledWindowMask 
                                                        backing:NSBackingStoreBuffered 
                                                          defer:NO];
    [outWindow orderOut:nil];
    [outWindow setTitle:@"Cocoa Renderer"];
    [outWindow setBackgroundColor:[NSColor blueColor]];

    // create renderer and attach to window
    NSRect cocoaRendererFrame = NSMakeRect(10, 10, width, height);
    cocoaRenderer = [[CocoaRenderView alloc] initWithFrame:cocoaRendererFrame];
    [[outWindow contentView] addSubview:(NSView*)cocoaRenderer];

    [outWindow makeKeyAndOrderFront:NSApp];

    return 0;
}

void* CocoaRenderViewCreator::init(int width, int height){
    pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];

    CocoaRenderView* testWindow;
    WebRtcCreateWindow(testWindow, 0, width, height);
    void* window = (void*)testWindow;

    return window;

}

}  // namespace webrtc
