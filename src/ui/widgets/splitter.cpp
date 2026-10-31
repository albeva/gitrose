#include "ui/widgets/splitter.hpp"

#include <hixxy/hixxy.hpp>

using namespace hixxy;
using namespace gitrose::ui;

/// The divider's grab area: the hairline plus Splitter::grab_margin on each side, laid over the
/// panes' edges as the splitter's last child, so it is hit before them. It shows the resize
/// cursor and turns a drag into Splitter::drag_to() calls.
class Splitter::Handle final : public Widget {
public:
    explicit Handle(Splitter& splitter)
    : m_splitter(splitter) {
        cursor(splitter.orientation() == Orientation::Horizontal ? Cursor::ResizeHorizontal : Cursor::ResizeVertical);
    }

protected:
    auto mouse_down_event(const MouseEvent& event) -> bool override {
        if (event.button != MouseButton::Left) {
            return false;
        }

        // Where in the divider the press landed, so the line keeps its place under the pointer.
        m_grab = along(event.position) - grab_margin;
        return true; // captures the mouse until the release
    }

    auto mouse_move_event(const MouseEvent& event) -> bool override {
        if (!pressed()) {
            return false;
        }

        const auto position = m_splitter.map_from_window(event.window_position);
        m_splitter.drag_to(along(position) - m_grab);
        return true;
    }

    auto mouse_up_event(const MouseEvent& /*event*/) -> bool override { return true; }

private:
    [[nodiscard]] auto along(const Point point) const noexcept -> float {
        return m_splitter.orientation() == Orientation::Horizontal ? point.x : point.y;
    }

    Splitter& m_splitter; ///< The parent.
    float m_grab = 0.0F;  ///< The press's offset from the divider's start.
};

Splitter::Splitter(const Options& options)
: m_options(options)
, m_handle(&add_child<Handle>(*this)) {}

Splitter::~Splitter() = default;

auto Splitter::divider_rect() const noexcept -> Rect {
    if (m_options.orientation == Orientation::Horizontal) {
        return { m_panes.first, 0.0F, m_panes.divider, size().height };
    }

    return { 0.0F, m_panes.first, size().width, m_panes.divider };
}

auto Splitter::measure_override(const Constraints& constraints) -> Size {
    const auto loose = Constraints::loose(constraints.max);
    auto cross = 0.0F;
    for (auto* const pane : { m_first, m_second }) {
        if (pane != nullptr) {
            const auto measured = pane->measure_with_margin(loose);
            cross = std::max(cross, m_options.orientation == Orientation::Horizontal ? measured.height : measured.width);
        }
    }

    const auto divider = computed_style().var_length("--border-width").value_or(1.0F);
    const auto main = m_options.first_minimum + divider + m_options.second_minimum;
    return constraints.constrain(m_options.orientation == Orientation::Horizontal ? Size { main, cross } : Size { cross, main });
}

auto Splitter::arrange_override(const Size size) -> void {
    m_panes = layout(geometry(along(size)));

    const auto horizontal = m_options.orientation == Orientation::Horizontal;
    const auto slot = [&](const float offset, const float length) {
        return horizontal ? Rect { offset, 0.0F, length, size.height } : Rect { 0.0F, offset, size.width, length };
    };

    if (m_first != nullptr) {
        m_first->arrange_in(slot(0.0F, m_panes.first));
    }

    if (m_second != nullptr) {
        m_second->arrange_in(slot(m_panes.first + m_panes.divider, m_panes.second));
    }

    m_handle->arrange(slot(m_panes.first - grab_margin, m_panes.divider + (2.0F * grab_margin)));
}

auto Splitter::paint(Painter& painter) -> void {
    const auto color = token_color("--border-color").value_or(Color::black());
    painter.fill_rect(painter.snapped(divider_rect()), color);
}

auto Splitter::set_pane(Widget*& slot, std::unique_ptr<Widget> pane, const std::size_t index) -> void {
    if (slot != nullptr) {
        remove_child(*slot);
    }

    slot = insert_child(index, std::move(pane));
}

auto Splitter::geometry(const float length) const -> Geometry {
    return {
        .length = length,
        .divider = computed_style().var_length("--border-width").value_or(1.0F),
        .first_minimum = m_options.first_minimum,
        .second_minimum = m_options.second_minimum,
        .resize = m_options.resize,
        .position = m_options.position,
    };
}

auto Splitter::along(const Size size) const noexcept -> float {
    return m_options.orientation == Orientation::Horizontal ? size.width : size.height;
}

auto Splitter::drag_to(const float offset) -> void {
    const auto position = dragged(geometry(along(this->size())), offset);
    if (position != m_options.position) {
        m_options.position = position;
        request_layout();
    }
}
