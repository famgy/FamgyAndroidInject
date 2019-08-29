//
// Created by famgy on 18-12-3.
//

#include <android/log.h>
#include <string.h>


#include "impl_open.h"


int (*old_openat)(int, const char *, int, int) = NULL;
int (*old_open)(const char *pathname, int flags, mode_t mode) = NULL;

int g_fd = 0;

int __nativehook_impl_android_openat(int dirFd, const char *pathName, int flag, int mode)
{
    int realFd = -1;

    realFd = old_openat(dirFd, pathName, flag, mode);

    if (0 == strcmp("/data/user/0/com.famgy.famgyandroidinject/cache/pdf_test.pdf-pdfview.pdf", pathName)) {
    //if (0 == strcmp("/data/user/0/com.famgy.famgyandroid/cache/pdf_test.pdf-pdfview.pdf", pathName)) {
        g_fd = realFd;
        __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "__nativehook_impl_android_openat %s, fd = %d", pathName, realFd);
    }

    return realFd;
}

int __nativehook_impl_android_open(const char *pathName, int flag, int mode)
{
    if (0 == strcmp("/data/user/0/com.famgy.famgyandroidinject/cache/pdf_test.pdf-pdfview.pdf", pathName)) {
    //if (0 == strcmp("/data/user/0/com.famgy.famgyandroid/cache/pdf_test.pdf-pdfview.pdf", pathName)) {
        __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "__nativehook_impl_android_open %s\n", pathName);
    }

    int realFd = -1;

    realFd = old_open(pathName, flag, mode);


    return realFd;
}
