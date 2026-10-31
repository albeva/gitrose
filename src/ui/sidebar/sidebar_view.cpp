#include "ui/sidebar/sidebar_view.hpp"

#include <hixxy/gfx/icons/lucide.hpp>
#include <hixxy/hixxy.hpp>

using namespace hixxy;
using namespace gitrose::ui;

namespace {

/// The widget of one sidebar row: a heading caption, or an entry's current mark, icon, name and
/// detail.
class SidebarRow final : public Row {
public:
    /// The width of the current mark's slot, kept by every entry so names line up.
    static constexpr auto mark_width = 8.0F;

    SidebarRow()
    : m_mark(&add<Label>("").add_class("sidebar-mark").fixed_width(mark_width))
    , m_icon(&add<IconView>(lucide::Dot).tone(Tone::Secondary))
    , m_name(&add<Label>("").add_class("sidebar-name").flex(1.0F))
    , m_detail(&add<Label>("", TextRole::Caption).tone(Tone::Secondary)) {
        cross_align(Align::Center);
        add_class("sidebar-row");
    }

    auto bind(const SidebarModel::Row& row) -> void {
        toggle_class("sidebar-heading", row.heading);
        toggle_class("sidebar-entry", !row.heading);
        toggle_class("sidebar-current", row.current);
        m_mark->visible(!row.heading).text(row.current ? "•" : "");
        m_icon->visible(row.icon.has_value());
        if (row.icon) {
            m_icon->icon(*row.icon);
        }

        m_name->text(row.name);
        m_detail->visible(!row.detail.empty()).text(row.detail);
    }

private:
    Label* m_mark;
    IconView* m_icon;
    Label* m_name;
    Label* m_detail;
};

} // namespace

SidebarView::SidebarView(std::shared_ptr<SidebarModel> model)
: ListView(model)
, m_model(std::move(model)) {
    add_class("sidebar");
    delegate(
        [] { return std::make_unique<SidebarRow>(); },
        [rows = m_model](SidebarRow& row, const std::size_t index) { row.bind(rows->at(index)); }
    );
    on_selection_change(*this, [this] { selection_moved(); });
}

auto SidebarView::selected_entry() const -> const SidebarModel::Row* {
    const auto row = selected_row();
    return row ? &m_model->at(*row) : nullptr;
}

auto SidebarView::select_workspace(const SidebarModel::Workspace workspace) -> void {
    if (const auto row = m_model->row_of(workspace)) {
        select(*row);
    }
}

auto SidebarView::selection_moved() -> void {
    const auto row = selected_row();
    if (row && m_model->at(*row).heading) {
        if (m_selected) {
            select(*m_selected);
        } else {
            clear_selection();
        }

        return;
    }

    if (row == m_selected) {
        return; // restored after a heading was picked
    }

    m_selected = row;
    if (row) {
        if (const auto workspace = m_model->at(*row).workspace) {
            workspace_selected.emit(*workspace);
        }
    }
}
