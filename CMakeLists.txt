cmake_minimum_required(VERSION 3.28)

project(cljonic LANGUAGES CXX)

include(CTest)

set(CLJONIC_VECTOR_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/cljonic/vector.hpp" CACHE FILEPATH "Path to the production vector header used by integration tests.")

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