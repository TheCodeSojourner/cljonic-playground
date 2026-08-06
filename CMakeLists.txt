cmake_minimum_required(VERSION 3.28)

project(cljonic LANGUAGES CXX)

# Architecture language baseline traceability: C++23.

include(CTest)

set(CLJONIC_VECTOR_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/vector.hpp" CACHE FILEPATH "Path to the production vector header used by integration tests.")

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(cljonic_tests
        tests/vector_spec_tests.cpp
    )
    target_compile_features(cljonic_tests PRIVATE cxx_std_23)
    target_link_libraries(cljonic_tests PRIVATE Catch2::Catch2WithMain)

    option(CLJONIC_COVERAGE "Enable coverage instrumentation (host profiles only)" OFF)
    if(CLJONIC_COVERAGE)
        target_compile_options(cljonic_tests PRIVATE --coverage)
        target_link_options(cljonic_tests PRIVATE --coverage)
    endif()

    set(CLJONIC_SANITIZERS "off" CACHE STRING "Sanitizer profile: off or asan-ubsan")
    set_property(CACHE CLJONIC_SANITIZERS PROPERTY STRINGS off asan-ubsan)
    if(NOT CLJONIC_SANITIZERS STREQUAL "off")
        if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            message(FATAL_ERROR "CLJONIC_SANITIZERS requires a Clang or GCC toolchain")
        endif()

        if(CLJONIC_SANITIZERS STREQUAL "asan-ubsan")
            set(_cljonic_sanitizer_flags -fsanitize=address,undefined -fno-omit-frame-pointer)
        else()
            message(FATAL_ERROR "Unsupported CLJONIC_SANITIZERS value: ${CLJONIC_SANITIZERS}")
        endif()

        target_compile_options(cljonic_tests PRIVATE ${_cljonic_sanitizer_flags})
        target_link_options(cljonic_tests PRIVATE ${_cljonic_sanitizer_flags})
    endif()
    target_include_directories(cljonic_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)

    if(EXISTS "${CLJONIC_VECTOR_HEADER}")
        target_compile_definitions(cljonic_tests PRIVATE CLJONIC_HAVE_VECTOR_IMPLEMENTATION=1)
        target_include_directories(cljonic_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        message(STATUS "Vector integration tests enabled with header: ${CLJONIC_VECTOR_HEADER}")
    else()
        message(STATUS "Vector integration tests skipped: production header not found at ${CLJONIC_VECTOR_HEADER}")
    endif()

    include(Catch)
    catch_discover_tests(cljonic_tests)
endif()