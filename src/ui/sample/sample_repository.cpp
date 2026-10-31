// The sample repositories: fixed content for the layout until the git module reads real ones.

#include "ui/sample/sample_repository.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <format>
#include <iterator>
#include <span>
#include <string_view>

using namespace gitrose::ui;

namespace {

using Change = RepositorySample::ChangedFile::Change;

/// A file a sample commit may change.
struct SampleFile {
    std::string_view path;
    Change change;
};

/// The item at @p index of @p items (asserted to be in range).
template<class T>
auto item_at(const std::span<const T> items, const std::size_t index) -> const T& {
    assert(index < items.size());
    return *std::next(items.begin(), static_cast<std::ptrdiff_t>(index));
}

/// Builds a history from summary lines (newest first): each commit gets a date an hour and a
/// quarter before the previous one, an author from @p authors, a hash derived from its index and
/// one to three files from @p files.
auto make_commits(
    const std::span<const std::string_view> descriptions,
    const std::span<const std::string_view> authors,
    const std::span<const SampleFile> files,
    const unsigned seed
) -> std::vector<RepositorySample::Commit> {
    auto commits = std::vector<RepositorySample::Commit> {};
    commits.reserve(descriptions.size());

    constexpr auto start_minutes = (31 * 24 * 60) + (17 * 60) + 40; // October 31st, 17:40
    for (std::size_t index = 0; index < descriptions.size(); ++index) {
        const auto minutes = start_minutes - (static_cast<int>(index) * 75);
        auto commit = RepositorySample::Commit {
            .description = std::string { item_at(descriptions, index) },
            .date = std::format("2026-10-{:02} {:02}:{:02}", minutes / (24 * 60), (minutes / 60) % 24, minutes % 60),
            .author = std::string { item_at(authors, index % authors.size()) },
            .hash = std::format("{:07x}", (((index + 1) * 2'654'435'761U) ^ seed) & 0xFFF'FFFFU),
            .files = {},
        };

        const auto count = 1 + (index % 3);
        for (std::size_t file = 0; file < count; ++file) {
            const auto& sample = item_at(files, ((index * 2) + file) % files.size());
            commit.files.push_back({ .path = std::string { sample.path }, .change = sample.change });
        }

        commits.push_back(std::move(commit));
    }

    return commits;
}

auto make_gitrose() -> RepositorySample {
    static constexpr auto descriptions = std::to_array<std::string_view>({
        "Add the main window layout",
        "Splitter: keep the first pane's size on resize",
        "Sidebar: mark the current worktree",
        "Toolbar: icons above captions",
        "History: commit table columns",
        "File Status: staged and unstaged lists",
        "Search view with mode choice",
        "Sample repositories for the layout",
        "Style sheet for gitrose's widgets",
        "Repository tabs under the toolbar",
        "Project setup",
        "Add OpenSpec",
        "Document the backlog sections",
        "Pin hixxy to its latest commit",
        "CMake presets for ASan and TSan",
        "clang-tidy configuration",
        "Format hook for changed files",
        "Code style imported from hixxy",
        "README: build instructions",
        "Doctest runner",
        "Warnings as errors in dev builds",
        "ccache for every preset",
        "Hardening in release builds",
        "Ignore build directories",
        "Editor configuration",
        "Git attributes for line endings",
        "Initial CMake project",
        "Application window with a label",
        "Choose a name",
        "first commit",
    });
    static constexpr auto authors = std::to_array<std::string_view>({ "Albert Varaksin", "Claude" });
    static constexpr auto files = std::to_array<SampleFile>({
        { "src/ui/main_window.cpp", Change::Added },
        { "src/ui/widgets/splitter.hpp", Change::Modified },
        { "src/ui/sidebar/sidebar_view.cpp", Change::Modified },
        { "CMakeLists.txt", Change::Modified },
        { "docs/backlog.md", Change::Modified },
        { "tests/smoke_test.cpp", Change::Deleted },
        { "src/app/main.cpp", Change::Modified },
        { "docs/code-style.md", Change::Renamed },
    });

    return {
        .name = "gitrose",
        .branches = {
            { .name = "main", .current = true },
            { .name = "feature/git-cli", .current = false },
            { .name = "feature/tabs", .current = false },
            { .name = "fix/splitter-cursor", .current = false },
        },
        .tags = { "v0.1.0", "v0.0.1" },
        .remotes = { "origin" },
        .stashes = { "WIP on main: sidebar spacing", "On feature/tabs: close button" },
        .worktrees = {
            { .name = "gitrose", .path = "~/Developer/gitrose", .branch = "main", .current = true },
            { .name = "gitrose-git-cli", .path = "~/Developer/gitrose-git-cli", .branch = "feature/git-cli", .current = false },
            { .name = "gitrose-tabs", .path = "~/Developer/gitrose-tabs", .branch = "feature/tabs", .current = false },
        },
        .submodules = { "external/lucide" },
        .commits = make_commits(descriptions, authors, files, 0x5EED'01U),
        .staged = {
            { .path = "src/ui/main_window.cpp", .change = Change::Added },
            { .path = "src/ui/style.cpp", .change = Change::Modified },
        },
        .unstaged = {
            { .path = "README.md", .change = Change::Modified },
            { .path = "docs/backlog/01-first-need.md", .change = Change::Modified },
            { .path = "tests/smoke_test.cpp", .change = Change::Deleted },
            { .path = "src/ui/sidebar/notes.txt", .change = Change::Added },
        },
    };
}

auto make_hixxy() -> RepositorySample {
    static constexpr auto descriptions = std::to_array<std::string_view>({
        "ui: MenuItem::hide_mark() drops a checkable action's mark",
        "widgets: TableView fits a column on double click",
        "style: over() blends state layers",
        "ListView: pinned rows follow their index",
        "TabView: animate the indicator",
        "gfx: snap hairlines to device pixels",
        "testing: UiTest verifies the paint cache",
        "platform: headless virtual clock",
        "widgets: ComboBox popup matches the anchor width",
        "text: shape fallback fonts lazily",
        "anim: spring settles in reduced motion",
        "docs: custom widgets guide",
        "ui: deferred destruction of removed widgets",
        "widgets: TextArea placeholder",
        "icons: Lucide 1.57.0",
        "style: :nth-child for virtualized rows",
        "sdl: cursor shapes",
        "core: String small buffer",
        "widgets: Stack aligns children",
        "ui: focus traversal skips hidden subtrees",
        "gfx: rounded rectangle tessellation",
        "widgets: Slider keyboard steps",
        "docs: architecture overview",
        "ui: command presenters",
        "platform: macOS colour scheme",
        "widgets: Panel and Card",
        "text: baseline alignment in rows",
        "core: Signal with lifetime contexts",
        "Build: presets for sanitizers",
        "Initial import",
    });
    static constexpr auto authors = std::to_array<std::string_view>({ "Albert Varaksin" });
    static constexpr auto files = std::to_array<SampleFile>({
        { "include/hixxy/widgets/table_view.hpp", Change::Modified },
        { "src/widgets/table_view.cpp", Change::Modified },
        { "src/ui/widget.cpp", Change::Modified },
        { "tests/widgets/table_view_test.cpp", Change::Added },
        { "docs/guides/custom-widgets.md", Change::Modified },
        { "src/widgets/old_splitter.cpp", Change::Deleted },
    });

    return {
        .name = "hixxy",
        .branches = {
            { .name = "main", .current = true },
            { .name = "split-view", .current = false },
            { .name = "tree-view", .current = false },
        },
        .tags = { "v0.3.0", "v0.2.0", "v0.1.0" },
        .remotes = { "origin", "upstream" },
        .stashes = { "WIP on main: tab close button" },
        .worktrees = {
            { .name = "hixxy", .path = "~/Developer/hixxy", .branch = "main", .current = true },
            { .name = "hixxy-split-view", .path = "~/Developer/hixxy-split-view", .branch = "split-view", .current = false },
            { .name = "hixxy-tree-view", .path = "~/Developer/hixxy-tree-view", .branch = "tree-view", .current = false },
        },
        .submodules = { "external/sdl", "external/harfbuzz" },
        .commits = make_commits(descriptions, authors, files, 0x41B'E7U),
        .staged = {
            { .path = "include/hixxy/widgets/tab_view.hpp", .change = Change::Modified },
        },
        .unstaged = {
            { .path = "src/widgets/tab_view.cpp", .change = Change::Modified },
            { .path = "tests/widgets/tab_view_test.cpp", .change = Change::Modified },
        },
    };
}

} // namespace

auto RepositorySample::samples() -> const std::vector<RepositorySample>& {
    static const auto all = std::vector<RepositorySample> { make_gitrose(), make_hixxy() };
    return all;
}
