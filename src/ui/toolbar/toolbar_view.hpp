#pragma once

#include <hixxy/widgets/flex.hpp>

namespace gitrose::ui {

/// The window's toolbar (`Row.toolbar`), above the repository tabs; it acts on the current tab's
/// repository. Its items (ToolbarButton), in Sourcetree's order and groups: Commit, Pull, Push,
/// Fetch | Branch, Merge | Stash, Discard, Tag, then a flexible space and Show in Finder,
/// Terminal, Settings at the right edge. Each item is named after its caption
/// (`toolbar-show-in-finder`); clicking one does nothing yet.
class ToolbarView final : public hixxy::Row {
public:
    /// The space between two groups of items.
    static constexpr auto group_gap = 16.0F;

    /// Builds the toolbar.
    ToolbarView();
};

} // namespace gitrose::ui
