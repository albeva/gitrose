#include "ui/workspace/history_view.hpp"

#include <vector>

#include <hixxy/hixxy.hpp>

#include "ui/workspace/commit_browser.hpp"

using namespace hixxy;
using namespace gitrose::ui;

HistoryView::HistoryView(const RepositorySample& repository)
: m_browser(&add_content(repository)) {
    spacing(0.0F);
}

auto HistoryView::add_content(const RepositorySample& repository) -> CommitBrowser& {
    auto& bar = add<Row>().add_class("view-bar").cross_align(Align::Center);
    bar.add<ComboBox>(std::vector<String> { "All Branches", "Current Branch" }, 0).fixed_width(160.0F);
    bar.add<CheckBox>("Show Remote Branches", true);
    bar.add<Spacer>();
    bar.add<ComboBox>(std::vector<String> { "Date Order", "Ancestor Order" }, 0).fixed_width(150.0F);

    add<Separator>();
    return add<CommitBrowser>(repository).flex(1.0F);
}
