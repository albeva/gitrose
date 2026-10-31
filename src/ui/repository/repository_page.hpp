#pragma once

#include <array>

#include <hixxy/ui/widget.hpp>

#include "ui/sample/sample_repository.hpp"
#include "ui/sidebar/sidebar_model.hpp"
#include "ui/widgets/splitter.hpp"

namespace gitrose::ui {

class SidebarView;

/// One repository tab's page: the sidebar beside the workspace views, split by a divider that
/// keeps the sidebar's width when the window is resized. The views (File Status, History,
/// Search) overlap in a Stack, only the selected one visible; the sidebar's WORKSPACE entries
/// switch between them. History is shown at first. The page keeps its own selection, so each tab
/// keeps its view.
class RepositoryPage final : public Splitter {
public:
    using Workspace = SidebarModel::Workspace;

    /// The sidebar's initial width, in points.
    static constexpr auto sidebar_width = 240.0F;

    /// The page of a repository.
    /// @param repository The repository (must outlive the page).
    explicit RepositoryPage(const RepositorySample& repository);

    /// The sidebar.
    /// @return The sidebar.
    [[nodiscard]] auto sidebar() const noexcept -> SidebarView& { return *m_sidebar; }

    /// The view shown in the content area.
    /// @return The view.
    [[nodiscard]] auto workspace() const noexcept -> Workspace { return m_workspace; }

    /// A workspace view's widget.
    /// @param workspace The view.
    /// @return The widget.
    [[nodiscard]] auto view(Workspace workspace) const -> hixxy::Widget&;

private:
    /// Shows a workspace view and hides the others.
    /// @param workspace The view.
    auto show(Workspace workspace) -> void;

    SidebarView* m_sidebar = nullptr;
    std::array<hixxy::Widget*, 3> m_views {}; ///< By Workspace.
    Workspace m_workspace = Workspace::History;
};

} // namespace gitrose::ui
