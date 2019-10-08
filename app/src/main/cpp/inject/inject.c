//
// Created by famgy on 8/20/19.
//

#include "android_ptrace.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <stdio.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <malloc.h>
#include <stdlib.h>

#define FUNCTION_NAME_ADDR_OFFSET 0X100

const char* libc_path = "/system/lib/libc.so";
const char* linker_path = "/system/bin/linker";

int inject_remote_process(pid_t target_pid, const char* library_path) {
    uint8_t *calloc_mem = 0;
    long parameters[10];

    printf("...ptrace_attach\n");
    if (0 != ptrace_attach(target_pid)) {
        return -1;
    }

    printf("...inject %s\n", library_path);

    struct pt_regs regs;
    if (0 != ptrace_getregs(target_pid, &regs)) {
        return -1;
    }

    struct pt_regs original_regs;
    memcpy(&original_regs, &regs, sizeof(regs));

    //Call calloc
    void *calloc_addr = get_remote_addr(target_pid, libc_path, (void *)calloc);
    printf("[+] Remote calloc_addr address: %x\n", calloc_addr);

    parameters[0] = strlen(library_path);
    parameters[1] = 1;
    if (ptrace_call_wrapper(target_pid, "calloc", calloc_addr, parameters, 2, &regs)) {
        return -1;
    }

    calloc_mem = regs.ARM_r0;
    ptrace_writedata(target_pid, calloc_mem, library_path, strlen(library_path) + 1);

    //Call dlopen
    void *dlopen_addr = get_remote_addr(target_pid, linker_path, (void*)dlopen);
    printf("[+] Remote dlopen address: %x\n", dlopen_addr);

    parameters[0] = calloc_mem;
    parameters[1] = 0;
    if (ptrace_call_wrapper(target_pid, "dlopen", dlopen_addr, parameters, 2, &regs) == -1) {
        return -1;
    }

    ptrace_setregs(target_pid, &original_regs);
    ptrace_detach(target_pid);

    return 0;
}

int main(int argc, char **argv)
{
    if(argc < 3){
        printf( "Usage: %s <pid> <library>\n", argv[0]);
        return 1;
    }
    else if(geteuid() != 0){
        fprintf( stderr, "This script must be executed as root.\n" );
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    char *library = argv[2];
    if(pid == 0){
        fprintf( stderr, "Invaid PID %s\n", argv[1] );
        return 1;
    }

    printf( "@ Ptracing from %d attach into process %d.\n\n", getpid(), pid);
    if (0 != inject_remote_process(pid, library)) {
        printf("Inject failed, exit\n");
        return -1;
    }

    printf("Inject successful, exit\n");
    return 0;
}