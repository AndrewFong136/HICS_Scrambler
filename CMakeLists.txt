cmake_minimum_required(VERSION 3.15)
project(Scrambler)

# Enable C++17 and disable standard library extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_GENERATOR_PLATFORM x64)


# For MinGW (if using GCC on Windows)
if(MINGW)
    add_compile_options(-Wa,-mbig-obj)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

# For MSVC
if(MSVC)
    # Static linking of runtime libraries
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
    )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()

# Add OpenXLSX as a subdirectory
add_subdirectory(OpenXLSX)


# Create executable
add_executable(Scrambler Scrambler.cpp sqlite3.c)

# Link against OpenXLSX and Windows libraries
target_link_libraries(Scrambler PRIVATE OpenXLSX::OpenXLSX)
