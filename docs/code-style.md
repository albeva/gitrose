# gitrose code style

Imported from hixxy's `docs/code-style.md` (github.com/albeva/hixxy) by way of looprail, and kept
in step with it, so the codebases read alike.

How gitrose C++ code is written and laid out. Two layers:

1. **clang-format** (`.clang-format`, checked by `cmake --build --preset dev --target
   format-check`) owns everything it can decide on its own: indentation, spaces, include order,
   brace placement.
2. **Manual layout rules** (below) cover what clang-format leaves alone. The configuration has
   no column limit (`ColumnLimit: 0`), so clang-format keeps the line breaks it finds: *where*
   to break is the author's job, and these rules make that choice consistent.

Naming, Doxygen, errors, ownership and the other non-negotiable rules are in
`.claude/rules/cpp.md`; this document is about layout, `const`,
`constexpr` and `noexcept` (§4) and how declarations are written (§5).

Workflow: write the code following the rules below, run `clang-format -i <files>`, and keep what
clang-format produces. Every layout in this document is stable under clang-format (it does not
undo it).

## 0. File header

No licence header yet: the repository is private and has no licence.
A header file starts with `#pragma once`, then its includes. A source file may open with a short
`//` comment saying what the file is for.

## 1. What clang-format does (`.clang-format`)

Based on **WebKit**, with these effective settings:

| Topic | Setting | Result |
|---|---|---|
| Indentation | `IndentWidth: 4`, `ContinuationIndentWidth: 4` | 4 spaces, never tabs; continuation lines +4 |
| Line length | `ColumnLimit: 0` | no automatic wrapping; existing breaks are kept |
| Braces | `BreakBeforeBraces: Attach` | `if (x) {`, `auto f() -> void {`, `class A {` on the same line |
| Braced lists | `Cpp11BracedListStyle: Block`, `SpaceBeforeCpp11BracedList: true` | spaces inside and before: `Point { 1.0F, 2.0F }`, `return { a, b };`, empty `{}` |
| Namespaces | `NamespaceIndentation: Inner`, `FixNamespaceComments: true` | top-level namespace bodies are not indented, nested ones (`namespace detail {`, `namespace {` inside `gitrose`) are; closing `} // namespace x` |
| Binary operators | `BreakBeforeBinaryOperators: All`, `BreakBeforeTernaryOperators: true` | a wrapped expression starts each line with its operator (`&&`, `+`, `?`, `:`) |
| Operand alignment | `AlignOperands: AlignAfterOperator` | continuation operands of an assignment or ternary are aligned under the first operand |
| Brackets | `AlignAfterOpenBracket: false`, `BreakBeforeCloseBracket*: true` | wrapped arguments indent by 4 (not aligned to the bracket); a closing `)`/`}` may sit on its own line |
| Constructor initializers | `BreakConstructorInitializers: BeforeComma`, `ConstructorInitializerIndentWidth: 0` | `: m_a(a)` / `, m_b(b)` at the start of the line, flush with the signature |
| Short forms | `AllowShortFunctionsOnASingleLine: All`, `AllowShortBlocksOnASingleLine: Empty`, `AllowShortIfStatementsOnASingleLine: Never`, `AllowShortLoopsOnASingleLine: false` | one-line functions allowed; `if`/loops always have a braced, multi-line body (braces are required: clang-tidy's `readability-braces-around-statements`; `InsertBraces` is off) |
| Pointers and references | `PointerAlignment: Left` | `const Widget* widget`, `String& text` |
| Templates | `SpaceAfterTemplateKeyword: false`, `BreakTemplateDeclarations: MultiLine`, `RequiresClausePosition: OwnLine` | `template<typename T>`; a `requires` clause on its own line |
| Access specifiers | `AccessModifierOffset: -4`, `EmptyLineBeforeAccessModifier: LogicalBlock` | `public:` flush with `class`; a blank line before it unless it opens the class |
| Blank lines | `MaxEmptyLinesToKeep: 1` | never two blank lines in a row |
| Includes | `SortIncludes: CaseSensitive`, `IncludeCategories` | the source's own header first; then standard library `<vector>`, third party `<SDL3/...>`/`<doctest/...>`, hixxy `<hixxy/...>`, then gitrose's own `"..."`. Sorted within each block; the blank lines between groups are the author's (`IncludeBlocks: Preserve`) |
| Other | `SpaceInEmptyBraces: Never`, `AlignTrailingComments: true`, `AlignConsecutiveBitFields: true` | `{}`; trailing comments of consecutive lines aligned |

## 2. Line length

There is no hard limit, but keep lines readable:

- Aim for **100 columns**. A line over **120 columns** that has a natural break (an argument or
  parameter list, a braced list, an operator chain) is broken by the rules below.
- A line that cannot be broken sensibly (a long string literal, a URL, a single long call with
  one short argument) may stay long.
- When a construct is broken, it is broken **completely** (one item per line, rules 3.1 to 3.3).
  Never use the old "bin-packed" style that fills a line and hangs the rest under it.
- A previously wrapped construct whose one-line form fits in 100 columns and holds no lambda is
  joined onto one line instead.

## 3. Manual layout rules

### 3.1 Braced initializer lists: one element per line

A braced list that does not fit on one line opens with `{` at the end of the line, has one
element per line (indented +4), and closes with `}` on its own line, aligned with the start of
the statement.

```cpp
// Bad
return { utf8::floor_codepoint_boundary(m_data, begin),
    utf8::floor_codepoint_boundary(m_data, end) };

// Good
return {
    utf8::floor_codepoint_boundary(m_data, begin),
    utf8::floor_codepoint_boundary(m_data, end)
};
```

Designated initializers follow the same rule; a short one stays on one line
(`*slot.record = { .offset = offset, .length = length };`). A braced list that is the only argument
of a call hugs the parentheses:

```cpp
auto window = app.create_window({
    .title = "gitrose",
    .size = { 420.0F, 220.0F },
});
```

clang-format keeps this block form for a statement-level list (`return {`, `T x {`, `x = {`)
and for one that ends with a **trailing comma**. A list without a trailing comma that is a call
argument, an element of another list or the range of a `for` is re-packed by clang-format (the
closing `}` moves onto the last element, or the list hangs). In new code, write such a list with
a trailing comma after the last element, as above, so the block form sticks; in existing code
the pass that introduced this guide kept clang-format's layout, since adding a comma is a token
change.

Raw data tables (pixel bytes, texels, coverage maps) may keep one *row* per line inside the
block form, so the data keeps its shape.

### 3.2 Function signatures: one parameter per line

A declaration or definition whose signature does not fit on one line breaks after `(`, puts each
parameter on its own line (indented +4), and closes with `)` on its own line, followed by the
qualifiers and the brace.

```cpp
// Bad
auto String::checked_range(std::size_t offset,
    std::size_t count) const noexcept -> std::pair<std::size_t, std::size_t> {

// Good
auto String::checked_range(
    std::size_t offset,
    std::size_t count
) const noexcept -> std::pair<std::size_t, std::size_t> {
```

The same applies to declarations in headers (`) const noexcept -> T;`), constructors (the initializer
list follows the closing `)` as clang-format lays it out) and lambda parameter lists.

### 3.3 Function calls: one argument per line

A call (including macro calls such as `assert`, `CHECK`, `log::debug`, and calls of
`std::format`) that does not fit on one line, or that takes a lambda with a body of more than a
trivial expression, breaks after `(`, puts each argument on its own line (indented +4), and
closes with `)` on its own line. A lambda argument gets its body on its own lines.

```cpp
// Bad
std::erase_if(span.classes,
    [name](const String& item) { return item.view() == name.view(); });

// Good
std::erase_if(
    span.classes,
    [name](const String& item) {
        return item.view() == name.view();
    }
);
```

```cpp
log::debug(
    "repository {}: branch \"{}\" at {}",
    repository.path().string(),
    branch.name(),
    branch.target().short_hex()
);
```

- Nested calls that fit stay on their argument's line; a nested call that does not fit is broken
  by the same rule, one level deeper.
- A call whose **last** argument is a lambda (or a braced list) and whose other arguments are
  short may keep them on the first line and hug the lambda: `run(a, b, [&](int v) {` ... `});`.
- clang-format sometimes keeps a closing `);` on the last argument's line: move it to its own
  line where clang-format accepts that, otherwise leave clang-format's choice.
- A broken call followed by a member chain (`make(\n...\n).flex(1.0F)`) gets an awkward
  indentation from clang-format; prefer keeping such a call on one line, or store the call's
  result in a variable first. The same goes for `(std::max)(...)`-style calls, which clang-format
  will not break one argument per line.

### 3.4 Lambdas assigned to variables: never on one line

A lambda stored in a variable has its body on its own lines, even when it is a single statement.

```cpp
// Bad
const auto grown = [step](int extent) { return ((extent + step - 1) / step) * step; };

// Good
const auto grown = [step](int extent) {
    return ((extent + step - 1) / step) * step;
};
```

A short lambda passed inline to a call that fits on one line (a comparator, a predicate) may stay
on one line: `std::ranges::sort(items, [](int a, int b) { return a < b; });`.

### 3.5 Long expressions: one operand per line

An expression with several distinct parts (a chain of `&&`/`||`, a long sum, a long ternary)
that does not fit on one line puts each part on its own line, **starting** with the operator.

```cpp
// Bad
return last.backdrop_blur == 0.0F && last.texture == texture && last.clip == clip && static_cast<std::size_t>(last.index_offset) + last.index_count == index_offset;

// Good
return last.backdrop_blur == 0.0F
    && last.texture == texture
    && last.clip == clip
    && static_cast<std::size_t>(last.index_offset) + last.index_count == index_offset;
```

- Break at the **outermost** operator only; keep grouped sub-expressions (`(a || b)`) together.
- In `if`/`while` conditions the parts start at the condition's indentation:

  ```cpp
  if (first_condition
      && second_condition
      && third_condition) {
  ```

- In an assignment or a ternary, clang-format aligns the continuation operands under the first
  operand (`AlignOperands`); keep its layout.

### 3.6 Blank lines: no walls of code

Separate the steps of a function with single blank lines, so that a block of code reads as a
sequence of paragraphs:

- A blank line **after a closing `}`** of a block (`if`, `for`, `while`, `switch`, a lambda
  `};`, a `});` call) when another statement follows in the same scope. Not before `else`,
  `catch`, a closing `}`, or `case`/`default`.
- A blank line before a comment that introduces a new step.
- No blank line directly after an opening `{` or before a closing `}`.
- Declarations that belong together (computing the inputs of the next step) stay together.

```cpp
for (std::uint64_t i = 0; i < table_count; ++i) {
    const auto record = base + 12 + (16 * i);
    const auto slot = table_slot(tables, file.u32(record));
    if (slot.record == nullptr || slot.record->seen) {
        continue; // like stb_truetype, the first record of a tag counts
    }

    const auto offset = std::uint64_t { file.u32(record + 8) };
    const auto length = std::uint64_t { file.u32(record + 12) };
    if (!file.has(offset, length)) {
        return decode_error("table outside of the file");
    }

    if (!inside) {
        return decode_error("table beyond 2 GiB");
    }

    // stb_truetype treats a table at offset 0 (where the directory is) as missing.
    *slot.record = { .offset = offset, .length = length, .present = offset != 0, .seen = true };
}
```

## 4. `const`, `constexpr` and `noexcept`

Use `const` and `constexpr` wherever they are possible, and `noexcept` wherever a function cannot
throw. These are requirements, not preferences.

### 4.1 `const` member functions

A member function that does not modify the object's observable state is `const`. `mutable` is
reserved for caches, memoization and synchronisation (a cache filled on first use, a mutex).
clang-tidy's `readability-make-member-function-const` reports the cases it can prove.

### 4.2 `const` value parameters in definitions

In a function **definition** (out-of-line definitions, inline member functions, lambdas), a
by-value parameter that the body never modifies is `const`. Declarations that are not definitions
keep the plain type, because top-level `const` is not part of the signature
(`readability-avoid-const-params-in-decls`).

```cpp
// widget.hpp (declaration)
auto set_spacing(float spacing, bool animate) noexcept -> void;

// widget.cpp (definition)
auto Widget::set_spacing(const float spacing, const bool animate) noexcept -> void {
```

- A pointer parameter that is never reassigned is a const pointer: `Widget* const widget` (the
  pointee keeps the constness the API needs).
- A parameter that is moved from (`std::move(text)`), modified, or passed to a non-const
  reference stays non-const. A `const` value would silently turn its moves into copies.
- Reference parameters are already `const T&` or deliberately `T&`/`T&&`.
- Parameters of function templates whose type depends on a template parameter are left alone.

clang-tidy's `misc-const-correctness` covers locals; this rule and §4.5 are checked in review
(hixxy's `tools/constfix.sh` is not ported yet).

### 4.3 `const` local variables

A local variable that is not modified after its initialization is `const`. This includes pointers
that are never reassigned (`auto* const focused = ...`), structured bindings
(`const auto [begin, end] = ...`) and range-for variables (`for (const auto& item : items)`).
`misc-const-correctness` enforces it, pointers included.

### 4.4 `constexpr`

Anything that can be evaluated at compile time is `constexpr`:

- **Values.** A value known at compile time is `constexpr`, not `const`: constants at namespace
  scope (`inline constexpr` in headers), `static constexpr` class members, and locals initialised
  from constant expressions.
- **Functions.** A function that can run at compile time is `constexpr`: computations on literal
  types such as maths, geometry, object-id and offset conversions, diff arithmetic and lookups
  in constant tables. C++23 allows almost the whole language in a `constexpr` function; what
  usually rules it out is a call to something that is not `constexpr` (I/O, SDL, hixxy, the
  allocating parts of the standard library, `std::sqrt` and most of `<cmath>`).
- **Types.** Constructors and member functions of literal types (object ids, line ranges,
  small value types) are `constexpr` too; default member initialisers keep a class literal.
- `consteval` for functions that must only run at compile time (building lookup tables,
  validating constants); `if constexpr` for compile-time branches.
- A `constexpr` function belongs in the header, so callers can evaluate it at compile time (it
  is implicitly `inline`).
- A unit test may check a `constexpr` function with `static_assert` where that is the simplest
  test.

```cpp
struct ObjectId {
    static constexpr auto size = std::size_t { 20 };

    static constexpr auto hex_length(const std::size_t bytes) noexcept -> std::size_t {
        return bytes * 2;
    }
};
```

No clang-tidy check covers this rule; review does.

### 4.5 `noexcept`

A function that cannot throw is `noexcept`. It cannot throw when:

- its body has no `throw`, no `new` and no `try`;
- every function it calls and every constructor it runs is `noexcept` (or a C function, or a
  trivial operation);
- it does not allocate through a throwing API. `std::bad_alloc` propagates, so a function that builds a string, grows a `std::vector` or
  formats text is **not** `noexcept`.

Notes:

- `noexcept` goes on every declaration of the function: the declaration in the header and the
  definition in the source.
- A virtual function is `noexcept` only when the contract requires it of every override (the
  override of a user's subclass included), or when nothing can override it again (`final`, or a
  `final` class). Adding `noexcept` to a non-final override would break every further override
  that is not `noexcept`.
- Destructors are implicitly `noexcept`. Move operations and `swap` are `noexcept`, as clang-tidy
  `cppcoreguidelines-noexcept-*` requires.
- A template is `noexcept` when its body cannot throw for any valid template argument (e.g.
  arithmetic on a numeric `T`). Otherwise it stays unmarked; use a conditional
  `noexcept(...)` only where it carries its weight.
- `main()` and test bodies are not marked.

## 5. Declarations

### 5.1 Trailing return types

Every function in gitrose's code is declared with a trailing return type,
`auto name(parameters) qualifiers -> ReturnType`: free functions, member functions, operators,
function templates, `void` functions and `main`.

```cpp
// Bad
int commit_count(const Branch& branch);
void Diff::clear() noexcept {

// Good
auto commit_count(const Branch& branch) -> int;
auto Diff::clear() noexcept -> void {
auto main() -> int {
```

- Qualifiers and `noexcept` come before the arrow; `override` and `final` after the return type,
  then `= 0`, `= default` or the body: `auto speed() const noexcept -> float override;`,
  `virtual auto name() const -> std::string_view = 0;`.
- Constructors, destructors and conversion operators have no return type and are left alone.
- Lambdas keep their deduced return type; one that needs an explicit type writes it trailing
  anyway (`[](const int x) -> float { ... }`).
- A function with a deduced return type (`auto`, `decltype(auto)`) has no arrow; deduce only
  where the type cannot be spelled reasonably.

clang-tidy's `modernize-use-trailing-return-type` enforces the rule (lambdas excluded in
`.clang-tidy`). It skips `void` functions, so review checks those.

### 5.2 `auto` variables

A variable initialised from an expression is declared with `auto`, wherever that gives it the
type it should have. The type is then stated once, on the right, or follows from the
expression.

```cpp
// Bad
const int count = map.tile_count();
Station* const station = world.station_at(tile);
std::vector<TrainId> trains = depot.trains();

// Good
const auto count = map.tile_count();
auto* const station = world.station_at(tile);
auto trains = depot.trains();
```

- `const` and `constexpr` apply as usual (§4): `const auto`, `constexpr auto`.
- Pointers are `auto*` / `const auto*`, references `auto&` / `const auto&`
  (`readability-qualified-auto`).
- When the variable needs a type other than the expression's (a widening, a specific integer
  type), name the type on the right: `const auto offset = std::uint64_t { file.u32(at) };`,
  `const auto index = static_cast<std::size_t>(tile.x);`. Numeric literals carry their type in a
  suffix (`1.5F`, `16U`).
- Initialise with `=`, never `auto x { ... }` or `auto x = { ... }` (the latter is a
  `std::initializer_list`).
- Spell the type out when `auto` would deduce a proxy or view that must not outlive its source
  (`std::vector<bool>::reference`, an expression template), or when there is no initialiser.

clang-tidy's `modernize-use-auto` catches the plainest cases (casts, `new`, iterators); review
checks the rest.

### 5.3 `using namespace` in source files

A source file (`.cpp`) uses the gitrose namespace it implements (`using namespace
gitrose::ui;`, `using namespace gitrose::git;`, ...) and, where its module may use hixxy,
`using namespace hixxy;`, instead of reopening the namespace around
its definitions. It keeps `hixxy::` and `gitrose::` clutter out of the code.

```cpp
#include "ui/screens/history/history.hpp"

#include <hixxy/hixxy.hpp>

using namespace hixxy;
using namespace gitrose::ui;

namespace {

/// File-local helpers stay in an anonymous namespace.
auto add_divider(Column& screen) -> void {
    screen.add<Container>().fixed_height(1.0F).add_class("divider");
}

} // namespace

HistoryScreen::HistoryScreen() {   // members: found through the using-directive
    add_divider(*this);
}

auto gitrose::ui::apply_style(Application& app) -> void {   // free functions: qualified
```

- Member functions are defined as `Class::member`. A free function declared in a header is
  defined with its qualified name (`auto gitrose::ui::apply_style(...) -> void`): an
  unqualified definition would declare a new global function, which the build rejects
  (`-Wmissing-prototypes` with clang, `-Wmissing-declarations` with GCC). So does any function
  with external linkage and no earlier declaration (and, with clang, any such variable): file-local
  helpers, tests' included, go in an anonymous namespace.
- Only in `.cpp` files, after the includes or inside a function. Never in headers (clang's
  `-Wheader-hygiene` rejects it there): headers qualify every hixxy name and open their own
  namespace (`namespace gitrose::ui { ... }`).
- Never `std` or any other namespace: standard library names stay qualified (`std::vector`,
  `std::span`).
- When a name exists in both hixxy and gitrose, the compiler reports the ambiguous use;
  qualify that use.

### 5.4 Member functions over free functions

A strong suggestion rather than a hard rule: an operation belongs to the type it works on, as a
member function (or a static member function), not as a function at namespace scope. A member
is found where its type is, can rely on the type's invariants and private state, and reads as
what it asks: `commit.parent(index)` rather than a free `parent()` that has to be told
the commit's parent list.

```cpp
// Bad
auto parent(const Commit& commit, std::size_t index) noexcept -> ObjectId;

// Good
class Commit {
public:
    auto parent(std::size_t index) const noexcept -> ObjectId;
};
```

Free functions remain the right choice for:

- operations on a namespace-scope enum, which cannot have members. Nesting the enum (§5.5) is
  usually better: its operations then become static members of the enclosing type
  (`FileStatus::is_staged(Change)`);
- operators and other functions found by argument-dependent lookup (`operator==`, `swap`);
- file-local helpers in an anonymous namespace, in a `.cpp` file or a test;
- generic algorithms over unrelated types (function templates).

### 5.5 Nested enums

Like §5.4, a strong suggestion rather than a hard rule: an enum that means something only
together with one value type is nested in that type, `FileStatus::Change` rather than a
namespace-scope `Change`. The name then says what the enum belongs to, it is found where the
type is, and it does not crowd the namespace. The type's operations on the enum become its
static members (`FileStatus::is_staged(Change)`, §5.4). Another type may still use a nested enum
by its qualified name: `Index::filter()` takes a `FileStatus::Change`. An enum stays at namespace
scope only when it belongs to no one type.

```cpp
// Bad
enum class Change : std::uint8_t { Added, Modified, Deleted };

struct FileStatus {
    Change change = Change::Modified;
};

// Good
struct FileStatus {
    enum class Change : std::uint8_t { Added, Modified, Deleted };

    Change change = Change::Modified;
};
```

## 6. Not covered here

- Generated files follow their generator's layout.
- Raw string literals (CSS, JSON in tests) are content, not code: never reflow them.
- Comments wrap at about 100 columns (Doxygen `///` included); clang-format does not reflow them.
