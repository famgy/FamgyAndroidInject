//
// Created by famgy on 9/20/19.
//

#ifndef ANDROID_PTRACE_H
#define ANDROID_PTRACE_H

#include <sys/types.h>

extern int ptrace_attach(pid_t pid);
extern int ptrace_detach(pid_t pid);
extern int ptrace_getregs(pid_t pid, struct pt_regs *regs);
extern int ptrace_setregs(pid_t pid, struct pt_regs *regs);
extern void *get_remote_addr(pid_t target_pid, const char *module_name, void *local_addr);
extern int ptrace_readdata(pid_t pid, uint8_t *src, uint8_t *buf, size_t size);
extern int ptrace_writedata(pid_t pid, uint8_t *dest, uint8_t *data, size_t size);
extern int ptrace_call_wrapper(pid_t target_pid, const char *func_name, void *func_addr, long *params, int param_num, struct pt_regs *regs);

#endif //ANDROID_PTRACE_H
