/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/examples/android/media_demo/jni_wh/jni_helpers.h"

#include <limits>

#include "third_party/icu/source/common/unicode/unistr.h"

using icu::UnicodeString;

jmethodID GetMethodID(JNIEnv* jni, jclass c, const std::string& name,
                      const char* signature) {
  jmethodID m = jni->GetMethodID(c, name.c_str(), signature);
  CHECK_JNI_EXCEPTION(jni, "error during GetMethodID");
  return m;
}

jlong jlongFromPointer(void* ptr) {
  CHECK(sizeof(intptr_t) <= sizeof(jlong), "Time to rethink the use of jlongs");
  // Going through intptr_t to be obvious about the definedness of the
  // conversion from pointer to integral type.  intptr_t to jlong is a standard
  // widening by the COMPILE_ASSERT above.
  jlong ret = reinterpret_cast<intptr_t>(ptr);
  CHECK(reinterpret_cast<void*>(ret) == ptr,
        "jlong does not convert back to pointer");
  return ret;
}

// Given a (UTF-16) jstring return a new UTF-8 native string.
std::string JavaToStdString(JNIEnv* jni, const jstring& j_string) {
  const jchar* jchars = jni->GetStringChars(j_string, NULL);
  CHECK_JNI_EXCEPTION(jni, "Error during GetStringChars");
  UnicodeString ustr(jchars, jni->GetStringLength(j_string));
  CHECK_JNI_EXCEPTION(jni, "Error during GetStringLength");
  jni->ReleaseStringChars(j_string, jchars);
  CHECK_JNI_EXCEPTION(jni, "Error during ReleaseStringChars");
  std::string ret;
  return ustr.toUTF8String(ret);
}

//WH
jstring mystr2jstring(JNIEnv* env, std::string str)
{
   return env->NewStringUTF(str.c_str());
   //return str2jstring(env, str.c_str());
}

jstring str2jstring(JNIEnv* env,const char* pat)
{  
    //定义java String类 strClass  
    jclass strClass = (env)->FindClass("Ljava/lang/String;");  
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String  
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");  
    //建立byte数组  
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));  
    //将char* 转换为byte数组  
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);  
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数  
    jstring encoding = (env)->NewStringUTF("GB2312");   
    //将byte数组转换为java String,并输出  
    return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);  
}

jbyteArray str2jbyteArray(JNIEnv* env, const char* pat)
{
    //建立byte数组  
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));  
    //将char* 转换为byte数组  
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    return bytes;
}
      
std::string jstring2str(JNIEnv* env, jstring jstr)  
{     
    char*   rtn   =   NULL;     
    jclass   clsstring   =   env->FindClass("java/lang/String");     
    jstring   strencode   =   env->NewStringUTF("GB2312");     
    jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");     
    jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);     
    jsize   alen   =   env->GetArrayLength(barr);     
    jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);     
    if(alen   >   0)     
    {     
        rtn   =   (char*)malloc(alen+1);           
        memcpy(rtn,ba,alen);     
        rtn[alen]=0;     
    }     
    env->ReleaseByteArrayElements(barr,ba,0);     
    std::string stemp(rtn);  
    free(rtn);  
    return   stemp;     
}   


ClassReferenceHolder::ClassReferenceHolder(JNIEnv* jni, const char** classes,
                                           int size) {
  for (int i = 0; i < size; ++i) {
    LoadClass(jni, classes[i]);
  }
}
ClassReferenceHolder::~ClassReferenceHolder() {
  CHECK(classes_.empty(), "Must call FreeReferences() before dtor!");
}

void ClassReferenceHolder::FreeReferences(JNIEnv* jni) {
  for (std::map<std::string, jclass>::const_iterator it = classes_.begin();
       it != classes_.end(); ++it) {
    jni->DeleteGlobalRef(it->second);
  }
  classes_.clear();
}

jclass ClassReferenceHolder::GetClass(const std::string& name) {
  std::map<std::string, jclass>::iterator it = classes_.find(name);
  CHECK(it != classes_.end(), "Could not find class");
  return it->second;
}

void ClassReferenceHolder::LoadClass(JNIEnv* jni, const std::string& name) {
  jclass localRef = jni->FindClass(name.c_str());
  CHECK_JNI_EXCEPTION(jni, "Could not load class");
  CHECK(localRef, name.c_str());
  jclass globalRef = reinterpret_cast<jclass>(jni->NewGlobalRef(localRef));
  CHECK_JNI_EXCEPTION(jni, "error during NewGlobalRef");
  CHECK(globalRef, name.c_str());
  bool inserted = classes_.insert(std::make_pair(name, globalRef)).second;
  CHECK(inserted, "Duplicate class name");
}
