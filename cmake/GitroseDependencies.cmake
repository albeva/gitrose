# Third-party dependencies. Policy: keep the list short; pin every one to an exact version.
#   - hixxy     UI toolkit (github.com/albeva/hixxy, private); brings SDL3 with it
#   - doctest   unit tests only (tests/)
#
# Co-development: set FETCHCONTENT_SOURCE_DIR_HIXXY to a local hixxy checkout (the `hixxy-local`
# user preset, CMakeUserPresets.example.json) to build against its working tree instead of the
# pinned commit. Bump GITROSE_HIXXY_COMMIT once the hixxy change gitrose needs is pushed.
include_guard(GLOBAL)

include(FetchContent)

# --- hixxy -----------------------------------------------------------------------------------
# hixxy finds an installed SDL3 >= its pinned release (CMAKE_PREFIX_PATH; the `local-deps` user
# preset points it at hixxy's prebuilt one) and otherwise fetches and builds SDL itself.
set(GITROSE_HIXXY_REPOSITORY "git@github.com:albeva/hixxy.git")
set(GITROSE_HIXXY_COMMIT "1e68a4eb51dc9ce84f9f28b15cf97ee15925f524")

FetchContent_Declare(hixxy
    GIT_REPOSITORY "${GITROSE_HIXXY_REPOSITORY}"
    GIT_TAG "${GITROSE_HIXXY_COMMIT}"
    SYSTEM
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(hixxy)

if(NOT TARGET hixxy::hixxy)
    message(FATAL_ERROR "gitrose: hixxy::hixxy target is unavailable")
endif()
if(FETCHCONTENT_SOURCE_DIR_HIXXY)
    message(STATUS "gitrose: hixxy from the local checkout ${FETCHCONTENT_SOURCE_DIR_HIXXY}")
else()
    message(STATUS "gitrose: hixxy at the pinned commit ${GITROSE_HIXXY_COMMIT}")
endif()

# --- doctest ---------------------------------------------------------------------------------
# v2.5.3, the release hixxy vendors. Only its header target (doctest::doctest) is used; the
# runner is tests/test_main.cpp.
set(GITROSE_DOCTEST_REPOSITORY "https://github.com/doctest/doctest.git")
set(GITROSE_DOCTEST_COMMIT "2d0a9359a60c51affe2a9bebb1be1dca47868151") # v2.5.3

set(DOCTEST_WITH_TESTS OFF)
set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF)
set(DOCTEST_NO_INSTALL ON)
FetchContent_Declare(doctest
    GIT_REPOSITORY "${GITROSE_DOCTEST_REPOSITORY}"
    GIT_TAG "${GITROSE_DOCTEST_COMMIT}"
    SYSTEM
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(doctest)

if(NOT TARGET doctest::doctest)
    message(FATAL_ERROR "gitrose: doctest::doctest target is unavailable")
endif()
