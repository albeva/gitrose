#include "ui/style.hpp"

#include <string_view>

#include <hixxy/style/stylesheet.hpp>
#include <hixxy/ui/application.hpp>

using namespace hixxy;
using namespace gitrose::ui;

namespace {

/// gitrose's style sheet.
constexpr auto stylesheet = std::string_view { R"css(
Row.toolbar {
    background-color: var(--surface);
    padding: 6px 10px;
    --spacing: 2px;
}

ToolbarButton {
    padding: 4px 6px;
    border-radius: var(--radius-small);
    color: var(--text);
    font-size: var(--font-size-caption);
    --icon-size: 20px;
}

ToolbarButton:hover {
    background-color: var(--surface-hover);
}

ToolbarButton:pressed {
    background-color: var(--surface-pressed);
}

ToolbarButton:disabled {
    color: var(--text-disabled);
}

ListView.sidebar {
    background-color: var(--surface-variant);
}

.sidebar-row {
    --spacing: 6px;
    padding: 0 8px;
}

.sidebar-row.sidebar-entry {
    padding-left: 10px;
}

.sidebar-row.sidebar-heading {
    padding-top: 6px;
}

.sidebar-heading Label.sidebar-name {
    color: var(--text-secondary);
    font-size: var(--font-size-caption);
    font-weight: var(--font-weight-strong);
}

.sidebar-current Label.sidebar-name,
.sidebar-current Label.sidebar-mark {
    font-weight: var(--font-weight-strong);
}

Row.view-bar {
    padding: 6px 8px;
}

Label.file-list-heading {
    padding: 6px 8px 2px 8px;
    color: var(--text-secondary);
    font-weight: var(--font-weight-strong);
}

Column.commit-area {
    padding: 8px;
}
)css" };

} // namespace

auto gitrose::ui::apply_style(Application& app) -> void {
    app.add_stylesheet(StyleSheet::parse(stylesheet, { .source = "gitrose.css" }));
}
