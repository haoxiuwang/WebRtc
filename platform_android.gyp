{
  'includes': ['../build/common.gypi'],
   'variables': {
    'talk_root%': '<(webrtc_root)/../talk',
  },
  'targets': [
        { # WH mylibjingle-jni
          'target_name': 'libmylibjingle-jni',
          'type': 'loadable_module',
          'dependencies': [
	         '<(talk_root)/libjingle.gyp:libjingle_p2p',
             '<(webrtc_root)/libjingle/libjingle.gyp:libjingle_xmpphelp',
             '<(webrtc_root)/libjingle/libjingle.gyp:jingle_session',
             '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
             '<(webrtc_root)/system_wrappers/system_wrappers.gyp:system_wrappers_default',
          ],
          'sources': [
             '<(webrtc_root)/examples/android/my_android/jni_android/jni_helpers.h',
             '<(webrtc_root)/examples/android/my_android/jni_android/jni_helpers.cc',
             '<(webrtc_root)/libjingle/examples/call/talk_call_android.h',
             '<(webrtc_root)/examples/android/my_android/jni_android/talk_call_android.cc',

             '<(webrtc_root)/libjingle/examples/call/console_dll.h',
             '<(webrtc_root)/libjingle/examples/call/console_dll.cc',

             '<(webrtc_root)/libjingle/examples/call/talk_call.h',
             '<(webrtc_root)/libjingle/examples/call/talk_call.cc',
             '<(webrtc_root)/libjingle/examples/call/callclient.cc',
             '<(webrtc_root)/libjingle/examples/call/callclient.h',
             '<(webrtc_root)/libjingle/examples/call/console.cc',
             '<(webrtc_root)/libjingle/examples/call/console.h',
             '<(webrtc_root)/libjingle/examples/call/PublicCallback.h',
             '<(webrtc_root)/libjingle/examples/call/PublicCallback.cc',
             '<(webrtc_root)/libjingle/examples/call/friendinvitesendtask.cc',
             '<(webrtc_root)/libjingle/examples/call/friendinvitesendtask.h',
             '<(webrtc_root)/libjingle/examples/call/muc.h',
             '<(webrtc_root)/libjingle/examples/call/mucinviterecvtask.cc',
             '<(webrtc_root)/libjingle/examples/call/mucinviterecvtask.h',
             '<(webrtc_root)/libjingle/examples/call/mucinvitesendtask.cc',
             '<(webrtc_root)/libjingle/examples/call/mucinvitesendtask.h',
             '<(webrtc_root)/libjingle/examples/call/presencepushtask.cc',
             '<(webrtc_root)/libjingle/examples/call/presencepushtask.h',
          ],
          'variables': {
            # This library uses native JNI exports; tell GYP so that the
            # required symbols will be kept.
            'use_native_jni_exports': 1,
          },
          'link_settings': {
            'libraries': [
              '-llog',
              '-lGLESv2',
              '-lOpenSLES',
            ],
          }
        }, #end mylibjingle-jni

    { #test-jni
	  'target_name': 'libtest-jni',
          'type': 'loadable_module',
          'dependencies': [
            '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
            '<(webrtc_root)/system_wrappers/system_wrappers.gyp:system_wrappers_default',
                  ],
          'sources': [
            '<(webrtc_root)/examples/android/my_test/jni_test/test_load.cc',
	             ],
          'variables': {
            'use_native_jni_exports': 1,
          },
          'link_settings': {
            'libraries': [
              '-llog',
              '-lGLESv2',
              '-lOpenSLES',
            ],
          }
	}, #end test-jni

    {
          'target_name': 'MylibjingleTest',
          'type': 'none',
          'dependencies': [
            #'libtest-jni',
	        'libmylibjingle-jni',
            '<(modules_java_gyp_path):*',
          ],
          'actions': [
            {
              'action_name': 'build_mylibjingletest_apk',
              'variables': {
                'android_my_libjingle_test_root': '<(webrtc_root)/examples/android/my_android',
                'ant_log': '../../../../<(INTERMEDIATE_DIR)/ant.log', # ../../../.. to compensate for the cd below.
              },
              'inputs' : [
                '<(PRODUCT_DIR)/lib.java/audio_device_module_java.jar',
                '<(PRODUCT_DIR)/lib.java/video_capture_module_java.jar',
                '<(PRODUCT_DIR)/lib.java/video_render_module_java.jar',
                '<(PRODUCT_DIR)/libmylibjingle-jni.so',
                '<!@(find <(android_my_libjingle_test_root)/src -name "*.java")',
                '<!@(find <(android_my_libjingle_test_root)/res -type f)',
                '<(android_my_libjingle_test_root)/AndroidManifest.xml',
                '<(android_my_libjingle_test_root)/build.xml',
                '<(android_my_libjingle_test_root)/project.properties',
              ],
              'outputs': ['<(PRODUCT_DIR)/MylibjingleTest-debug.apk'],
              'action': [
                'bash', '-ec',
                'rm -fr <(_outputs) <(android_my_libjingle_test_root)/{bin,libs,gen,obj} && '
                'mkdir -p <(INTERMEDIATE_DIR) && ' # Must happen _before_ the cd below
                'mkdir -p <(android_my_libjingle_test_root)/libs/<(android_app_abi) && '
                'cp <(PRODUCT_DIR)/lib.java/audio_device_module_java.jar <(android_my_libjingle_test_root)/libs/ &&'
                'cp <(PRODUCT_DIR)/lib.java/video_capture_module_java.jar <(android_my_libjingle_test_root)/libs/ &&'
                'cp <(PRODUCT_DIR)/lib.java/video_render_module_java.jar <(android_my_libjingle_test_root)/libs/ &&'
                '<(android_strip) -o <(android_my_libjingle_test_root)/libs/<(android_app_abi)/libmylibjingle-jni.so <(PRODUCT_DIR)/libmylibjingle-jni.so && '
                'cd <(android_my_libjingle_test_root) && '
                '{ ANDROID_SDK_ROOT=<(android_sdk_root) '
                'ant debug > <(ant_log) 2>&1 || '
                '  { cat <(ant_log) ; exit 1; } } && '
                'cd - > /dev/null && '
                'cp <(android_my_libjingle_test_root)/bin/MylibjingleTest-debug.apk <(_outputs)'
              ],
            },
          ],
        },
      ],
    }
