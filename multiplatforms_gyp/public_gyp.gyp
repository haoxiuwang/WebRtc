# Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

{
  'includes': ['../build/common.gypi'],
  'variables': {
    'talk_root%': '<(webrtc_root)/../talk',
  },
  'targets': [

  { # target call
      'target_name': 'public_call',
           'type': 'none',
           'dependencies': [
             '<(talk_root)/libjingle.gyp:libjingle_p2p',
             '<(webrtc_root)/libjingle/libjingle.gyp:libjingle_xmpphelp',
             '<(webrtc_root)/libjingle/libjingle.gyp:jingle_session',],
           'sources': [
             '<(webrtc_root)/libjingle/examples/call/callclient.cc',
             '<(webrtc_root)/libjingle/examples/call/callclient.h',
             '<(webrtc_root)/libjingle/examples/call/console.cc',
             '<(webrtc_root)/libjingle/examples/call/console.h',
             '<(webrtc_root)/libjingle/examples/call/CommonUtilities.h',
             '<(webrtc_root)/libjingle/examples/call/CommonUtilities.cc',
             '<(webrtc_root)/libjingle/examples/call/PublicCallback.h',
             '<(webrtc_root)/libjingle/examples/call/PublicCallback.cc',
             '<(webrtc_root)/libjingle/examples/call/friendinvitesendtask.cc',
             '<(webrtc_root)/libjingle/examples/call/friendinvitesendtask.h',
             #¡¯<(webrtc_root)/libjingle/examples/call/mediaenginefactory.cc',
             #¡¯<(webrtc_root)/libjingle/examples/call/mediaenginefactory.h',
             '<(webrtc_root)/libjingle/examples/call/muc.h',
             '<(webrtc_root)/libjingle/examples/call/mucinviterecvtask.cc',
             '<(webrtc_root)/libjingle/examples/call/mucinviterecvtask.h',
             '<(webrtc_root)/libjingle/examples/call/mucinvitesendtask.cc',
             '<(webrtc_root)/libjingle/examples/call/mucinvitesendtask.h',
             '<(webrtc_root)/libjingle/examples/call/presencepushtask.cc',
             '<(webrtc_root)/libjingle/examples/call/presencepushtask.h',
           ],
           'conditions': [
             ['OS=="linux"', {
               'link_settings': {
                 'libraries': [
                   '<!@(pkg-config --libs-only-l gobject-2.0 gthread-2.0'
                       ' gtk+-2.0)',
                 ],
               },
             }],
             ['OS=="win"', {
               'msvs_settings': {
                 'VCLinkerTool': {
                   'AdditionalDependencies': [
                     'strmiids.lib',
                   ],
                 },
               },
             }],
           ],  # conditions
  }, # target call end

  { # target libmycall
           'target_name': 'libmycall',
           'type': 'static_library',
           'dependencies': [
              'public_call',],
           'sources': [
             '<(webrtc_root)/libjingle/examples/call/talk_call.h',
             '<(webrtc_root)/libjingle/examples/call/talk_call.cc',
             '<(webrtc_root)/libjingle/examples/call/console_dll.cc',
             '<(webrtc_root)/libjingle/examples/call/console_dll.h',

             '<(webrtc_root)/libjingle/examples/call/talk_call_mac.h',
             '<(webrtc_root)/libjingle/examples/call/talk_call_mac.mm',
             '<(webrtc_root)/libjingle/examples/call/ARDP2PCallback.h',
             '<(webrtc_root)/libjingle/examples/call/ARDP2PCallback.mm',
           ],
  }, # target libmycall end



  ],
}
