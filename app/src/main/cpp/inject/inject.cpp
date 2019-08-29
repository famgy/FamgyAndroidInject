//
// Created by famgy on 8/20/19.
//

#include "inject.h"
#include "traced.hpp"

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>


int usage( char *argvz ){
    printf( "Usage: %s <pid> <library>\n", argvz );
    return 1;
}

int main(int argc, char **argv)
{
//    if( argc < 3 ){
//        return usage(argv[0]);
//    }
//    else if( geteuid() != 0 ){
//        fprintf( stderr, "This script must be executed as root.\n" );
//        return 1;
//    }

    pid_t pid = atoi(argv[1]);
    //std::string library = argv[2];
    std::string library = "/data/local/tmp/libinlinehook.so";

    if( pid == 0 ){
        fprintf( stderr, "Invaid PID %s\n", argv[1] );
        return 1;
    }

    sleep(20000);

    printf( "@ Ptracing from %d attach into process %d.\n\n", getpid(), pid);
    Traced *proc = new Traced(pid);

    printf( "@ Calling dlopen %s in target process ...\n",  library.c_str());
    unsigned long dlret = proc->dlopenInjectSo( library.c_str() );
    printf( "@ dlopen returned 0x%lX\n", dlret );

    proc->detachNow();
    printf( "@ detachNow returned\n");

    //delete proc;

    return 0;
}