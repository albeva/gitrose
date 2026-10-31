#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <hixxy/core/string.hpp>
#include <hixxy/widgets/table_model.hpp>
#include <hixxy/widgets/table_view.hpp>

#include "ui/sample/sample_repository.hpp"
#include "ui/widgets/splitter.hpp"

namespace gitrose::ui {

class FileListView;

/// The commits of a history, for the commit table: model columns Graph, Description, Date, Author
/// and Commit.
class CommitTableModel final : public hixxy::TableModel {
public:
    /// The model columns.
    enum class Column : std::uint8_t {
        Graph,       ///< The commit graph (a placeholder dot until it is drawn).
        Description, ///< The summary line.
        Date,        ///< When it was committed.
        Author,      ///< Who wrote it.
        Hash,        ///< The abbreviated hash.
    };

    /// The commits of a repository.
    /// @param repository The repository (must outlive the model).
    explicit CommitTableModel(const RepositorySample& repository) noexcept;

    /// The number of commits.
    /// @return The count.
    [[nodiscard]] auto row_count() const -> std::size_t override { return m_repository.commits.size(); }

    /// A cell's text.
    /// @param row The commit (out of range: empty).
    /// @param column The model column (out of range: empty).
    /// @return The text.
    [[nodiscard]] auto cell_text(std::size_t row, std::size_t column) const -> hixxy::String override;

    /// A commit.
    /// @param row The row (below row_count()).
    /// @return The commit.
    [[nodiscard]] auto at(std::size_t row) const -> const RepositorySample::Commit& { return m_repository.commits.at(row); }

private:
    const RepositorySample& m_repository;
};

/// The commit table above the selected commit's changed files beside the diff area, shared by the
/// History and Search views. The table (named `commit-table`) and the details are split by a
/// stacked Splitter that keeps its ratio; the files (named `commit-files`) and the diff area by a
/// side-by-side one that keeps the files' width. The first commit is selected at first; selecting
/// a commit lists its files.
class CommitBrowser final : public Splitter {
public:
    /// A browser of a repository's history.
    /// @param repository The repository (must outlive the browser).
    explicit CommitBrowser(const RepositorySample& repository);

    /// The commit table.
    /// @return The table.
    [[nodiscard]] auto table() const noexcept -> hixxy::TableView& { return *m_table; }

    /// The selected commit's files.
    /// @return The list.
    [[nodiscard]] auto files() const noexcept -> FileListView& { return *m_files; }

private:
    /// Lists the selected commit's files.
    auto show_selected() -> void;

    std::shared_ptr<CommitTableModel> m_commits;
    hixxy::TableView* m_table = nullptr;
    FileListView* m_files = nullptr;
};

} // namespace gitrose::ui
