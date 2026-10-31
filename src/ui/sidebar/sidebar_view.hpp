#pragma once

#include <cstddef>
#include <memory>
#include <optional>

#include <hixxy/core/signal.hpp>
#include <hixxy/widgets/list_view.hpp>

#include "ui/sidebar/sidebar_model.hpp"

namespace gitrose::ui {

/// A repository's sidebar (`ListView.sidebar`): the rows of a SidebarModel, headings as uppercase
/// captions (`.sidebar-heading`) and entries indented under them with their icon, name and detail.
/// The checked-out branch and the shown worktree are marked (`.sidebar-current`: a leading dot and
/// the strong text weight). Headings cannot be selected: picking one restores the previous
/// selection. Selecting a WORKSPACE entry emits `workspace_selected`.
class SidebarView final : public hixxy::ListView {
public:
    /// A sidebar of a repository's rows.
    /// @param model The rows.
    explicit SidebarView(std::shared_ptr<SidebarModel> model);

    /// The rows.
    /// @return The model.
    [[nodiscard]] auto sidebar_model() const noexcept -> const SidebarModel& { return *m_model; }

    /// The selected entry.
    /// @return Its row, or `nullptr` when nothing is selected.
    [[nodiscard]] auto selected_entry() const -> const SidebarModel::Row*;

    /// Selects a workspace view's entry (emits `workspace_selected`).
    /// @param workspace The view.
    auto select_workspace(SidebarModel::Workspace workspace) -> void;

    /// Emitted when a WORKSPACE entry becomes selected, with its view.
    hixxy::Signal<SidebarModel::Workspace> workspace_selected;

private:
    /// Keeps headings out of the selection and reports WORKSPACE entries.
    auto selection_moved() -> void;

    std::shared_ptr<SidebarModel> m_model;
    std::optional<std::size_t> m_selected; ///< The last selected entry.
};

} // namespace gitrose::ui
