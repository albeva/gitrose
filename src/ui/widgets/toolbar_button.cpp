#include "ui/widgets/toolbar_button.hpp"

#include <algorithm>

#include <hixxy/hixxy.hpp>

using namespace hixxy;
using namespace gitrose::ui;

ToolbarButton::ToolbarButton(Icon icon, AttributedString caption)
: AbstractButton(std::move(caption))
, m_icon(std::move(icon)) {
    focus_policy(FocusPolicy::TabFocus);
}

auto ToolbarButton::activate() -> void {
    clicked.emit();
}

auto ToolbarButton::measure_override(const Constraints& constraints) -> Size {
    const auto text = text_size().value_or(Size {});
    const auto icon = icon_size();
    const auto width = std::max(std::max(icon, text.width) + padding().horizontal(), min_width);
    const auto height = icon + caption_gap + text.height + padding().vertical();
    return constraints.constrain({ width, height });
}

auto ToolbarButton::paint(Painter& painter) -> void {
    const auto content = content_rect();
    const auto icon = icon_size();
    const auto box = Rect { content.x + ((content.width - icon) / 2.0F), content.y, icon, icon };
    const auto color = effectively_enabled() ? computed_style().color() : token_color("--text-disabled").value_or(computed_style().color());
    draw_icon(painter, m_icon, box, color, font());
    draw_text(painter, text_box(size()));
}

auto ToolbarButton::text_box(const Size size) -> Rect {
    const auto content = Rect::from_size(size).inset(padding());
    const auto text = text_size().value_or(Size {});
    const auto width = std::min(text.width, content.width);
    const auto top = content.y + icon_size() + caption_gap;
    return { content.x + ((content.width - width) / 2.0F), top, width, std::max(content.bottom() - top, 0.0F) };
}

auto ToolbarButton::icon_size() const -> float {
    return computed_style().var_length("--icon-size").value_or(16.0F);
}
