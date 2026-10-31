#pragma once

#include <utility>

#include <hixxy/core/geometry.hpp>
#include <hixxy/core/signal.hpp>
#include <hixxy/gfx/attributed_string.hpp>
#include <hixxy/gfx/icon.hpp>
#include <hixxy/gfx/painter.hpp>
#include <hixxy/ui/layout.hpp>
#include <hixxy/widgets/button.hpp>

namespace gitrose::ui {

/// A toolbar item: an icon above a caption, as in a macOS toolbar. Style type `ToolbarButton`;
/// the style sheet gives it its padding, caption font and hover and pressed layers. It takes no
/// focus from a click. A click emits `clicked`.
class ToolbarButton final : public hixxy::AbstractButton {
public:
    /// The style type `ToolbarButton`.
    HIXXY_STYLE_TYPE("ToolbarButton", Widget)

    /// The narrowest item, so short captions get the same width as most others.
    static constexpr auto min_width = 56.0F;

    /// The gap between the icon and the caption.
    static constexpr auto caption_gap = 3.0F;

    /// An item.
    /// @param icon The icon.
    /// @param caption The caption under it.
    ToolbarButton(hixxy::Icon icon, hixxy::AttributedString caption);

    /// The icon.
    /// @return The icon.
    [[nodiscard]] auto icon() const noexcept -> const hixxy::Icon& { return m_icon; }

    /// Emitted when the item is clicked.
    hixxy::Signal<> clicked;

    /// Connects a slot to `clicked` (Widget::on() forms).
    /// @tparam Self The widget's type (deduced).
    /// @tparam A Argument types.
    /// @param self The item.
    /// @param a A slot, a lifetime context and a slot, or a receiver and a member function.
    /// @return @p self.
    template<class Self, class... A>
    auto on_click(this Self& self, A&&... a) -> Self& {
        return self.on(&ToolbarButton::clicked, std::forward<A>(a)...);
    }

protected:
    /// Emits `clicked`.
    auto activate() -> void override;

    /// The icon's size above the caption's, plus padding.
    /// @param constraints The constraints.
    /// @return The size.
    auto measure_override(const hixxy::Constraints& constraints) -> hixxy::Size override;

    /// Draws the icon centred at the top and the caption centred under it.
    /// @param painter The painter.
    auto paint(hixxy::Painter& painter) -> void override;

    /// The caption's line, centred under the icon.
    /// @param size The frame size.
    /// @return The caption's box.
    [[nodiscard]] auto text_box(hixxy::Size size) -> hixxy::Rect override;

private:
    /// The icon's side: the `--icon-size` token.
    /// @return The side.
    [[nodiscard]] auto icon_size() const -> float;

    hixxy::Icon m_icon;
};

} // namespace gitrose::ui
