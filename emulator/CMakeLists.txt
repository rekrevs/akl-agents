# Emulator CMakeLists.txt

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/compat
)

# Compatibility layer sources
set(COMPAT_SOURCES
    compat/tagged_pointers_compat.c
)

# Add compatibility subdirectory for 64-bit support
add_subdirectory(compat)

# Source files for the emulator core
set(EMULATOR_CORE_SOURCES
    abstraction.c
    aggregate.c
    array.c
    bam.c
    bignum.c
    builtin.c
    candidate.c
    code.c
    compare.c
    config.c
    copy.c
    debug.c
    display.c
    engine.c
    error.c
    examine.c
    exstate.c
    fd.c
    fd_akl.c
    foreign.c
    functor.c
    gc.c
    ghash.c
    initial.c
    inout.c
    intrpt.c
    is.c
    load.c
    main.c
    names.c
    port.c
    predicate.c
    ptrhash.c
    rational.c
    reflection.c
    statistics.c
    storage.c
    time.c
    trace.c
    unify.c
)

# Parser generated files
set(PARSER_SOURCES
    parser.tab.c
    parser.yy.c
)

# Define the emulator library (without parser)
add_library(akl_core STATIC ${EMULATOR_CORE_SOURCES} ${COMPAT_SOURCES})

# Define targets for parser generation
find_package(BISON)
find_package(FLEX)

if(BISON_FOUND AND FLEX_FOUND)
    # Custom commands to generate parser files
    add_custom_command(
        OUTPUT parser.y
        COMMAND m4 ${CMAKE_CURRENT_SOURCE_DIR}/parser.y.m4 > ${CMAKE_CURRENT_BINARY_DIR}/parser.y
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y.m4
    )
    
    add_custom_command(
        OUTPUT parser.tab.c parser.tab.h
        COMMAND ${BISON_EXECUTABLE} -d ${CMAKE_CURRENT_BINARY_DIR}/parser.y
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parser.y
    )
    
    add_custom_command(
        OUTPUT parser.yy.c
        COMMAND ${FLEX_EXECUTABLE} -8 ${CMAKE_CURRENT_SOURCE_DIR}/parser.l
        COMMAND mv lex.yy.c parser.yy.c
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.l ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
    )
    
    # Parser library
    add_library(akl_parser STATIC ${PARSER_SOURCES})
    target_include_directories(akl_parser PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    
    # Main executable
    add_executable(agents main.c foreign_stub.c)
    target_link_libraries(agents akl_core akl_parser)
    
    if(AKL_WITH_GMP)
        target_link_libraries(agents ${GMP_LIBRARIES})
    endif()
    
    # Math library is always needed
    target_link_libraries(agents m)
else()
    message(WARNING "Bison and/or Flex not found. Parser generation disabled.")
endif()

# Component test executables - commented out until test files are created
# These allow testing individual components in isolation

# TODO: Create these test files or remove these targets
# # Tagged pointer test
# add_executable(emulator_test_tagged_pointers tests/test_tagged_pointers.c)
# target_include_directories(emulator_test_tagged_pointers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# 
# # Memory management test
# add_executable(emulator_test_memory tests/test_memory.c)
# target_link_libraries(emulator_test_memory akl_core)
# 
# # Add tests
# add_test(NAME EmulatorTaggedPointers COMMAND emulator_test_tagged_pointers)
# add_test(NAME EmulatorMemoryManagement COMMAND emulator_test_memory)
