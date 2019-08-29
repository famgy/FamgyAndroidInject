add_executable( # Sets the name of the library.
        inject

        # Provides a relative path to your source file(s).
        inject.cpp)

target_link_libraries( # Specifies the target library.
        inject

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

add_library( # Sets the name of the library.
        inject_jni

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        inject_jni.cpp)

target_link_libraries( # Specifies the target library.
        inject_jni

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})