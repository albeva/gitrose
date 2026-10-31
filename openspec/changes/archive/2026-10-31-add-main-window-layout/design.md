# Design

## Context

`src/app/main.cpp` opens one hixxy window with a label; there is no other module. The look and
behaviour are in the specs (`specs/ui/main-window`, `specs/ui/split-panes`). hixxy, as pinned,
offers what most of the layout needs: `TabView`, `TableView` (fixed header, resizable columns,
virtualized rows), `ListView` with custom row delegates, `Panel`, `Row`/`Column`/`Stack`,
`ComboBox`, `TextField`, `TextArea`, lucide icons (including the git ones) and custom widgets
with mouse capture and cursors. It has **no split view** and **no tree view**, and `TabView`
tabs have no close button and no trailing "+" control. gitrose consumes hixxy without patching
it (CLAUDE.md), and the user chose to build the splitter in gitrose.

## Goals / Non-Goals

**Goals:**
- A `ui` module whose structure the git-backed changes can fill in place: each view takes its
  data from plain sample structs today and from the `git` module later, without reshaping the
  widget tree.
- A `Splitter` whose geometry is a pure function, unit-tested without a window.

**Non-Goals:**
- Sidebar entries grouped into a tree (`feature/` folders, remotes with their branches): entries
  are flat under their section until a tree is needed.
- Collapsible sidebar sections, keyboard control of the divider, the menu bar.
- Any styling beyond what the layout needs to read like Sourcetree (no custom theme).

## Decisions

### Module and file layout

New static library `gitrose_ui` in `src/ui/`, linked `PUBLIC` to `hixxy::hixxy`, namespace
`gitrose::ui`, following looprail's `ui` layout (a folder per major component):

```
src/ui/
  main_window.{hpp,cpp}        MainWindow::build(Window&): toolbar above the repository TabView
  style.{hpp,cpp}              gitrose's style sheet (toolbar items, sidebar headings, divider)
  sample/sample_repository.{hpp,cpp}
                               plain structs + the two samples (no hixxy types)
  widgets/splitter.{hpp,cpp}   Splitter and its pure geometry
  widgets/toolbar_button.{hpp,cpp}
  toolbar/toolbar_view.{hpp,cpp}
  repository/repository_page.{hpp,cpp}
                               one tab's page: Splitter(sidebar | Stack of workspace views)
  sidebar/sidebar_model.{hpp,cpp}, sidebar/sidebar_view.{hpp,cpp}
  workspace/commit_browser.{hpp,cpp}   commit table + files | diff, shared by History and Search
  workspace/history_view.{hpp,cpp}, workspace/file_status_view.{hpp,cpp},
  workspace/search_view.{hpp,cpp}, workspace/diff_placeholder.{hpp,cpp}
```

`src/app/main.cpp` creates the window (1280 × 800, `min_size` 900 × 560) and calls
`MainWindow::build`. Tests go to `tests/ui/` as one executable `test_ui`, linked to `gitrose_ui`
and `hixxy::testing` (hixxy's headless `UiTest` harness, built but outside the umbrella target).
`tests/smoke_test.cpp` and `test_smoke` are removed.

*Alternative:* everything in `src/app/`. Rejected: views need tests, and the app target is an
executable.

### Sample data as the future model's shape

`sample_repository.hpp` declares `RepositorySample` with plain fields: `name`, `branches` (name,
`current`), `tags`, `remotes`, `stashes`, `worktrees` (name, path, branch, `current`),
`submodules`, `commits` (graph placeholder, description, date, author, short hash) and per-commit
changed files (path, change kind), plus `staged` and `unstaged` files. `samples()` returns the
`gitrose` and `hixxy` samples. Views take a `const RepositorySample&` and never see where it came
from; the follow-up that reads a real repository replaces `samples()` with the `git` module's
data and deletes `sample/`.

### Splitter: a custom widget with pure geometry

`Splitter` derives from `hixxy::Widget`, holds exactly two children (`first<T>(...)`,
`second<T>(...)` add them) and has `Orientation`, nested `Splitter::Resize { KeepFirst,
KeepSecond, KeepRatio }`, per-pane minimums and an initial position (length or ratio).

The geometry lives in a static, `constexpr` member `Splitter::layout(Geometry) -> Panes`:
given the total length, divider thickness, minimums, resize mode and the stored value (kept
length or ratio), it returns the first pane's length, the divider's offset and the second pane's
length. The rules are the spec's: the panes and the divider fill the length; the kept pane gives
way to the other's minimum; below both minimums the first keeps its minimum. A drag converts the
pointer position into a new stored value through the same clamp, so dragging and resizing can
never disagree. The tests cover this function directly.

The divider is drawn as a hairline in `--border-color` (`--border-width` thick); its grab area
is the line plus 3 points on each side, overlapping the panes' edges. `mouse_down_event` on the
grab area returns `true` (capture until release); `cursor()` is `ResizeHorizontal` or
`ResizeVertical` over the grab area and while dragging.

*Alternatives:* a `SplitView` in hixxy first (the better long-term home, but blocks this change
on a hixxy session); fixed sizes now (rejected by the user). If hixxy gains a split view later,
`Splitter` is replaced and its tests show the behaviour to keep.

Splitters used (initial position, resize mode):

| Where | Orientation | Resize | Initial | Minimums |
|---|---|---|---|---|
| Page: sidebar \| workspace | side by side | KeepFirst | 240 | 180 / 480 |
| Commit browser: table / details | stacked | KeepRatio | 0.55 | 120 / 160 |
| Commit browser details: files \| diff | side by side | KeepFirst | 300 | 180 / 240 |
| File Status: lists+diff / commit area | stacked | KeepSecond | 150 | 200 / 110 |
| File Status: lists \| diff | side by side | KeepFirst | 340 | 200 / 240 |
| File Status lists: staged / unstaged | stacked | KeepRatio | 0.5 | 80 / 80 |

### Toolbar

`ToolbarView` is a `Row` above the `TabView`, outside it, as in Sourcetree: one toolbar for the
window, acting on the current tab. Each item is a `ToolbarButton`: a small widget with a lucide
icon above a caption, hover and pressed looks from the style sheet, and a `clicked` signal that
nothing connects to yet. Groups are separated by a fixed gap; a flexible `Spacer` pushes Show in
Finder, Terminal and Settings to the right. Icons (lucide; exact names checked when building):
Commit `GitCommitHorizontal`, Pull `ArrowDownToLine`, Push `ArrowUpFromLine`, Fetch
`RefreshCw`, Branch `GitBranchPlus`, Merge `GitMerge`, Stash `Archive`, Discard `Undo2`, Tag
`Tag`, Show in Finder `Folder`, Terminal `SquareTerminal`, Settings `Settings`.

*Alternative:* hixxy `Button` with `ButtonVariant::Subtle`. Rejected: its icon sits left of the
text on one line, not Sourcetree's icon-above-caption.

### Repository tabs and pages

The window content is a `Column`: the toolbar, then a `TabView` (flex) with one
`RepositoryPage` per sample. Each page owns its sidebar, its `Stack` of the three workspace
views (only the selected one visible) and its selection, so tabs keep their own view.

### Sidebar

A `ListView` over `SidebarModel`, a flat list of rows: a section heading (uppercase caption,
secondary tone) or an entry (icon, name, optional detail such as a worktree's branch, a
`current` mark drawn in the strong text weight with a leading dot). Section order and entries
come from the spec and the sample. Selecting a WORKSPACE entry asks the page to show its view
(`selection_changed`, since the selection is the view's state, not a model the list mirrors).
Headings are not selectable: when one gets selected, the sidebar restores the previous
selection. The list virtualizes, so repositories with thousands of branches or tags need no
change later.

*Alternative:* a `Column` of row widgets in a `ScrollView`. Simpler, but not virtualized.

### Workspace views

- **CommitBrowser** (History and Search): a stacked `Splitter` with a `TableView` on top
  (columns Graph `fixed(80)`, Description flex, Date `fixed(160)`, Author `fraction(0.15)`,
  Commit `fixed(96)`; the Graph cells show a placeholder dot until the graph change) and, below,
  a side-by-side `Splitter` of the changed-files `ListView` and a `DiffPlaceholder`. The first
  commit is selected; its files fill the list.
- **HistoryView**: a filter bar (`ComboBox` "All Branches", `CheckBox` "Show Remote Branches",
  `ComboBox` "Date Order") above a `CommitBrowser`.
- **SearchView**: a search bar (`TextField` with a placeholder, flex; `ComboBox` "Commit
  Messages" / "File Changes" / "Authors") above a `CommitBrowser`.
- **FileStatusView**: a top bar (`ComboBox` "Pending files, sorted by path"), then the splitters
  in the table above: Staged files and Unstaged files lists (each a heading and a `ListView` with
  the file's change icon) beside the `DiffPlaceholder`, and the commit area (`TextArea` with a
  placeholder, flex; a primary Commit button).
- **DiffPlaceholder**: a `Panel` with a centred secondary label ("Select a file to see its
  changes").

### Styling

`style.cpp` adds one gitrose style sheet to the application (as looprail's `ui/style.cpp` does)
with rules only for gitrose's own style types and classes (`ToolbarButton`, `.sidebar-heading`,
`.sidebar-current`, the divider), written with hixxy's design tokens so light and dark schemes
both work.

### No architecture document yet

gitrose has no `docs/architecture.md` or `docs/decisions/`. This change adds none: the Splitter's
contract is in its header's Doxygen and the layout in the spec. The first change that needs a
cross-module rule starts `docs/architecture.md`.

## Risks / Trade-offs

- [`TabView` has no close button or "+" control] → Repository tabs stay fixed in this change; a
  backlog item (NEXT section) records that tab management needs hixxy support or a gitrose tab
  strip, decided when that change comes.
- [`ListView` has no per-row "not selectable" hook] → The sidebar restores the previous
  selection when a heading is picked; keyboard navigation may briefly land on a heading. If that
  reads badly, a hixxy change can add the hook later.
- [A gitrose `Splitter` may duplicate a future hixxy split view] → Its interface is small and
  its geometry tests describe the behaviour, so swapping it is cheap.
- [Sample data could outlive its purpose] → It lives only in `src/ui/sample/`, and follow-up 2
  in the proposal deletes it.
- [Twelve toolbar items at the 900-point minimum width] → Item width is about 64 points, so the
  row needs about 830 points with gaps; the minimum width is chosen to fit it.
