#include "ui/workspace/file_list.hpp"

#include <hixxy/gfx/icons/lucide.hpp>
#include <hixxy/hixxy.hpp>

using namespace hixxy;
using namespace gitrose::ui;

namespace {

using Change = FileListModel::ChangedFile::Change;

/// The widget of one file row.
class FileRow final : public Row {
public:
    FileRow()
    : m_icon(&add<IconView>(lucide::FilePen))
    , m_path(&add<Label>("").flex(1.0F)) {
        cross_align(Align::Center);
        add_class("file-row");
    }

    auto bind(const FileListModel::ChangedFile& file) -> void {
        m_icon->icon(FileListModel::icon(file.change)).tone(FileListModel::tone(file.change));
        m_path->text(String { StringView { file.path } });
    }

private:
    IconView* m_icon;
    Label* m_path;
};

} // namespace

auto FileListModel::text(const std::size_t row) const -> String {
    return row < m_files.size() ? String { StringView { m_files.at(row).path } } : String {};
}

auto FileListModel::set_files(std::vector<ChangedFile> files) -> void {
    m_files = std::move(files);
    rows_reset.emit();
}

auto FileListModel::icon(const ChangedFile::Change change) noexcept -> Icon {
    switch (change) {
    case Change::Added:
        return lucide::FilePlus;
    case Change::Modified:
        return lucide::FilePen;
    case Change::Deleted:
        return lucide::FileMinus;
    case Change::Renamed:
        return lucide::FileSymlink;
    }

    return lucide::File;
}

auto FileListModel::tone(const ChangedFile::Change change) noexcept -> Tone {
    switch (change) {
    case Change::Added:
        return Tone::Positive;
    case Change::Modified:
        return Tone::Warning;
    case Change::Deleted:
        return Tone::Danger;
    case Change::Renamed:
        return Tone::Accent;
    }

    return Tone::Secondary;
}

FileListView::FileListView(std::shared_ptr<FileListModel> model)
: ListView(model)
, m_files(std::move(model)) {
    add_class("file-list");
    delegate(
        [] { return std::make_unique<FileRow>(); },
        [files = m_files](FileRow& row, const std::size_t index) { row.bind(files->at(index)); }
    );
}
