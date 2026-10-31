# Proposal

## Why

gitrose's window shows a single "gitrose" label. Before any git work, the app needs its main
window layout, so later changes have a place to show repository data. The layout follows
Sourcetree on macOS, which is simple and clean but lacks proper worktree support; adding that
support is why this project exists. This change builds only the layout, filled with static sample
data: nothing reads a repository yet.

## What Changes

- A new `ui` module (static library `gitrose_ui`) holds the window's views, and `src/app/main.cpp`
  shows its main window instead of the label.
- **Toolbar** across the top, in Sourcetree's order: Commit, Pull, Push, Fetch | Branch, Merge |
  Stash, Discard, Tag | (flexible space) | Show in Finder, Terminal, Settings. Each item is an
  icon above a caption. It acts on the current tab's repository; a click does nothing yet.
- **Repository tabs** under the toolbar, one per open repository, as macOS window tabs sit under
  Sourcetree's toolbar. Two sample tabs are shown, each with its own sidebar and views; opening,
  closing and adding tabs come later.
- **Sidebar** on the left, in sections: WORKSPACE (File Status, History, Search), BRANCHES, TAGS,
  REMOTES, STASHES, **WORKTREES** (new compared to Sourcetree, with the current worktree marked)
  and SUBMODULES, filled with sample entries.
- **Workspace views** in the content area, switched by the WORKSPACE entries in the sidebar (the
  only interaction besides resizing):
  - **History**: a filter bar, a commit table (Graph, Description, Date, Author, Commit) and,
    under it, the selected commit's files beside the diff area.
  - **File Status**: staged and unstaged file lists beside the diff area, with the commit message
    area below them.
  - **Search**: a search bar with a mode choice above History's table and detail panes.
- **Resizable panes**: a gitrose `Splitter` widget with a draggable divider separates the sidebar
  from the content, and the panes inside each view. hixxy has no split view widget, and the user
  chose to build this one in gitrose.

Out of scope, each for later changes:
- Any git access: the `git` module (git CLI behind a typed API, discussed before this change),
  reading branches, tags, worktrees, history, status and diffs.
- Toolbar actions, sidebar context menus, opening repositories, the repository browser, closing
  and adding tabs.
- The commit graph drawing (the Graph column shows a placeholder), diff rendering and syntax
  highlighting.
- Saving pane sizes, the selected view and open tabs between runs.
- Collapsing sidebar sections, and the menu bar.

Follow-up changes, in order:
1. `add-git-cli-layer`: the `git` module (process runner, `Repository`, status/log/diff parsers).
2. Fill the sidebar and History from a real repository opened from the command line.
3. File Status from `git status`, with staging and the diff view.
4. Worktree actions (add, switch, remove) from the WORKTREES section.
5. Repository tabs: open, close, add; save window state.

Depends on: hixxy as pinned today (TabView, TableView, ListView, Panel, lucide icons, custom
widget support). No hixxy change is needed. No backlog item is taken on.

## Capabilities

### New Capabilities
- `ui/main-window`: the main window's structure: toolbar, repository tabs, sidebar sections,
  the workspace views and how the sidebar switches between them.
- `ui/split-panes`: two panes with a draggable divider: sizes, limits and how they react to
  window resizing.

### Modified Capabilities

None.

## Impact

- New: `src/ui/` (library `gitrose_ui`, linked to hixxy), `tests/ui/` (test `test_ui`).
- Changed: `src/CMakeLists.txt`, `src/app/CMakeLists.txt`, `src/app/main.cpp`,
  `tests/CMakeLists.txt`; `tests/smoke_test.cpp` is removed once `test_ui` exists.
- No new dependencies.
