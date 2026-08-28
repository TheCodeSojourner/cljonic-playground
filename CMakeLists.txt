cmake_minimum_required(VERSION 3.28)

project(cljonic LANGUAGES CXX)

# Architecture language baseline traceability: C++23.

include(CTest)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(CLJONIC_VECTOR_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/cljonic-vector.hpp" CACHE FILEPATH "Path to the production vector header used by integration tests.")

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)

    file(GLOB CLJONIC_SPEC_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/cljonic-*-spec-tests.cpp"
    )
    add_library(cljonic_modular_header_smoke OBJECT tests/cljonic-modular-header-smoke.cpp)
    target_compile_features(cljonic_modular_header_smoke PRIVATE cxx_std_23)
    target_include_directories(cljonic_modular_header_smoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    add_custom_target(cljonic_header
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/amalgamate.py
                --source-dir ${CMAKE_CURRENT_SOURCE_DIR}/src
                --root cljonic-core.hpp
                --output ${CMAKE_CURRENT_SOURCE_DIR}/cljonic.hpp
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/cljonic.hpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
    )

    function(configure_cljonic_spec_tests target_name use_single_header)
        add_executable(${target_name} ${CLJONIC_SPEC_TEST_SOURCES})
        target_compile_features(${target_name} PRIVATE cxx_std_23)
        target_link_libraries(${target_name} PRIVATE Catch2::Catch2WithMain)
        target_include_directories(${target_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        if(use_single_header)
            target_compile_definitions(${target_name} PRIVATE CLJONIC_USE_SINGLE_HEADER=1)
            target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
            add_dependencies(${target_name} cljonic_header)
        endif()
    endfunction()

    configure_cljonic_spec_tests(cljonic_tests FALSE)
    configure_cljonic_spec_tests(cljonic_single_header_tests TRUE)
    add_dependencies(cljonic_tests cljonic_modular_header_smoke)

    option(CLJONIC_COVERAGE "Enable coverage instrumentation (host profiles only)" OFF)
    if(CLJONIC_COVERAGE)
        target_compile_options(cljonic_tests PRIVATE --coverage)
        target_link_options(cljonic_tests PRIVATE --coverage)
        target_compile_options(cljonic_single_header_tests PRIVATE --coverage)
        target_link_options(cljonic_single_header_tests PRIVATE --coverage)
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
        target_compile_options(cljonic_single_header_tests PRIVATE ${_cljonic_sanitizer_flags})
        target_link_options(cljonic_single_header_tests PRIVATE ${_cljonic_sanitizer_flags})
    endif()

    if(EXISTS "${CLJONIC_VECTOR_HEADER}")
        target_compile_definitions(cljonic_tests PRIVATE CLJONIC_HAVE_VECTOR_IMPLEMENTATION=1)
        target_compile_definitions(cljonic_single_header_tests PRIVATE CLJONIC_HAVE_VECTOR_IMPLEMENTATION=1)
    else()
        message(FATAL_ERROR "Vector integration tests require the production header at ${CLJONIC_VECTOR_HEADER}")
    endif()

    include(Catch)
    catch_discover_tests(cljonic_tests)
    catch_discover_tests(cljonic_single_header_tests)

    file(GLOB CLJONIC_NO_HEAP_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/no_heap/*.cpp"
    )

    function(configure_cljonic_no_heap_probe target_name use_single_header)
        add_executable(${target_name} EXCLUDE_FROM_ALL ${CLJONIC_NO_HEAP_SOURCES})
        target_compile_features(${target_name} PRIVATE cxx_std_23)
        target_include_directories(${target_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/no_heap
        )
        if(use_single_header)
            target_compile_definitions(${target_name} PRIVATE CLJONIC_USE_SINGLE_HEADER=1)
            target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
            add_dependencies(${target_name} cljonic_header)
        endif()
    endfunction()

    configure_cljonic_no_heap_probe(cljonic_no_heap_probe FALSE)
    configure_cljonic_no_heap_probe(cljonic_single_header_no_heap_probe TRUE)
endif()