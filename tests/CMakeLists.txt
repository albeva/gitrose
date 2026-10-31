# Tests: headless doctest executables, registered with CTest.

# The doctest runner shared by every unit-test executable.
add_library(gitrose_test_main STATIC test_main.cpp)
target_link_libraries(gitrose_test_main PUBLIC doctest::doctest)
gitrose_configure_target(gitrose_test_main)
gitrose_mark_foreign_source(test_main.cpp)

# gitrose_add_test(<name> SOURCES <source>... [LIBS <library>...])
# Builds the doctest executable <name> from SOURCES, linked to the shared runner and LIBS, with
# src/ and tests/ on its include path, and registers it with CTest.
function(gitrose_add_test name)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "" "SOURCES;LIBS")
    add_executable(${name} ${arg_SOURCES})
    target_link_libraries(${name} PRIVATE gitrose_test_main ${arg_LIBS})
    target_include_directories(${name} PRIVATE "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/tests")
    gitrose_configure_target(${name})
    add_test(NAME ${name} COMMAND ${name} --no-intro=true --force-colors=false)
    set_tests_properties(${name} PROPERTIES TIMEOUT 120)
endfunction()

add_subdirectory(ui)
