
# Produce executable - hooktest
add_executable( # Sets the name of the library.
        hooktest

        # Provides a relative path to your source file(s).
        inlineHook.c
        relocate.c

        hooktest.c)

target_link_libraries( # Specifies the target library.
        hooktest

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})


# Produce libhook.so
add_library( # Sets the name of the library.
        inlinehook

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        inlineHook.c
        relocate.c

        impl/impl_open.c
        impl/impl_close.c

        main.c)

target_link_libraries( # Specifies the target library.
        inlinehook

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})