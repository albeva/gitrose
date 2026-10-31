#include "ui/workspace/search_view.hpp"

#include <vector>

#include <hixxy/hixxy.hpp>

#include "ui/workspace/commit_browser.hpp"

using namespace hixxy;
using namespace gitrose::ui;

SearchView::SearchView(const RepositorySample& repository)
: m_browser(&add_content(repository)) {
    spacing(0.0F);
}

auto SearchView::add_content(const RepositorySample& repository) -> CommitBrowser& {
    auto& bar = add<Row>().add_class("view-bar").cross_align(Align::Center);
    bar.add<TextField>().placeholder("Search").name("search-field").flex(1.0F);
    bar.add<ComboBox>(std::vector<String> { "Commit Messages", "File Changes", "Authors" }, 0).fixed_width(190.0F);

    add<Separator>();
    return add<CommitBrowser>(repository).flex(1.0F);
}
