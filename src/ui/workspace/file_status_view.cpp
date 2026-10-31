#include "ui/workspace/file_status_view.hpp"

#include <memory>
#include <vector>

#include <hixxy/hixxy.hpp>

#include "ui/widgets/splitter.hpp"
#include "ui/workspace/diff_placeholder.hpp"
#include "ui/workspace/file_list.hpp"

using namespace hixxy;
using namespace gitrose::ui;

namespace {

/// A heading above a list of files.
auto add_files(
    Column& pane,
    const StringView heading,
    const StringView name,
    std::vector<RepositorySample::ChangedFile> files
) -> void {
    pane.spacing(0.0F);
    pane.add<Label>(String { heading }, TextRole::Caption).add_class("file-list-heading");

    auto model = std::make_shared<FileListModel>();
    model->set_files(std::move(files));
    pane.add<FileListView>(std::move(model)).name(String { name }).flex(1.0F);
}

} // namespace

FileStatusView::FileStatusView(const RepositorySample& repository) {
    spacing(0.0F);

    auto& bar = add<Row>().add_class("view-bar").cross_align(Align::Center);
    bar.add<ComboBox>(std::vector<String> { "Pending files, sorted by path", "All files, sorted by path" }, 0).fixed_width(240.0F);

    add<Separator>();

    auto& main = add<Splitter>(Splitter::Options {
        .orientation = Orientation::Vertical,
        .resize = Splitter::Resize::KeepSecond,
        .position = 150.0F,
        .first_minimum = 200.0F,
        .second_minimum = 110.0F,
    });
    main.flex(1.0F);

    auto& files = main.first<Splitter>(Splitter::Options {
        .orientation = Orientation::Horizontal,
        .resize = Splitter::Resize::KeepFirst,
        .position = 340.0F,
        .first_minimum = 200.0F,
        .second_minimum = 240.0F,
    });

    auto& lists = files.first<Splitter>(Splitter::Options {
        .orientation = Orientation::Vertical,
        .resize = Splitter::Resize::KeepRatio,
        .position = 0.5F,
        .first_minimum = 80.0F,
        .second_minimum = 80.0F,
    });
    add_files(lists.first<Column>(), "Staged files", "staged-files", repository.staged);
    add_files(lists.second<Column>(), "Unstaged files", "unstaged-files", repository.unstaged);
    files.second<DiffPlaceholder>();

    auto& commit = main.second<Column>().add_class("commit-area");
    commit.add<TextArea>().placeholder("Commit message").name("commit-message").flex(1.0F);
    auto& actions = commit.add<Row>();
    actions.add<Spacer>();
    actions.add<Button>("Commit", ButtonVariant::Primary).name("commit-button");
}
