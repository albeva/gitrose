#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace gitrose::ui {

/// A repository's data as the views show it, filled with fixed sample content until the git
/// module reads real repositories. Plain data: no hixxy types.
struct RepositorySample {
    /// A local branch.
    struct Branch {
        std::string name;     ///< The branch name (`main`, `feature/tabs`).
        bool current = false; ///< Whether it is checked out in the worktree the tab shows.
    };

    /// A worktree of the repository.
    struct Worktree {
        std::string name;     ///< The worktree's folder name.
        std::string path;     ///< Where it is on disk.
        std::string branch;   ///< The branch checked out in it.
        bool current = false; ///< Whether it is the worktree the tab shows.
    };

    /// A file changed by a commit, or in the index or the working tree.
    struct ChangedFile {
        /// How the file changed.
        enum class Change : std::uint8_t {
            Added,    ///< A new file.
            Modified, ///< Changed contents.
            Deleted,  ///< Removed.
            Renamed,  ///< Moved, possibly with changes.
        };

        std::string path;                 ///< The path relative to the repository's root.
        Change change = Change::Modified; ///< How it changed.
    };

    /// A commit of the history.
    struct Commit {
        std::string description;        ///< The message's summary line.
        std::string date;               ///< When it was committed (`2026-10-31 14:05`).
        std::string author;             ///< Who wrote it.
        std::string hash;               ///< The abbreviated hash.
        std::vector<ChangedFile> files; ///< The files it changed.
    };

    std::string name;                    ///< The repository's folder name (the tab's title).
    std::vector<Branch> branches;        ///< Local branches.
    std::vector<std::string> tags;       ///< Tag names.
    std::vector<std::string> remotes;    ///< Remote names.
    std::vector<std::string> stashes;    ///< Stash descriptions, newest first.
    std::vector<Worktree> worktrees;     ///< Worktrees, the main one first.
    std::vector<std::string> submodules; ///< Submodule paths.
    std::vector<Commit> commits;         ///< The history, newest first.
    std::vector<ChangedFile> staged;     ///< Files in the index.
    std::vector<ChangedFile> unstaged;   ///< Changed files in the working tree.

    /// The sample repositories the window shows: `gitrose` and `hixxy`.
    /// @return The samples (static storage).
    [[nodiscard]] static auto samples() -> const std::vector<RepositorySample>&;
};

} // namespace gitrose::ui
