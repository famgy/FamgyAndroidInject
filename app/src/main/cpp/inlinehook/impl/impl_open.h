//
// Created by famgy on 18-12-3.
//

#ifndef IMPL_OPEN_H
#define IMPL_OPEN_H


#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int g_fd;

extern int (*old_openat)(int, const char *, int, int);
extern int (*old_open)(const char *pathname, int flags, mode_t mode);

extern int __nativehook_impl_android_openat(int dirFd, const char *pathName, int flag, int mode);
extern int __nativehook_impl_android_open(const char *pathName, int flag, int mode);


#ifdef __cplusplus
}
#endif


#endif //IMPL_OPEN_H
