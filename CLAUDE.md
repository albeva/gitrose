# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

gitrose is a git client with a UI built on hixxy (github.com/albeva/hixxy, the author's UI
toolkit). C++23. The project setup (style, tooling, CMake, backlog) follows looprail
(`~/Developer/looprail`), the author's other hixxy application.

Where knowledge lives:
- `docs/code-style.md`: layout; `const`, `constexpr` and `noexcept`; trailing return types,
  `auto` variables, `using namespace`, member functions over free functions and nested enums
  (§5). `.claude/rules/cpp.md` (loaded for C++ sources): the non-negotiable C++ rules.
- `docs/backlog.md`: deferred work (below).
- `openspec/`: OpenSpec specs and changes, for work that spans sessions (`/opsx:*` commands).

## Build & test (this machine)

Homebrew's LLVM (`/opt/homebrew/opt/llvm/bin/{clang++,clang-format,clang-tidy}`), CMake, Ninja,
ccache. Presets in `CMakeUserPresets.json` (copied from `CMakeUserPresets.example.json`): `dev*`
build against the local hixxy checkout (`~/Developer/hixxy`) and its prebuilt SDL
(`~/Developer/hixxy/.cache/sdl3`); `dev-pinned` against the pinned commit.

```bash
cmake --preset dev && cmake --build --preset dev   # Homebrew clang, Debug, -Werror, hardening
ctest --preset dev                                 # all tests
build/dev/tests/ui/test_ui -tc="<test name>"       # one doctest executable, filtered to a case
build/dev/src/app/gitrose                          # run the app
cmake --build --preset dev --target format-check   # clang-format check (`format` fixes in place)
cmake --preset dev-tidy && cmake --build --preset dev-tidy   # build with clang-tidy as errors
cmake --preset dev-asan && cmake --build --preset dev-asan && ctest --preset dev-asan   # ASan+UBSan (dev-tsan: TSan)
```

## Build structure

- `CMakeLists.txt` → `cmake/GitroseCompilerOptions.cmake` (ccache, sanitizers, the warning set,
  libc++ hardening, clang-tidy), `cmake/GitroseTooling.cmake` (`format`, `format-check`),
  `cmake/GitroseDependencies.cmake` (hixxy and doctest via FetchContent, pinned, `SYSTEM`).
- Every first-party target calls `gitrose_configure_target(<target>)`; tests are added in
  `tests/CMakeLists.txt` with `gitrose_add_test(<name> SOURCES ... LIBS ...)` (doctest, runner in
  `tests/test_main.cpp`, `src/` and `tests/` on the include path).
- `src/<module>/`: a static library per module; the executable is `src/app/`. `tests/<module>/`
  mirrors it, one test executable per module (`test_ui`; hixxy's headless `UiTest` harness comes
  from `hixxy::testing`).

## hixxy

gitrose consumes hixxy; it does not patch it. When gitrose needs something from hixxy, the
change is made in `~/Developer/hixxy` under its own CLAUDE.md, in a separate session; the `dev`
presets pick it up from the working tree at once. Before pushing gitrose work that needs it, the
hixxy commit must be pushed and `GITROSE_HIXXY_COMMIT` in `cmake/GitroseDependencies.cmake`
bumped (check with the `dev-pinned` preset). hixxy's own samples (`~/Developer/hixxy/samples/`)
show the API in use.

## Commits and the backlog

Commit and push only when the user asks. The backlog is local, in `docs/backlog.md` (no GitHub
issues): an index, each item's details in its section's file in `docs/backlog/`. Each item has an
ID (`LTR-3`), never reused. Add deferred work to the section's file under the section's next ID,
with a line in the index; when work takes an item on, remove it from both.

`.claude/hooks/format-changed.sh` clang-formats every changed or untracked C++ file when wired
as a Stop hook in `.claude/settings.json`.
