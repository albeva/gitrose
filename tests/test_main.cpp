// The doctest runner shared by every gitrose unit-test executable.
//
// <cfloat> is included explicitly: doctest's implementation uses DBL_EPSILON but relies on a
// transitive include that newer libc++ releases no longer provide.
#include <cfloat> // IWYU pragma: keep

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
