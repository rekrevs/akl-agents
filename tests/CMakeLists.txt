# Tests CMakeLists.txt

# Create test directory structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unit)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/integration)

# Test framework
add_library(test_framework STATIC
    framework/test_framework.c
)
target_include_directories(test_framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/framework
)

# Unit tests for core components
# These will be added as we modernize each component

# Create directories for test source files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/unit)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/integration)

# Add test executables as they are developed
# For now, we'll create placeholder test files that we'll implement later

# Create a test for tagged pointers
add_executable(test_tagged_pointers unit/test_tagged_pointers.c)
target_link_libraries(test_tagged_pointers test_framework)
add_test(NAME TaggedPointers COMMAND test_tagged_pointers)

# Create a test for memory management
add_executable(test_memory unit/test_memory.c)
target_link_libraries(test_memory test_framework)
add_test(NAME MemoryManagement COMMAND test_memory)

# Create a test for garbage collection
add_executable(test_gc unit/test_gc.c)
target_link_libraries(test_gc test_framework)
add_test(NAME GarbageCollection COMMAND test_gc)

# GMP compatibility test (only built when using system GMP)
if(AKL_WITH_GMP AND AKL_USE_SYSTEM_GMP)
    add_executable(test_gmp_compat unit/test_gmp_compat.c)
    target_include_directories(test_gmp_compat PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(test_gmp_compat test_framework ${GMP_LIBRARIES})
    add_test(NAME GmpCompat COMMAND test_gmp_compat)
endif()

# Modern tagged pointers test (only built when using modern tagged pointers)
if(AKL_USE_MODERN_TAGGED_POINTERS)
    add_executable(test_tagged_pointers_modern unit/test_tagged_pointers_modern.c)
    target_include_directories(test_tagged_pointers_modern PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(test_tagged_pointers_modern test_framework)
    add_test(NAME TaggedPointersModern COMMAND test_tagged_pointers_modern)
endif()

# 64-bit tagged pointers test
add_executable(test_tagged_pointers_64bit unit/test_tagged_pointers_64bit.c)
target_include_directories(test_tagged_pointers_64bit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_tagged_pointers_64bit test_framework)
add_test(NAME TaggedPointers64bit COMMAND test_tagged_pointers_64bit)

# Comprehensive 64-bit tagged pointers test
add_executable(test_tagged_pointers_64bit_comprehensive unit/test_tagged_pointers_64bit_comprehensive.c)
target_include_directories(test_tagged_pointers_64bit_comprehensive PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_tagged_pointers_64bit_comprehensive test_framework)
add_test(NAME TaggedPointers64bitComprehensive COMMAND test_tagged_pointers_64bit_comprehensive)

# Memory management with 64-bit tagged pointers test
add_executable(test_memory_management_64bit unit/test_memory_management_64bit.c)
target_include_directories(test_memory_management_64bit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_memory_management_64bit test_framework)
add_test(NAME MemoryManagement64bit COMMAND test_memory_management_64bit)

# Memory-GC integration with 64-bit tagged pointers test
add_executable(test_memory_gc_integration_64bit unit/test_memory_gc_integration_64bit.c)
target_include_directories(test_memory_gc_integration_64bit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_memory_gc_integration_64bit test_framework compat_64bit)
add_test(NAME MemoryGcIntegration64bit COMMAND test_memory_gc_integration_64bit)

# Diagnostic test for 64-bit tagged pointers
add_executable(debug_tagged_pointers_64bit unit/debug_tagged_pointers_64bit.c)
target_include_directories(debug_tagged_pointers_64bit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(debug_tagged_pointers_64bit test_framework)

# Integration tests will be added as components are modernized
