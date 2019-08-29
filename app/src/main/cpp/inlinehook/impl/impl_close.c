//
// Created by famgy on 18-12-3.
//

#include <android/log.h>
#include "impl_close.h"
#include "impl_open.h"

int (*old_close)(int fd) = NULL;

int __nativehook_impl_android_close(int fd) {
    if (g_fd != 0 &&fd == g_fd) {
        __android_log_print(ANDROID_LOG_DEBUG, "===inlinehook===", "__nativehook_impl_android_close, fd = %d", fd);
        g_fd = 0;
    }

    int result = old_close(fd);

    return result;
}