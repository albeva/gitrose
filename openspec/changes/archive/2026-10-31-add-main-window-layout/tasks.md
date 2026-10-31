# Tasks

Every task leaves `cmake --build --preset dev` green (warnings are errors) and clang-format clean.

## 1. Module scaffolding

- [x] 1.1 Add `src/ui/` as the static library `gitrose_ui` (namespace `gitrose::ui`, linked to `hixxy::hixxy`, `gitrose_configure_target`) with `style.{hpp,cpp}` installing an empty gitrose style sheet, and `tests/ui/` as `test_ui` linked to `gitrose_ui` and `hixxy::testing`; remove `tests/smoke_test.cpp` and `test_smoke`. Verify: `ctest --preset dev` runs `test_ui`.
- [x] 1.2 Add `sample/sample_repository.{hpp,cpp}`: the `RepositorySample` structs and `samples()` with the `gitrose` and `hixxy` samples (branches with `main` current, tags, remotes, stashes, three worktrees with the main one current, submodules, about 30 commits each with changed files, staged and unstaged files). Verify: a `test_ui` case checks exactly one current branch and one current worktree per sample.

## 2. Splitter

- [x] 2.1 Implement `Splitter::layout()` (pure, `constexpr`) and its drag clamp per `specs/ui/split-panes`. Verify: `test_ui` cases for drag within limits, drag past a minimum, too small for both minimums, each resize mode, the kept pane giving way, and panes plus divider always summing to the length.
- [x] 2.2 Implement the `Splitter` widget (two children, orientation, hairline divider, 3-point grab margin, mouse capture, resize cursors) on top of `layout()`. Verify: a `UiTest` case drags the divider 40 points, including a press 3 points beside the line, and checks both panes' frames.

## 3. Window shell

- [x] 3.1 Add `ToolbarButton` and `ToolbarView` (the twelve items in three groups plus the right-hand group, icons per design, styles in the gitrose sheet). Verify: a `UiTest` case checks the item captions in order and that widening the window grows only the flexible space.
- [x] 3.2 Add `MainWindow::build()` (toolbar above a `TabView` with one placeholder page per sample) and use it in `src/app/main.cpp` with the 1280 × 800 size and 900 × 560 minimum. Verify: `build/dev/src/app/gitrose` opens with the toolbar and the `gitrose` and `hixxy` tabs.

## 4. Sidebar and page

- [x] 4.1 Add `SidebarModel` (section headings and entries from a `RepositorySample`) and `SidebarView` (row delegate with icon, name, detail, the current mark; headings not selectable). Verify: `test_ui` cases check the section order, the current branch and worktree marks, and that selecting a heading keeps the previous selection.
- [x] 4.2 Add `RepositoryPage` (sidebar | `Stack` of the workspace views in a `Splitter` that keeps the sidebar's width; WORKSPACE entries switch the view, History first) with placeholder views, and use it for each tab. Verify: a `UiTest` case selects File Status in the first tab, switches tabs and back, and finds File Status still shown and the second tab on History.

## 5. Workspace views

- [x] 5.1 Add `DiffPlaceholder` and `CommitBrowser` (commit table with the five columns over files | diff, splitters per design, first commit selected) and `HistoryView` with its filter bar. Verify: run the app and check the History layout against `specs/ui/main-window`.
- [x] 5.2 Add `SearchView` (search field and mode choice over a `CommitBrowser`). Verify: run the app, select Search, check the layout.
- [x] 5.3 Add `FileStatusView` (top bar, staged / unstaged lists beside the diff placeholder, commit area with message field and Commit button, splitters per design). Verify: run the app, select File Status, check the layout.

## 6. Verification and wrap-up

- [x] 6.1 Full checks: `ctest --preset dev`, the `format-check` target, the `dev-tidy` build and the `dev-asan` build with its tests. Verify: all pass.
- [x] 6.2 Add backlog item NEXT-1 ("Repository tabs need close and add controls": hixxy's `TabView` has neither; decide between a hixxy change and a gitrose tab strip when tab management is built) to `docs/backlog/01-first-need.md` and the index, moving the section's next ID to NEXT-2. Verify: the index and the section file list it.
- [x] 6.3 Manual check for the user: run `build/dev/src/app/gitrose`; check the toolbar order and its right-hand group, both tabs, the sidebar sections with the current branch and worktree marked, switching between File Status, History and Search, dragging every divider (minimums hold, the sidebar keeps its width on window resize, the commit table keeps its share), the 900 × 560 minimum, and both light and dark schemes.

## Workflow follow-up

- Archive the change with `/opsx:archive` once the user has done the manual check.
