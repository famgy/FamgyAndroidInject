add_executable( # Sets the name of the library.
        inject

        # Provides a relative path to your source file(s).
        android_ptrace.c
        inject.c)

target_link_libraries( # Specifies the target library.
        inject

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})