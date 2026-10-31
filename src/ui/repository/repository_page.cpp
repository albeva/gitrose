#include "ui/repository/repository_page.hpp"

#include <cstddef>
#include <memory>

#include <hixxy/hixxy.hpp>

#include "ui/sidebar/sidebar_view.hpp"
#include "ui/workspace/file_status_view.hpp"
#include "ui/workspace/history_view.hpp"
#include "ui/workspace/search_view.hpp"

using namespace hixxy;
using namespace gitrose::ui;

namespace {

constexpr auto slot_of(const RepositoryPage::Workspace workspace) noexcept -> std::size_t {
    return static_cast<std::size_t>(workspace);
}

} // namespace

RepositoryPage::RepositoryPage(const RepositorySample& repository)
: Splitter({
      .orientation = Orientation::Horizontal,
      .resize = Resize::KeepFirst,
      .position = sidebar_width,
      .first_minimum = 180.0F,
      .second_minimum = 480.0F,
  }) {
    m_sidebar = &first<SidebarView>(std::make_shared<SidebarModel>(repository));

    auto& views = second<Stack>();
    m_views.at(slot_of(Workspace::FileStatus)) = &views.add<FileStatusView>(repository);
    m_views.at(slot_of(Workspace::History)) = &views.add<HistoryView>(repository);
    m_views.at(slot_of(Workspace::Search)) = &views.add<SearchView>(repository);

    m_sidebar->workspace_selected.connect(*this, [this](const Workspace workspace) { show(workspace); });
    show(Workspace::History);
    m_sidebar->select_workspace(Workspace::History);
}

auto RepositoryPage::view(const Workspace workspace) const -> Widget& {
    return *m_views.at(slot_of(workspace));
}

auto RepositoryPage::show(const Workspace workspace) -> void {
    m_workspace = workspace;
    for (auto* const view : m_views) {
        view->visible(view == m_views.at(slot_of(workspace)));
    }
}
