#include <jni.h>
#include <string>
#include "test.h"


extern "C"
//jstring to char*
char* jstringTostring(JNIEnv* env, jstring jstr)
{
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

extern "C"
jstring
Java_com_example_admin_ndkapplication_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    test t;
    char buf[20];
    sprintf(buf, "%d", t.func());
    std::string hello = "Hello from C++";
    hello += buf;
    return env->NewStringUTF(hello.c_str());
}

/**
 * 启动Service
 */
extern "C"
void Java_com_example_admin_ndkapplication_NativeRuntime_startService(JNIEnv* env, jobject thiz,
                                                  jstring cchrptr_ProcessName, jstring sdpath) {
    char * rtn = jstringTostring(env, cchrptr_ProcessName); // 得到进程名称
    char * sd = jstringTostring(env, sdpath);
    start(1, rtn, sd);
}