#pragma once

#include <hixxy/core/geometry.hpp>
#include <hixxy/ui/window.hpp>
#include <hixxy/widgets/flex.hpp>
#include <hixxy/widgets/tab_view.hpp>

namespace gitrose::ui {

/// The main window's content: the toolbar (ToolbarView) above the repository tabs (a TabView named
/// `repository-tabs`), one RepositoryPage per open repository, titled with its folder name. In this version
/// the tabs show the sample repositories (RepositorySample::samples()), the first one current.
class MainWindow final : public hixxy::Column {
public:
    /// The size of the window's content area when gitrose starts, in points.
    static constexpr auto initial_size = hixxy::Size { 1280.0F, 800.0F };

    /// The smallest content area the window can be resized to, in points.
    static constexpr auto minimum_size = hixxy::Size { 900.0F, 560.0F };

    /// Builds the content.
    MainWindow();

    /// Builds the content and sets it as @p window's content.
    /// @param window The window.
    /// @return The content.
    static auto build(hixxy::Window& window) -> MainWindow&;

    /// The repository tabs.
    /// @return The tab view.
    [[nodiscard]] auto tabs() const noexcept -> hixxy::TabView& { return *m_tabs; }

private:
    hixxy::TabView* m_tabs = nullptr; ///< The repository tabs (a child).
};

} // namespace gitrose::ui
