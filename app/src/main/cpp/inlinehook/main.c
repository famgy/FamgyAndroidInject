//
// Created by famgy on 8/21/19.
//

#include "inlineHook.h"
#include "impl/impl_close.h"
#include "impl/impl_open.h"

#include <android/log.h>
#include <unistd.h>

extern int __openat(int, const char*, int, int);

int (*old_puts)(const char *) = NULL;

int new_puts(const char *string)
{
    __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "puts");
    old_puts("inlineHook success");
}

int hook()
{
    if (registerInlineHook((uint32_t) puts, (uint32_t) new_puts, (uint32_t **) &old_puts) != ELE7EN_OK) {
        return -1;
    }
    if (inlineHook((uint32_t) puts) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

int unHook()
{
    if (inlineUnHook((uint32_t) puts) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

void __attribute__ ((constructor)) libhook__main()
{
    // register
#if defined(__arm__)
    __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "register");
    registerInlineHook((void *)__openat, (void *)__nativehook_impl_android_openat, (void **)&old_openat);
#elif defined(__aarch64__)
    registerInlineHook((void *)open, (void *)__nativehook_impl_android_open, (void **)&old_open);
#elif defined(__i386__) || defined(__x86_64__)
#endif

    registerInlineHook((void *)close, (void *)__nativehook_impl_android_close, (void **)&old_close);

    // hook
    __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "hook");
    inlineHookAll();

//    __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "puts start ");
//
//    puts("test");
//    hook();
//    puts("test");
//    unHook();
//    puts("test");
//
//    __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "puts end");
}
