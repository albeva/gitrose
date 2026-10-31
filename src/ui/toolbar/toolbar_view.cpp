#include "ui/toolbar/toolbar_view.hpp"

#include <array>
#include <span>

#include <hixxy/gfx/icons/lucide.hpp>
#include <hixxy/hixxy.hpp>

#include "ui/widgets/toolbar_button.hpp"

using namespace hixxy;
using namespace gitrose::ui;

namespace {

/// One toolbar item.
struct Item {
    LucideIcon icon;
    StringView caption;
    StringView name;
};

constexpr auto remote_items = std::to_array<Item>({
    { .icon = lucide::GitCommitHorizontal, .caption = "Commit", .name = "toolbar-commit" },
    { .icon = lucide::ArrowDownToLine, .caption = "Pull", .name = "toolbar-pull" },
    { .icon = lucide::ArrowUpFromLine, .caption = "Push", .name = "toolbar-push" },
    { .icon = lucide::RefreshCw, .caption = "Fetch", .name = "toolbar-fetch" },
});

constexpr auto branch_items = std::to_array<Item>({
    { .icon = lucide::GitBranchPlus, .caption = "Branch", .name = "toolbar-branch" },
    { .icon = lucide::GitMerge, .caption = "Merge", .name = "toolbar-merge" },
});

constexpr auto change_items = std::to_array<Item>({
    { .icon = lucide::Archive, .caption = "Stash", .name = "toolbar-stash" },
    { .icon = lucide::Undo2, .caption = "Discard", .name = "toolbar-discard" },
    { .icon = lucide::Tag, .caption = "Tag", .name = "toolbar-tag" },
});

constexpr auto window_items = std::to_array<Item>({
    { .icon = lucide::Folder, .caption = "Show in Finder", .name = "toolbar-show-in-finder" },
    { .icon = lucide::SquareTerminal, .caption = "Terminal", .name = "toolbar-terminal" },
    { .icon = lucide::Settings, .caption = "Settings", .name = "toolbar-settings" },
});

auto add_items(Row& row, const std::span<const Item> items) -> void {
    for (const auto& item : items) {
        row.add<ToolbarButton>(item.icon, String { item.caption }).name(String { item.name });
    }
}

} // namespace

ToolbarView::ToolbarView() {
    add_class("toolbar");
    cross_align(Align::Center);

    add_items(*this, remote_items);
    add<Spacer>(group_gap);
    add_items(*this, branch_items);
    add<Spacer>(group_gap);
    add_items(*this, change_items);
    add<Spacer>().name("toolbar-flexible-space");
    add_items(*this, window_items);
}
