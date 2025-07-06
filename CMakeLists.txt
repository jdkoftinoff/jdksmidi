cmake_minimum_required(VERSION 3.16)

# Project definition
project(jdksmidi
    VERSION 2.2.0
    DESCRIPTION "C++ MIDI Library"
    LANGUAGES CXX
)

set(CMAKE_WARN_DEPRECATED
    OFF
    CACHE BOOL "" FORCE)
set(DOCTEST_NO_INSTALL
    ON
    CACHE BOOL "" FORCE)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project information
set(JDKSMIDI_VERSION_MAJOR 2)
set(JDKSMIDI_VERSION_MINOR 2)
set(JDKSMIDI_VERSION_PATCH 0)
set(JDKSMIDI_VERSION "${JDKSMIDI_VERSION_MAJOR}.${JDKSMIDI_VERSION_MINOR}.${JDKSMIDI_VERSION_PATCH}")

# Options
option(JDKSMIDI_BUILD_EXAMPLES "Build example programs" ON)
option(JDKSMIDI_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(JDKSMIDI_BUILD_TESTS "Build unit tests" ON)

# Analysis and debugging options
option(JDKSMIDI_ENABLE_STATIC_ANALYSIS "Enable clang static analysis" OFF)
option(JDKSMIDI_ENABLE_IWYU "Enable include-what-you-use" OFF)
option(JDKSMIDI_ENABLE_COVERAGE "Enable test coverage reporting" OFF)
option(JDKSMIDI_ENABLE_SANITIZERS "Enable clang sanitizers" OFF)
option(JDKSMIDI_SANITIZER_ADDRESS "Enable AddressSanitizer" OFF)
option(JDKSMIDI_SANITIZER_MEMORY "Enable MemorySanitizer" OFF)
option(JDKSMIDI_SANITIZER_THREAD "Enable ThreadSanitizer" OFF)
option(JDKSMIDI_SANITIZER_UNDEFINED "Enable UndefinedBehaviorSanitizer" OFF)

# Set library type based on option
if(JDKSMIDI_BUILD_SHARED_LIBS)
    set(JDKSMIDI_LIBRARY_TYPE SHARED)
else()
    set(JDKSMIDI_LIBRARY_TYPE STATIC)
endif()

# Configure static analysis
if(JDKSMIDI_ENABLE_STATIC_ANALYSIS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_CLANG_TIDY clang-tidy)
        set(CMAKE_C_CLANG_TIDY clang-tidy)
        
        # Set up clang static analyzer
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_CURRENT_SOURCE_DIR}/scripts/clang-analyzer-wrapper.sh")
        
        # Enable additional static analysis flags
        add_compile_options(
            -Weverything
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-padded
            -Wno-weak-vtables
            -Wno-documentation
            -Wno-documentation-unknown-command
        )
    else()
        message(WARNING "Static analysis requested but Clang not detected. Static analysis disabled.")
        set(JDKSMIDI_ENABLE_STATIC_ANALYSIS OFF)
    endif()
endif()

# Configure include-what-you-use
if(JDKSMIDI_ENABLE_IWYU)
    find_program(IWYU_PROGRAM NAMES include-what-you-use iwyu)
    if(IWYU_PROGRAM)
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE 
            "${IWYU_PROGRAM};-Xiwyu;--mapping_file=${CMAKE_CURRENT_SOURCE_DIR}/iwyu.imp;-Xiwyu;--verbose=3")
        message(STATUS "include-what-you-use enabled: ${IWYU_PROGRAM}")
    else()
        message(WARNING "include-what-you-use requested but not found. IWYU analysis disabled.")
        set(JDKSMIDI_ENABLE_IWYU OFF)
    endif()
endif()

# Configure sanitizers
set(JDKSMIDI_SANITIZER_FLAGS "")
if(JDKSMIDI_ENABLE_SANITIZERS OR JDKSMIDI_SANITIZER_ADDRESS OR JDKSMIDI_SANITIZER_MEMORY OR 
   JDKSMIDI_SANITIZER_THREAD OR JDKSMIDI_SANITIZER_UNDEFINED)
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        if(JDKSMIDI_SANITIZER_ADDRESS OR JDKSMIDI_ENABLE_SANITIZERS)
            list(APPEND JDKSMIDI_SANITIZER_FLAGS "-fsanitize=address")
            list(APPEND JDKSMIDI_SANITIZER_FLAGS "-fno-omit-frame-pointer")
            message(STATUS "AddressSanitizer enabled")
        endif()
        
        if(JDKSMIDI_SANITIZER_MEMORY)
            if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                list(APPEND JDKSMIDI_SANITIZER_FLAGS "-fsanitize=memory")
                list(APPEND JDKSMIDI_SANITIZER_FLAGS "-fno-omit-frame-pointer")
                message(STATUS "MemorySanitizer enabled")
            else()
                message(WARNING "MemorySanitizer requires Clang. Skipping.")
            endif()
        endif()
        
        if(JDKSMIDI_SANITIZER_THREAD)
            list(APPEND JDKSMIDI_SANITIZER_FLAGS "-fsanitize=thread")
            message(STATUS "ThreadSanitizer enabled")
        endif()
        
        if(JDKSMIDI_SANITIZER_UNDEFINED OR JDKSMIDI_ENABLE_SANITIZERS)
            list(APPEND JDKSMIDI_SANITIZER_FLAGS "-fsanitize=undefined")
            list(APPEND JDKSMIDI_SANITIZER_FLAGS "-fno-sanitize-recover=undefined")
            message(STATUS "UndefinedBehaviorSanitizer enabled")
        endif()
        
        # Check for conflicting sanitizers
        if(JDKSMIDI_SANITIZER_ADDRESS AND JDKSMIDI_SANITIZER_MEMORY)
            message(FATAL_ERROR "AddressSanitizer and MemorySanitizer cannot be used together")
        endif()
        
        if(JDKSMIDI_SANITIZER_ADDRESS AND JDKSMIDI_SANITIZER_THREAD)
            message(FATAL_ERROR "AddressSanitizer and ThreadSanitizer cannot be used together")
        endif()
        
        if(JDKSMIDI_SANITIZER_MEMORY AND JDKSMIDI_SANITIZER_THREAD)
            message(FATAL_ERROR "MemorySanitizer and ThreadSanitizer cannot be used together")
        endif()
        
    else()
        message(WARNING "Sanitizers requested but Clang/GCC not detected. Sanitizers disabled.")
    endif()
endif()

# Configure test coverage
set(JDKSMIDI_COVERAGE_FLAGS "")
if(JDKSMIDI_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        list(APPEND JDKSMIDI_COVERAGE_FLAGS "--coverage")
        list(APPEND JDKSMIDI_COVERAGE_FLAGS "-fprofile-arcs")
        list(APPEND JDKSMIDI_COVERAGE_FLAGS "-ftest-coverage")
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            list(APPEND JDKSMIDI_COVERAGE_FLAGS "-fprofile-instr-generate")
            list(APPEND JDKSMIDI_COVERAGE_FLAGS "-fcoverage-mapping")
        endif()
        message(STATUS "Test coverage reporting enabled")
        
        # Find gcov or llvm-cov for coverage reporting
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            find_program(GCOV_PROGRAM gcov)
            if(GCOV_PROGRAM)
                message(STATUS "Found gcov: ${GCOV_PROGRAM}")
            else()
                message(WARNING "gcov not found - coverage reports may not be available")
            endif()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            find_program(LLVM_COV_PROGRAM llvm-cov)
            if(LLVM_COV_PROGRAM)
                message(STATUS "Found llvm-cov: ${LLVM_COV_PROGRAM}")
            else()
                message(WARNING "llvm-cov not found - coverage reports may not be available")
            endif()
        endif()
        
        # Find lcov for HTML coverage reports (optional)
        find_program(LCOV_PROGRAM lcov)
        find_program(GENHTML_PROGRAM genhtml)
        if(LCOV_PROGRAM AND GENHTML_PROGRAM)
            message(STATUS "Found lcov tools for HTML coverage reports")
            message(STATUS "  lcov: ${LCOV_PROGRAM}")
            message(STATUS "  genhtml: ${GENHTML_PROGRAM}")
        else()
            message(STATUS "lcov tools not found - HTML coverage reports not available")
        endif()
    else()
        message(WARNING "Coverage requested but only GCC/Clang are supported. Coverage disabled.")
        set(JDKSMIDI_ENABLE_COVERAGE OFF)
    endif()
endif()

# Include directories
include_directories(include)

# Source files for the main library
set(JDKSMIDI_SOURCES
    src/jdksmidi_advancedsequencer.cpp
    src/jdksmidi_driver.cpp
    src/jdksmidi_driverdump.cpp
    src/jdksmidi_edittrack.cpp
    src/jdksmidi_file.cpp
    src/jdksmidi_fileread.cpp
    src/jdksmidi_filereadmultitrack.cpp
    src/jdksmidi_fileshow.cpp
    src/jdksmidi_filewrite.cpp
    src/jdksmidi_filewritemultitrack.cpp
    src/jdksmidi_keysig.cpp
    src/jdksmidi_manager.cpp
    src/jdksmidi_matrix.cpp
    src/jdksmidi_midi.cpp
    src/jdksmidi_msg.cpp
    src/jdksmidi_multitrack.cpp
    src/jdksmidi_parser.cpp
    src/jdksmidi_process.cpp
    src/jdksmidi_queue.cpp
    src/jdksmidi_sequencer.cpp
    src/jdksmidi_showcontrol.cpp
    src/jdksmidi_showcontrolhandler.cpp
    src/jdksmidi_smpte.cpp
    src/jdksmidi_sysex.cpp
    src/jdksmidi_tempo.cpp
    src/jdksmidi_tick.cpp
    src/jdksmidi_track.cpp
)

# Platform-specific sources
if(WIN32)
    list(APPEND JDKSMIDI_SOURCES src/win32/jdksmidi_driverwin32.cpp)
endif()

# Header files for installation
set(JDKSMIDI_HEADERS
    include/jdksmidi/advancedsequencer.h
    include/jdksmidi/driver.h
    include/jdksmidi/driverdump.h
    include/jdksmidi/driverwin32.h
    include/jdksmidi/edittrack.h
    include/jdksmidi/file.h
    include/jdksmidi/fileread.h
    include/jdksmidi/filereadmultitrack.h
    include/jdksmidi/fileshow.h
    include/jdksmidi/filewrite.h
    include/jdksmidi/filewritemultitrack.h
    include/jdksmidi/keysig.h
    include/jdksmidi/manager.h
    include/jdksmidi/matrix.h
    include/jdksmidi/midi.h
    include/jdksmidi/msg.h
    include/jdksmidi/multitrack.h
    include/jdksmidi/parser.h
    include/jdksmidi/process.h
    include/jdksmidi/queue.h
    include/jdksmidi/sequencer.h
    include/jdksmidi/showcontrol.h
    include/jdksmidi/showcontrolhandler.h
    include/jdksmidi/smpte.h
    include/jdksmidi/song.h
    include/jdksmidi/sysex.h
    include/jdksmidi/tempo.h
    include/jdksmidi/tick.h
    include/jdksmidi/track.h
    include/jdksmidi/world.h
)

# Create the main library
add_library(jdksmidi ${JDKSMIDI_LIBRARY_TYPE} ${JDKSMIDI_SOURCES})

# Set library properties
set_target_properties(jdksmidi PROPERTIES
    VERSION ${JDKSMIDI_VERSION}
    SOVERSION ${JDKSMIDI_VERSION_MAJOR}
    PUBLIC_HEADER "${JDKSMIDI_HEADERS}"
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(jdksmidi PRIVATE winmm)
    if(MINGW)
        target_compile_options(jdksmidi PRIVATE -mthreads)
        target_link_options(jdksmidi PRIVATE -mthreads)
    endif()
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(jdksmidi PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -Wunused-result
    )
endif()

if(MSVC)
    target_compile_options(jdksmidi PRIVATE
        /W4
        /wd4100  # unreferenced formal parameter
    )
endif()

# Apply sanitizer flags
if(JDKSMIDI_SANITIZER_FLAGS)
    target_compile_options(jdksmidi PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
    target_link_options(jdksmidi PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
endif()

# Apply coverage flags
if(JDKSMIDI_COVERAGE_FLAGS)
    target_compile_options(jdksmidi PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
    target_link_options(jdksmidi PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
endif()

# Include directories for the library
target_include_directories(jdksmidi
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Build examples if requested
if(JDKSMIDI_BUILD_EXAMPLES)
    # List of example programs
    set(JDKSMIDI_EXAMPLES
        jdksmidi_rewrite_midifile
        jdksmidi_example_drv
        jdksmidi_example_multitrack
        jdksmidi_example_multitrack1
        jdksmidi_example_parse
        jdksmidi_example_sequencer
        jdksmidi_example_show
    )

    # Create example executables
    foreach(example ${JDKSMIDI_EXAMPLES})
        add_executable(${example} examples/${example}.cpp)
        target_link_libraries(${example} PRIVATE jdksmidi)
        
        # Apply sanitizer flags to examples
        if(JDKSMIDI_SANITIZER_FLAGS)
            target_compile_options(${example} PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
            target_link_options(${example} PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
        endif()
        
        # Apply coverage flags to examples
        if(JDKSMIDI_COVERAGE_FLAGS)
            target_compile_options(${example} PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
            target_link_options(${example} PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
        endif()
        
        # Platform-specific linking for examples
        if(WIN32)
            target_link_libraries(${example} PRIVATE winmm)
        endif()
    endforeach()

    # Windows-specific example
    if(WIN32)
        add_executable(jdksmidi_test_drvwin32 examples/win32/jdksmidi_test_drvwin32.cpp)
        target_link_libraries(jdksmidi_test_drvwin32 PRIVATE jdksmidi winmm)
        
        # Apply sanitizer flags to Windows example
        if(JDKSMIDI_SANITIZER_FLAGS)
            target_compile_options(jdksmidi_test_drvwin32 PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
            target_link_options(jdksmidi_test_drvwin32 PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
        endif()
        
        # Apply coverage flags to Windows example
        if(JDKSMIDI_COVERAGE_FLAGS)
            target_compile_options(jdksmidi_test_drvwin32 PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
            target_link_options(jdksmidi_test_drvwin32 PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
        endif()
    endif()
endif()

# Installation
include(GNUInstallDirs)

# Install the library
install(TARGETS jdksmidi
    EXPORT jdksmidicTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/jdksmidi
)

# Install examples if built
if(JDKSMIDI_BUILD_EXAMPLES)
    install(TARGETS ${JDKSMIDI_EXAMPLES}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    if(WIN32)
        install(TARGETS jdksmidi_test_drvwin32
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endif()

# Install additional files
install(FILES
    COPYING
    README
    CHANGELOG.txt
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# Create and install CMake package config files
include(CMakePackageConfigHelpers)

# Generate the config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/jdksmidicConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/jdksmidicConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jdksmidc
)

# Generate the version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/jdksmidicConfigVersion.cmake"
    VERSION ${JDKSMIDI_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install CMake config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/jdksmidicConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/jdksmidicConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jdksmidc
)

# Install the export targets
install(EXPORT jdksmidicTargets
    FILE jdksmidicTargets.cmake
    NAMESPACE jdksmidc::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jdksmidc
)

# Add doctest for unit testing
if(JDKSMIDI_BUILD_TESTS)
    # Add doctest as a subdirectory
    add_subdirectory(third-party/doctest)
    
    # Create tests directory
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    
    # Enable testing
    enable_testing()
    
    # Find all test files
    file(GLOB TEST_SOURCES "tests/test_*.cpp")
    
    # Create test executable if test files exist
    if(TEST_SOURCES)
        add_executable(jdksmidi_tests ${TEST_SOURCES})
        target_link_libraries(jdksmidi_tests PRIVATE jdksmidi doctest::doctest)
        
        # Apply sanitizer flags to tests
        if(JDKSMIDI_SANITIZER_FLAGS)
            target_compile_options(jdksmidi_tests PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
            target_link_options(jdksmidi_tests PRIVATE ${JDKSMIDI_SANITIZER_FLAGS})
        endif()
        
        # Apply coverage flags to tests
        if(JDKSMIDI_COVERAGE_FLAGS)
            target_compile_options(jdksmidi_tests PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
            target_link_options(jdksmidi_tests PRIVATE ${JDKSMIDI_COVERAGE_FLAGS})
        endif()
        
        # Copy test data files to build directory for integration tests
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/songs")
            file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/songs" 
                 DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
            message(STATUS "  Test MIDI files copied to build directory")
        endif()
        
        # Add test to CTest
        add_test(NAME jdksmidi_unit_tests COMMAND jdksmidi_tests)
        
        # Add coverage report targets if coverage is enabled
        if(JDKSMIDI_ENABLE_COVERAGE)
            # Create coverage directory
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/coverage")
            
            # Clean coverage data target
            add_custom_target(coverage-clean
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/coverage"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/coverage"
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Cleaning coverage data"
            )
            
            if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND LCOV_PROGRAM AND GENHTML_PROGRAM)
                # Generate HTML coverage report using gcov + lcov
                add_custom_target(coverage-html
                    COMMAND ${LCOV_PROGRAM} --capture --directory . --output-file coverage/coverage.info --ignore-errors inconsistent
                    COMMAND ${LCOV_PROGRAM} --extract coverage/coverage.info "*/src/*" "*/include/*" --output-file coverage/coverage_filtered.info --ignore-errors inconsistent
                    COMMAND ${GENHTML_PROGRAM} coverage/coverage_filtered.info --output-directory coverage/html --ignore-errors inconsistent
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS jdksmidi_tests
                    COMMENT "Generating HTML coverage report with lcov"
                )
                
                # Generate coverage summary
                add_custom_target(coverage-summary
                    COMMAND ${LCOV_PROGRAM} --capture --directory . --output-file coverage/coverage.info --ignore-errors inconsistent
                    COMMAND ${LCOV_PROGRAM} --extract coverage/coverage.info "*/src/*" "*/include/*" --output-file coverage/coverage_filtered.info --ignore-errors inconsistent
                    COMMAND ${LCOV_PROGRAM} --summary coverage/coverage_filtered.info --ignore-errors inconsistent
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS jdksmidi_tests
                    COMMENT "Generating coverage summary with lcov"
                )
                
                # Main coverage target
                add_custom_target(coverage
                    DEPENDS coverage-html
                    COMMENT "Generating coverage reports"
                )
                
            elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND LLVM_COV_PROGRAM)
                # Generate coverage report using llvm-cov
                add_custom_target(coverage-html
                    COMMAND ${LLVM_COV_PROGRAM} show $<TARGET_FILE:jdksmidi_tests> -format=html -output-dir=coverage/html -instr-profile=default.profraw
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS jdksmidi_tests
                    COMMENT "Generating HTML coverage report with llvm-cov"
                )
                
                # Generate coverage summary
                add_custom_target(coverage-summary
                    COMMAND ${LLVM_COV_PROGRAM} report $<TARGET_FILE:jdksmidi_tests> -instr-profile=default.profraw
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS jdksmidi_tests
                    COMMENT "Generating coverage summary with llvm-cov"
                )
                
                # Main coverage target
                add_custom_target(coverage
                    DEPENDS coverage-html
                    COMMENT "Generating coverage reports"
                )
            else()
                # Basic coverage target without HTML generation
                add_custom_target(coverage
                    COMMAND ${CMAKE_COMMAND} -E echo "Coverage data collected. Use appropriate tools to generate reports."
                    DEPENDS jdksmidi_tests
                    COMMENT "Coverage instrumentation enabled - run tests to collect data"
                )
            endif()
            
            # Add coverage to dependencies of coverage-clean (only if targets exist)
            if(TARGET coverage-html)
                add_dependencies(coverage-html coverage-clean)
            endif()
            if(TARGET coverage-summary)
                add_dependencies(coverage-summary coverage-clean)
            endif()
        endif()
    endif()
endif()

# Display configuration summary
message(STATUS "")
message(STATUS "jdksmidi ${JDKSMIDI_VERSION} Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Library type: ${JDKSMIDI_LIBRARY_TYPE}")
message(STATUS "  Build examples: ${JDKSMIDI_BUILD_EXAMPLES}")
message(STATUS "  Build tests: ${JDKSMIDI_BUILD_TESTS}")
message(STATUS "  Static analysis: ${JDKSMIDI_ENABLE_STATIC_ANALYSIS}")
message(STATUS "  Include-what-you-use: ${JDKSMIDI_ENABLE_IWYU}")
message(STATUS "  Coverage reporting: ${JDKSMIDI_ENABLE_COVERAGE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
if(WIN32)
    message(STATUS "  Platform: Windows (winmm library will be linked)")
else()
    message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
endif()
message(STATUS "")
