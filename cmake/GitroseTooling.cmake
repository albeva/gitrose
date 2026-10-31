# Developer tooling targets:
#   format        reformat all first-party sources in place (clang-format)
#   format-check  fail if any first-party source is not formatted
include_guard(GLOBAL)

# GITROSE_CLANG_FORMAT_EXE, auto-detected when unset or empty (an empty value is cleared first: see
# GITROSE_CLANG_TIDY_EXE in GitroseCompilerOptions.cmake). Without clang-format there are no targets.
if(NOT GITROSE_CLANG_FORMAT_EXE)
    unset(GITROSE_CLANG_FORMAT_EXE CACHE)
endif()
find_program(GITROSE_CLANG_FORMAT_EXE NAMES clang-format DOC "clang-format executable")
if(NOT GITROSE_CLANG_FORMAT_EXE AND PROJECT_IS_TOP_LEVEL)
    message(STATUS "gitrose: clang-format not found - no format/format-check targets (set GITROSE_CLANG_FORMAT_EXE)")
endif()

if(GITROSE_CLANG_FORMAT_EXE)
    file(GLOB_RECURSE gitrose_format_sources CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/tests/*.hpp" "${PROJECT_SOURCE_DIR}/tests/*.cpp")
    add_custom_target(format
        COMMAND "${GITROSE_CLANG_FORMAT_EXE}" -i --style=file ${gitrose_format_sources}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "clang-format: formatting sources"
        VERBATIM)
    add_custom_target(format-check
        COMMAND "${GITROSE_CLANG_FORMAT_EXE}" --dry-run --Werror --style=file ${gitrose_format_sources}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "clang-format: checking formatting"
        VERBATIM)
endif()
