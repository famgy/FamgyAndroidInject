
#include <jni.h>
#include <android/log.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>

#include "traced.hpp"

#define DEBUG_FLAG "===InjectAndroid==="
#define JNI_API_DEF(f) Java_com_famgy_famgyandroidinject_MainActivity_##f

extern "C" {

JNIEXPORT jstring JNICALL JNI_API_DEF(injectAndroid)(JNIEnv *env, jobject obj, jint mPid) {
    std::string hello = "Hello from C++, injectAndroid";

    int ret = execl("/su/bin/su", "sh", "-c", "su");
    //int ret = execl("/system/bin/sh", "sh", "-c", "am start -a android.intent.action.MAIN -n   com.android.browser/.BrowserActivity", (char *)NULL);

    if( geteuid() != 0 ){
        __android_log_print(ANDROID_LOG_DEBUG, DEBUG_FLAG, "This script must be executed as root." );
        return env->NewStringUTF(hello.c_str());
    }

    pid_t pid = mPid;
    std::string library = "/data/local/tmp/libinlinehook.so";

    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_FLAG, "@ Ptracing from %d attach into process %d.", getpid(), pid);
    Traced *proc = new Traced(pid);

    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_FLAG, "@ Calling dlopen %s in target process ...",  library.c_str());
    unsigned long dlret = proc->dlopenInjectSo( library.c_str() );
    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_FLAG, "@ dlopen returned 0x%lX\n", dlret );

    proc->detachNow();
    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_FLAG, "@ detachNow returned");





    return env->NewStringUTF(hello.c_str());
}

}