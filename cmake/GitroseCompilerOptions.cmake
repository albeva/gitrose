# Compiler configuration shared by every gitrose target: ccache, sanitizers, warnings, hardening
# and optional clang-tidy.
include_guard(GLOBAL)

# --- ccache ----------------------------------------------------------------------------------
# Every build directory of this checkout shares one cache: CCACHE_BASEDIR makes paths below the
# source tree relative and CCACHE_NOHASHDIR leaves the working directory out of the hash. An
# explicit CMAKE_<LANG>_COMPILER_LAUNCHER wins.
option(GITROSE_CCACHE "Use ccache, when found, as the compiler launcher" ${PROJECT_IS_TOP_LEVEL})
if(GITROSE_CCACHE AND NOT CMAKE_C_COMPILER_LAUNCHER AND NOT CMAKE_CXX_COMPILER_LAUNCHER)
    find_program(GITROSE_CCACHE_EXE NAMES ccache)
    if(GITROSE_CCACHE_EXE)
        set(gitrose_ccache_launcher
            "${CMAKE_COMMAND}" -E env
            "CCACHE_BASEDIR=${PROJECT_SOURCE_DIR}" "CCACHE_NOHASHDIR=1"
            "${GITROSE_CCACHE_EXE}")
        set(CMAKE_C_COMPILER_LAUNCHER ${gitrose_ccache_launcher})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${gitrose_ccache_launcher})
    endif()
endif()

# --- clang-tidy ------------------------------------------------------------------------------
# GITROSE_CLANG_TIDY_EXE, auto-detected when unset or empty. find_program() skips the search for
# a variable that is defined and not *-NOTFOUND, even when it is empty, so an empty value is
# cleared first.
if(NOT GITROSE_CLANG_TIDY_EXE)
    unset(GITROSE_CLANG_TIDY_EXE CACHE)
endif()
find_program(GITROSE_CLANG_TIDY_EXE NAMES clang-tidy DOC "clang-tidy executable")
if(GITROSE_ENABLE_CLANG_TIDY AND NOT GITROSE_CLANG_TIDY_EXE)
    message(FATAL_ERROR "gitrose: GITROSE_ENABLE_CLANG_TIDY is ON but clang-tidy was not found; "
        "set GITROSE_CLANG_TIDY_EXE to its path")
endif()

# --- sanitizers ------------------------------------------------------------------------------
# GITROSE_SANITIZERS="address;undefined" or "thread": applied to every target, dependencies too.
function(gitrose_enable_sanitizers sanitizers)
    if(NOT sanitizers)
        return()
    endif()
    if("thread" IN_LIST sanitizers AND "address" IN_LIST sanitizers)
        message(FATAL_ERROR "ThreadSanitizer cannot be combined with AddressSanitizer")
    endif()

    list(JOIN sanitizers "," joined)
    set(flags -fsanitize=${joined} -fno-omit-frame-pointer -fno-sanitize-recover=all)
    add_compile_options(${flags})
    add_link_options(${flags})
    message(STATUS "gitrose: sanitizers enabled: ${joined}")
endfunction()

# --- warnings --------------------------------------------------------------------------------
# Strict warning set. Third-party code is consumed through SYSTEM include paths (FetchContent's
# SYSTEM for hixxy and doctest, SYSTEM on an installed SDL's headers), so it is never subject to
# these flags.
function(gitrose_target_warnings target)
    set(common_gcc_clang
        -Wall -Wextra -Wpedantic
        -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wcast-qual
        -Wunused -Woverloaded-virtual -Wsuggest-override
        -Wconversion -Wsign-conversion -Wdouble-promotion
        -Wnull-dereference -Wformat=2 -Wimplicit-fallthrough
        -Wextra-semi -Wundef -Wmissing-declarations)
    set(clang_only
        -Wshadow-all -Wcomma -Wloop-analysis -Wunreachable-code-aggressive
        -Wnewline-eof -Wdeprecated -Wdocumentation -Wheader-hygiene -Wrange-loop-analysis
        -Wmissing-prototypes -Wmissing-variable-declarations)
    set(gcc_only
        -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op
        -Wuseless-cast -Wredundant-decls)

    target_compile_options(${target} PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:${common_gcc_clang}>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${clang_only}>
        $<$<CXX_COMPILER_ID:GNU>:${gcc_only}>)

    if(GITROSE_WARNINGS_AS_ERRORS)
        set_target_properties(${target} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    endif()
endfunction()

# Applies the full gitrose build policy to a target: language level, warnings, hardening,
# optional clang-tidy.
function(gitrose_configure_target target)
    target_compile_features(${target} PUBLIC cxx_std_23)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    gitrose_target_warnings(${target})

    # Standard-library hardening in every build type, so out-of-range container and span access
    # traps rather than being undefined behaviour: libc++'s debug mode in Debug, its fast mode in
    # every other build type; and libstdc++'s assertions.
    if(GITROSE_HARDENING)
        target_compile_definitions(${target} PRIVATE
            $<IF:$<CONFIG:Debug>,_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG,_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST>
            _GLIBCXX_ASSERTIONS)
    endif()

    if(GITROSE_ENABLE_CLANG_TIDY)
        set_target_properties(${target} PROPERTIES
            CXX_CLANG_TIDY "${GITROSE_CLANG_TIDY_EXE};--quiet")
    endif()
endfunction()

# Marks a source file as third-party: no warnings, no linting.
function(gitrose_mark_foreign_source)
    foreach(src IN LISTS ARGN)
        set_source_files_properties(${src} PROPERTIES
            SKIP_LINTING ON
            COMPILE_OPTIONS "-w")
    endforeach()
endfunction()
