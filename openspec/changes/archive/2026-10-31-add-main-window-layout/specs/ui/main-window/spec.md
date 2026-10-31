# Spec Delta

## Purpose

The main window's structure, modelled on Sourcetree for macOS: the toolbar, a tab per open
repository, the sidebar of workspace views and repository objects, and the workspace views.

## ADDED Requirements

### Requirement: Window structure
The main window SHALL show, from top to bottom, the toolbar and the repository tabs; the current
tab's page SHALL fill the rest, with the sidebar on the left and the selected workspace view on
the right, separated by a split pane divider. The window SHALL open at 1280 × 800 points and
SHALL NOT be resizable below 900 × 560 points.

#### Scenario: Launch
- **WHEN** gitrose starts
- **THEN** the window shows the toolbar at the top, the repository tabs under it, and the first
  tab's sidebar and History view below them

#### Scenario: Minimum size
- **WHEN** the user drags the window's edge to make it smaller than 900 × 560 points
- **THEN** the window stops at 900 × 560 points

### Requirement: Toolbar
The toolbar SHALL show these items in this order, each an icon above its caption: Commit, Pull,
Push, Fetch; Branch, Merge; Stash, Discard, Tag; then a flexible space; then Show in Finder,
Terminal, Settings. The groups SHALL be visibly apart. Clicking an item SHALL have no effect in
this version.

#### Scenario: Toolbar items
- **WHEN** the window is shown
- **THEN** the toolbar shows the twelve items in the listed order, with Show in Finder, Terminal
  and Settings at the right edge

#### Scenario: Wide and narrow window
- **WHEN** the window is made wider
- **THEN** the space before Show in Finder grows and every item keeps its size

### Requirement: Repository tabs
The window SHALL show one tab per open repository, titled with the repository's folder name. Each
tab SHALL have its own page with its own sidebar selection and workspace view. In this version
the tabs SHALL be two sample repositories, `gitrose` and `hixxy`, and the first SHALL be current
at launch.

#### Scenario: Switch tabs
- **WHEN** the user clicks the `hixxy` tab
- **THEN** the page below shows the `hixxy` sample's sidebar and its selected workspace view

#### Scenario: Each tab keeps its view
- **WHEN** the user selects File Status in the `gitrose` tab, switches to the `hixxy` tab and back
- **THEN** the `hixxy` tab shows History and the `gitrose` tab still shows File Status

### Requirement: Sidebar sections
The sidebar SHALL list these sections in this order, each a heading with its entries indented
under it: WORKSPACE (File Status, History, Search), BRANCHES, TAGS, REMOTES, STASHES, WORKTREES
and SUBMODULES. The sidebar SHALL scroll vertically when its entries do not fit.

#### Scenario: Section order
- **WHEN** the window is shown
- **THEN** the sidebar shows WORKSPACE, BRANCHES, TAGS, REMOTES, STASHES, WORKTREES and
  SUBMODULES, in that order

#### Scenario: Many entries
- **WHEN** the window is too short for every sidebar entry
- **THEN** the sidebar scrolls and the toolbar and tabs stay in place

### Requirement: Current branch and worktree marks
The sidebar SHALL mark the checked-out branch under BRANCHES and the worktree the tab shows under
WORKTREES, so that each stands out from its section's other entries. Each worktree entry SHALL
show its branch beside its name.

#### Scenario: Current branch
- **WHEN** the `gitrose` sample tab is shown
- **THEN** its checked-out branch `main` is marked and the other branches are not

#### Scenario: Current worktree
- **WHEN** the `gitrose` sample tab is shown
- **THEN** WORKTREES lists the main worktree, marked, and the sample's linked worktrees unmarked,
  each with its branch

### Requirement: Workspace view switching
Selecting File Status, History or Search under WORKSPACE SHALL show that view in the content
area. History SHALL be selected when a tab is first shown. Selecting an entry in another section
SHALL NOT change the workspace view.

#### Scenario: Show File Status
- **WHEN** the user clicks File Status in the sidebar
- **THEN** the content area shows the File Status view and File Status is highlighted

#### Scenario: Other entries keep the view
- **WHEN** History is shown and the user clicks a branch in the sidebar
- **THEN** the content area still shows History

### Requirement: History view
The History view SHALL show a filter bar (branch filter, ordering choice) above the commit table
with the columns Graph, Description, Date, Author and Commit. Under the table, split by a
divider, it SHALL show the selected commit's changed files beside the diff area.

#### Scenario: History layout
- **WHEN** the History view is shown
- **THEN** the commit table fills the upper part with sample commits, and the file list and the
  diff area share the lower part side by side

### Requirement: File Status view
The File Status view SHALL show the staged files above the unstaged files on the left, each list
with its heading, the diff area on the right, and the commit message area along the bottom with
a multi-line message field and a Commit button.

#### Scenario: File Status layout
- **WHEN** the File Status view is shown
- **THEN** the Staged files and Unstaged files lists are on the left, the diff area on the right
  and the commit message area below both

### Requirement: Search view
The Search view SHALL show a search bar (a search field and a choice of what to search: commit
messages, file changes, authors) above the same commit table and detail panes as History.

#### Scenario: Search layout
- **WHEN** the Search view is shown
- **THEN** the search bar is at the top and the commit table, file list and diff area are below
  it as in History

### Requirement: Static sample content
In this version every list, table and pane SHALL show fixed sample content, and the window SHALL
NOT read any repository or run git. Panes without sample content, such as the diff area, SHALL
show a placeholder message.

#### Scenario: Independent of the working directory
- **WHEN** gitrose is started from any directory, in a repository or not
- **THEN** it shows the same sample content
