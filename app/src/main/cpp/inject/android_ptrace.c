//
// Created by famgy on 9/20/19.
//

#include <sys/ptrace.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "android_ptrace.h"


#if defined(__i386__)
#define pt_regs user_regs_struct
#endif

#define CPSR_T_MASK (1u << 5)

int ptrace_attach(pid_t pid) {
    if (ptrace(PTRACE_ATTACH, pid, NULL, 0) < 0) {
        perror("ptrace_attach");
        return -1;
    }

    return 0;
}

int ptrace_detach(pid_t pid) {
    if (ptrace(PTRACE_DETACH, pid, NULL, 0) < 0) {
        perror("ptrace_detach");
        return -1;
    }

    return 0;
}

int ptrace_getregs(pid_t pid, struct pt_regs *regs) {
    if (ptrace(PTRACE_GETREGS, pid, NULL, regs) < 0) {
        perror("ptrace_getregs: Can not get register values.");
        return -1;
    }

    return 0;
}

void *get_module_base(pid_t pid, const char *module_name) {
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[1024];
    char line[1024];

    if (pid < 0) {
        snprintf(filename, sizeof(filename), "/proc/self/maps");
    } else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }

    fp = fopen(filename, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name)) {
                pch = strtok(line, "-");
                addr = strtoul(pch, NULL, 16);
                if (addr == 0x8000) {
                    addr = 0;
                }

                break;
            }
        }
        fclose(fp);
    }

    return (void *)addr;
}



void *get_remote_addr(pid_t target_pid, const char *module_name, void *local_addr) {
    void *local_handle = get_module_base(-1, module_name);
    void *remote_handle = get_module_base(target_pid, module_name);

    printf("[+]get remote address: local[%x], remote[%x]\n", local_handle, remote_handle);

    void *ret_addr = (void *)((uint32_t)local_addr - (uint32_t)local_handle) + (uint32_t)remote_handle;

    return ret_addr;
}

int ptrace_readdata(pid_t pid, uint8_t *src, uint8_t *buf, size_t size) {
    uint32_t i;
    uint32_t j;
    uint32_t remain;
    uint8_t  *laddr;

    union u{
        long val;
        char chars[sizeof(long)];
    }d;

    j = size / 4;
    remain = size % 4;
    laddr = buf;

    for (i = 0; i < j; i++) {
        d.val = ptrace(PTRACE_PEEKTEXT, pid, src, 0);
        memcpy(laddr, d.chars, 4);
        src += 4;
        laddr += 4;
    }

    if (remain > 0) {
        d.val = ptrace(PTRACE_PEEKTEXT, pid, src, 0);
        memcpy(laddr, d.chars, remain);
    }

    return 0;
}

int ptrace_writedata(pid_t pid, uint8_t *dest, uint8_t *data, size_t size) {
    uint32_t i;
    uint32_t j;
    uint32_t remain;
    uint8_t  *laddr;

    union u{
        long val;
        char u_data[sizeof(long)];
    }d;

    j = size / 4;
    remain = size % 4;
    laddr = data;

    for (i = 0; i < j; i++) {
        memcpy(d.u_data, laddr, 4);
        ptrace(PTRACE_POKETEXT, pid, dest, d.val);

        dest += 4;
        laddr += 4;
    }

    if (remain > 0) {
        d.val = ptrace(PTRACE_PEEKTEXT, pid, dest, 0);
        for (i = 0; i < remain; i++) {
            d.u_data[i] = *laddr++;
        }

        ptrace(PTRACE_POKETEXT, pid, dest, d.val);
    }

    return 0;
}

int ptrace_setregs(pid_t pid, struct pt_regs *regs) {
    if (ptrace(PTRACE_SETREGS, pid, NULL, regs) < 0) {
        perror("ptrace_setregs:Can not set regsiter values.");
        return -1;
    }

    return 0;
}

int ptrace_continue(pid_t pid) {
    if (ptrace(PTRACE_CONT, pid, NULL, 0) < 0) {
        perror("ptrace_cont");
        return -1;
    }

    return 0;
}

int ptrace_call(pid_t pid, uint32_t addr, long *params, uint32_t num_params, struct pt_regs *regs) {
    uint32_t i;

    for (int i = 0; i < num_params && i < 4; ++i) {
        regs->uregs[i] = params[i];
    }

    if (i < num_params) {
        regs->ARM_sp -= (num_params - i) * sizeof(long);
        ptrace_writedata(pid, (void *)regs->ARM_sp, (uint8_t *)&params[i], (num_params - i) * sizeof(long));
    }

    regs->ARM_pc = addr;
    if (regs->ARM_pc & 1) {
        //thumb
        regs->ARM_pc &= (-1u);
        regs->ARM_cpsr |= CPSR_T_MASK;
    } else {
        //arm
        regs->ARM_cpsr &= ~CPSR_T_MASK;
    }

    regs->ARM_lr = 0;

    if (ptrace_setregs(pid, regs) == -1 || ptrace_continue(pid) == -1) {
        printf("ptrace_call error.\n");
        return -1;
    }

    int stat = 0;
    waitpid(pid, &stat, WUNTRACED);

    while(stat != 0xb7f) {
        if (ptrace_continue(pid) == -1) {
            printf("ptrace_continue error.\n");
            return -1;
        }
        waitpid(pid, &stat, WUNTRACED);
    }

    return 0;
};

long ptrace_retval(struct pt_regs* regs){
    return regs->ARM_r0;
}

long ptrace_ip(struct pt_regs* regs){
    return regs->ARM_pc;
}


int ptrace_call_wrapper(pid_t target_pid, const char *func_name, void *func_addr, long *params, int param_num, struct pt_regs *regs) {
    printf("[+]Calling %s in target process.\n", func_name);

    if (ptrace_call(target_pid, (uint32_t)func_addr, params, param_num, regs) == -1) {
        return -1;
    }

    //使用ptrace_getregs函数获取当前的所有寄存器值，方便后面使用ptrace_retval函数获取函数的返回值。
    if (ptrace_getregs(target_pid, regs) == -1) {
        return -1;
    }

    printf("[+] Target process returned from %s, return value = %x, pc = %x \n", func_name, ptrace_retval(regs), ptrace_ip(regs));
    return 0;
}