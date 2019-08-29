//
// Created by famgy on 18-12-3.
//

#ifndef IMPL_CLOSE_H
#define IMPL_CLOSE_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


extern int (*old_close)(int fd);

extern int __nativehook_impl_android_close(int fd);

#ifdef __cplusplus
}
#endif



#endif //IMPL_CLOSE_H
