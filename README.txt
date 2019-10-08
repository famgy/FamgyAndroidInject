可验证内容：
    1. Android进程Ptrace注入测试验证
    2. libc基础库函数hook测试验证 

测试步骤：
    1. 准备一台已root的手机，安装FamgyAndroid.apk到手机后运行

    2. 将测试文件拷贝到手机中，并增加执行权限：
        adb push inject /data/local/tmp/
        adb push libinlinehook.so /data/local/tmp/
        adb shell
        su
        cd /data/local/tmp
        cp libinlinehook.so /system/lib/
        chmod +x inject

    3. 找到FamgyAndroid程序运行的进程号pid
        top -n 1 | grep famgy

    4. 运行注入程序inject，attch到FamgyAndroid进程中，并加载libinlinehook.so到FamgyAndroid进程中。
        ./inject 6143 /system/lib/libinlinehook.so

    5. 打开手机日志，观察是否已经注入，并hook成功openat和close函数
        logcat
        点击手机程序中的按钮 Pdf，观察手机实时日志，会找到类似这样的日志信息，说明验证成功：
            ===inlinehook===: __nativehook_impl_android_openat /data/user/0/com.famgy.famgyandroid/cache/pdf_test.pdf-pdfview.pdf, fd = 49
            ===inlinehook===: __nativehook_impl_android_close, fd = 49
            




Reference:

pt_regs结构的定义：
    struct pt_regs{
        long uregs[18];
    };
    #define ARM_cpsr uregs[16]    存储状态寄存器的值
    #define ARM_pc uregs[15]    存储当前的执行地址
    #define ARM_lr uregs[14]    存储返回地址
    #define ARM_sp uregs[13]    存储当前的栈顶地址
    #define ARM_ip uregs[12]
    #define ARM_fp uregs[11]
    #define ARM_10 uregs[10]
    #define ARM_9 uregs[9]
    #define ARM_8 uregs[8]
    #define ARM_7 uregs[7]
    #define ARM_6 uregs[6]
    #define ARM_5 uregs[5]
    #define ARM_4 uregs[4]
    #define ARM_3 uregs[3]
    #define ARM_2 uregs[2]
    #define ARM_1 uregs[1]
    #define ARM_0 uregs[0]        存储R0寄存器的值，函数调用后的返回值会存储在R0寄存器中