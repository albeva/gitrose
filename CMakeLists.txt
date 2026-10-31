cmake_minimum_required(VERSION 3.28)

project(gitrose
    VERSION 0.1.0
    DESCRIPTION "A git client"
    LANGUAGES C CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use a preset: cmake --preset dev")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

option(GITROSE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(GITROSE_ENABLE_CLANG_TIDY "Run clang-tidy as part of compilation" OFF)
option(GITROSE_HARDENING "Enable standard-library hardening assertions (the fast checks outside Debug)" ON)
set(GITROSE_SANITIZERS "" CACHE STRING "Sanitizers to enable, e.g. 'address;undefined' or 'thread'")

# gitrose does not use C++20 modules; skip the per-file dependency scan CMake does otherwise.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GitroseCompilerOptions)
include(GitroseTooling)

# Before the dependencies, so that hixxy and SDL are built with the same sanitizers.
gitrose_enable_sanitizers("${GITROSE_SANITIZERS}")

include(GitroseDependencies)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
