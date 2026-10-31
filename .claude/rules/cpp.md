---
paths:
  - "src/**/*.cpp"
  - "src/**/*.hpp"
  - "tests/**/*.cpp"
  - "tests/**/*.hpp"
  - "tools/**/*.cpp"
  - "tools/**/*.hpp"
---

# C++ rules (gitrose)

- C++23, portable subset only: **no** `std::move_only_function`
  (use `hixxy::UniqueFunction`), `views::enumerate`, `std::generator`, `std::flat_map`,
  `std::stacktrace`, `#embed`.
- Zero warnings with the project warning set (`-Wconversion -Wsign-conversion
  -Wshadow-all -Wold-style-cast -Wdocumentation ...`). Use explicit `static_cast`; never silence
  warnings with pragmas in first-party code.
- clang-tidy clean (`.clang-tidy`). `NOLINT` only with a specific check name and a reason.
- No UB, no leaks: RAII everywhere, no raw `new`/`delete`, no `reinterpret_cast` except at C API
  boundaries (with `// NOLINT(cppcoreguidelines-pro-type-reinterpret-cast): <reason>`),
  bounds-safe access (`std::span`, `.at()` in non-hot paths, asserts). Code passes under ASan
  and UBSan (and TSan for code that runs on several threads).
- No platform `#ifdef`s outside the code that talks to SDL.
- Errors via `std::expected` (`hixxy::Result<T>` at the hixxy boundary); gitrose code throws no
  exceptions.
- Every declaration in a header gets a Doxygen `///` comment: simple, clear, concise,
  self-contained. Headers are self-contained.
- Naming: `CamelCase` types/enumerators, `snake_case` functions/variables, `m_` private members,
  `GITROSE_` macros, namespace `gitrose`. Files `snake_case.hpp`/`.cpp`.
- `const`, `constexpr` and `noexcept` wherever possible (`docs/code-style.md` §4): compile-time
  values are `constexpr`; so are functions and constructors that can run at compile time.
- Trailing return types on every function, `void` and `main` included: `auto f() -> T`
  (`docs/code-style.md` §5.1). Not on constructors, destructors, conversion operators or
  lambdas with deduced return types.
- `auto` for variables initialised from an expression (`const auto n = f();`, `auto* p`,
  `const auto&`); a different type goes on the right: `auto x = std::uint64_t { y };`
  (`docs/code-style.md` §5.2).
- `.cpp` files use the gitrose namespace they implement (`using namespace gitrose::ui;`) and,
  in modules that may use hixxy, `using namespace hixxy;`, after the includes, instead of
  reopening the namespace: members are defined as `Class::member`, free functions with their
  qualified name (`auto gitrose::ui::f() -> void`), file-local helpers in an anonymous
  namespace. Never in headers, never `std` or any other namespace (`docs/code-style.md` §5.3).
- Prefer operations as members (or static members) of the type they work on, and enums that
  belong to one type nested in it (`FileStatus::Change`): strong suggestions, not hard rules
  (`docs/code-style.md` §5.4–§5.5).
- Tests: doctest, headless; unit tests only, lean and high-value (public contracts, tricky edge
  cases, regressions).
- No licence header (`docs/code-style.md` §0); headers start with `#pragma once`.
- Layout: `docs/code-style.md` (clang-format plus the manual rules it cannot apply).
