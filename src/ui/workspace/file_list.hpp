#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include <hixxy/core/string.hpp>
#include <hixxy/gfx/icon.hpp>
#include <hixxy/widgets/common.hpp>
#include <hixxy/widgets/list_model.hpp>
#include <hixxy/widgets/list_view.hpp>

#include "ui/sample/sample_repository.hpp"

namespace gitrose::ui {

/// Changed files, for a FileListView.
class FileListModel final : public hixxy::ListModel {
public:
    using ChangedFile = RepositorySample::ChangedFile;

    /// A model without files.
    FileListModel() = default;

    /// The number of files.
    /// @return The count.
    [[nodiscard]] auto row_count() const -> std::size_t override { return m_files.size(); }

    /// A file's path (type-ahead).
    /// @param row The row (out of range: empty).
    /// @return The path.
    [[nodiscard]] auto text(std::size_t row) const -> hixxy::String override;

    /// A file.
    /// @param row The row (below row_count()).
    /// @return The file.
    [[nodiscard]] auto at(std::size_t row) const -> const ChangedFile& { return m_files.at(row); }

    /// Replaces the files (emits `rows_reset`).
    /// @param files The files.
    auto set_files(std::vector<ChangedFile> files) -> void;

    /// The icon of a kind of change.
    /// @param change The change.
    /// @return The icon.
    [[nodiscard]] static auto icon(ChangedFile::Change change) noexcept -> hixxy::Icon;

    /// The tone of a kind of change's icon.
    /// @param change The change.
    /// @return The tone.
    [[nodiscard]] static auto tone(ChangedFile::Change change) noexcept -> hixxy::Tone;

private:
    std::vector<ChangedFile> m_files;
};

/// A list of changed files (`ListView.file-list`): each row the change's icon and the path.
class FileListView final : public hixxy::ListView {
public:
    /// A list showing a model's files.
    /// @param model The files.
    explicit FileListView(std::shared_ptr<FileListModel> model);

    /// The files.
    /// @return The model.
    [[nodiscard]] auto files() const noexcept -> FileListModel& { return *m_files; }

private:
    std::shared_ptr<FileListModel> m_files;
};

} // namespace gitrose::ui
