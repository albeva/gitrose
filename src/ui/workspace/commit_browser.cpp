#include "ui/workspace/commit_browser.hpp"

#include <vector>

#include <hixxy/hixxy.hpp>

#include "ui/workspace/diff_placeholder.hpp"
#include "ui/workspace/file_list.hpp"

using namespace hixxy;
using namespace gitrose::ui;

namespace {

auto commit_columns() -> std::vector<TableColumn> {
    return {
        { .title = "Graph", .width = TableColumnWidth::fixed(80.0F), .sortable = false },
        { .title = "Description", .width = TableColumnWidth::fixed(240.0F), .min_width = 120.0F, .sortable = false },
        { .title = "Date", .width = TableColumnWidth::fixed(160.0F), .sortable = false },
        { .title = "Author", .width = TableColumnWidth::fraction(0.15F), .sortable = false },
        { .title = "Commit", .width = TableColumnWidth::fixed(96.0F), .sortable = false },
    };
}

} // namespace

CommitTableModel::CommitTableModel(const RepositorySample& repository) noexcept
: m_repository(repository) {}

auto CommitTableModel::cell_text(const std::size_t row, const std::size_t column) const -> String {
    if (row >= m_repository.commits.size() || column > static_cast<std::size_t>(Column::Hash)) {
        return {};
    }

    const auto& commit = m_repository.commits.at(row);
    switch (static_cast<Column>(column)) {
    case Column::Graph:
        return "●";
    case Column::Description:
        return String { StringView { commit.description } };
    case Column::Date:
        return String { StringView { commit.date } };
    case Column::Author:
        return String { StringView { commit.author } };
    case Column::Hash:
        return String { StringView { commit.hash } };
    }

    return {};
}

CommitBrowser::CommitBrowser(const RepositorySample& repository)
: Splitter({
      .orientation = Orientation::Vertical,
      .resize = Resize::KeepRatio,
      .position = 0.55F,
      .first_minimum = 120.0F,
      .second_minimum = 160.0F,
  })
, m_commits(std::make_shared<CommitTableModel>(repository)) {
    m_table = &first<TableView>(m_commits, commit_columns()).name("commit-table").flex_column(static_cast<std::size_t>(CommitTableModel::Column::Description));
    m_table->add_class("commit-table");

    auto& details = second<Splitter>(Options {
        .orientation = Orientation::Horizontal,
        .resize = Resize::KeepFirst,
        .position = 300.0F,
        .first_minimum = 180.0F,
        .second_minimum = 240.0F,
    });
    m_files = &details.first<FileListView>(std::make_shared<FileListModel>()).name("commit-files");
    details.second<DiffPlaceholder>();

    m_table->on_selection_change(*this, [this] { show_selected(); });
    if (m_commits->row_count() > 0) {
        m_table->select(0);
    }
}

auto CommitBrowser::show_selected() -> void {
    const auto row = m_table->selected_row();
    m_files->files().set_files(row ? m_commits->at(*row).files : std::vector<RepositorySample::ChangedFile> {});
}
