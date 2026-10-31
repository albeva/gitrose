#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include <hixxy/core/string.hpp>
#include <hixxy/gfx/icon.hpp>
#include <hixxy/widgets/list_model.hpp>

#include "ui/sample/sample_repository.hpp"

namespace gitrose::ui {

/// The rows of a repository's sidebar, flat: each section's heading followed by its entries.
/// Sections, in order: WORKSPACE (File Status, History, Search), BRANCHES, TAGS, REMOTES, STASHES,
/// WORKTREES and SUBMODULES. The model does not change once built.
class SidebarModel final : public hixxy::ListModel {
public:
    /// A section of the sidebar.
    enum class Section : std::uint8_t {
        Workspace,  ///< The workspace views.
        Branches,   ///< Local branches.
        Tags,       ///< Tags.
        Remotes,    ///< Remotes.
        Stashes,    ///< Stashes.
        Worktrees,  ///< Worktrees.
        Submodules, ///< Submodules.
    };

    /// A workspace view, shown in the content area beside the sidebar.
    enum class Workspace : std::uint8_t {
        FileStatus, ///< Staged and unstaged files, and the commit message.
        History,    ///< The commit table and the selected commit's changes.
        Search,     ///< The commit table filtered by a search.
    };

    // NOLINTBEGIN(readability-redundant-member-init): keeps designated initializers warning-free

    /// One row: a section heading or an entry under it.
    struct Row {
        Section section = Section::Workspace;  ///< The section it belongs to.
        bool heading = false;                  ///< Whether it is the section's heading.
        hixxy::String name {};                 ///< The heading's or the entry's text.
        hixxy::String detail {};               ///< Shown beside the name (a worktree's branch).
        std::optional<hixxy::Icon> icon {};    ///< The entry's icon (none for headings).
        bool current = false;                  ///< The checked-out branch or the shown worktree.
        std::optional<Workspace> workspace {}; ///< The view a WORKSPACE entry shows.
    };

    // NOLINTEND(readability-redundant-member-init)

    /// The rows of a repository.
    /// @param repository The repository.
    explicit SidebarModel(const RepositorySample& repository);

    /// The number of rows.
    /// @return The count.
    [[nodiscard]] auto row_count() const -> std::size_t override { return m_rows.size(); }

    /// A row's name (type-ahead).
    /// @param row The row (out of range: empty).
    /// @return The name.
    [[nodiscard]] auto text(std::size_t row) const -> hixxy::String override;

    /// A row.
    /// @param row The row (below row_count()).
    /// @return The row.
    [[nodiscard]] auto at(std::size_t row) const -> const Row& { return m_rows.at(row); }

    /// The row of a workspace view's entry.
    /// @param workspace The view.
    /// @return The row.
    [[nodiscard]] auto row_of(Workspace workspace) const noexcept -> std::optional<std::size_t>;

    /// A section's heading text.
    /// @param section The section.
    /// @return The heading (`BRANCHES`).
    [[nodiscard]] static auto heading(Section section) noexcept -> hixxy::StringView;

private:
    /// Appends a section's heading.
    /// @param section The section.
    auto add_heading(Section section) -> void;

    /// Appends an entry.
    /// @param section The section it belongs to.
    /// @param name Its name.
    /// @param icon Its icon.
    /// @return The entry, to set its other fields.
    auto add_entry(Section section, hixxy::String name, const hixxy::Icon& icon) -> Row&;

    std::vector<Row> m_rows;
};

} // namespace gitrose::ui
