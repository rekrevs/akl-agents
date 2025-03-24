cmake_minimum_required(VERSION 3.10)
project(AKL VERSION 0.1.0 LANGUAGES C)

# Add our custom CMake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options
option(AKL_BUILD_TESTS "Build the AKL test suite" ON)
option(AKL_WITH_GMP "Build with GMP support" ON)
option(AKL_USE_SYSTEM_GMP "Use system GMP instead of bundled version" OFF)
option(AKL_ENABLE_ASAN "Enable AddressSanitizer for debugging" OFF)
option(AKL_USE_MODERN_TAGGED_POINTERS "Use modernized tagged pointer implementation" ON)
option(AKL_WITH_FD "Build with finite domain support" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/emulator)

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    
    # Debug build
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
    
    # Release build
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
    
    # AddressSanitizer
    if(AKL_ENABLE_ASAN)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif()
endif()

# Find dependencies
if(AKL_WITH_GMP)
    if(AKL_USE_SYSTEM_GMP)
        # Use system GMP
        find_package(GMP REQUIRED)
        include_directories(${GMP_INCLUDE_DIRS})
        add_definitions(-DHAVE_GMP -DUSE_SYSTEM_GMP)
        message(STATUS "Using system GMP library")
    else()
        # Use bundled GMP (original approach)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gmp)
        add_definitions(-DHAVE_GMP -DUSE_BUNDLED_GMP)
        message(STATUS "Using bundled GMP library")
    endif()
else()
    add_definitions(-DNOBIGNUM)
    message(STATUS "Building without GMP support")
endif()

# Define platform-specific settings
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DLINUX)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_definitions(-DMACOSX)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DWINDOWS)
endif()

# Conditional compilation for finite domain support
if(AKL_WITH_FD)
    add_definitions(-DWITH_FD)
    message(STATUS "Building with finite domain support")
else()
    add_definitions(-DNOFD)
    message(STATUS "Building without finite domain support")
endif()

# Define architecture-specific settings
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Building for 64-bit architecture")
    add_definitions(-DAKL_64BIT)
else()
    message(STATUS "Building for 32-bit architecture")
    add_definitions(-DAKL_32BIT)
endif()

# Configure tagged pointer implementation
if(AKL_USE_MODERN_TAGGED_POINTERS)
    message(STATUS "Using modernized tagged pointer implementation")
    add_definitions(-DUSE_MODERN_TAGGED_POINTERS)
else()
    message(STATUS "Using original tagged pointer implementation")
endif()

# Add architecture detection
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(-DSIXTY_FOUR_BIT)
else()
    add_definitions(-DTHIRTY_TWO_BIT)
endif()

# Add subdirectories
add_subdirectory(emulator)

# Testing
if(AKL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
install(TARGETS DESTINATION bin)
install(FILES README.md LICENSE DESTINATION share/akl)
