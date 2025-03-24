# CMake configuration for compatibility layer

# Create library for 64-bit compatibility
add_library(compat_64bit
    tagged_pointers_64bit.c
    storage_64bit.c
    gc_64bit.c
)

target_include_directories(compat_64bit PUBLIC
    ${CMAKE_SOURCE_DIR}
)

set_target_properties(compat_64bit PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Add compiler flags
target_compile_options(compat_64bit PRIVATE
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O2>
)
