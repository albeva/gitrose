#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <memory>
#include <utility>

#include <hixxy/core/geometry.hpp>
#include <hixxy/gfx/painter.hpp>
#include <hixxy/ui/layout.hpp>
#include <hixxy/ui/widget.hpp>
#include <hixxy/widgets/common.hpp>

namespace gitrose::ui {

/// Two panes side by side or one above the other, with a divider the user drags to share the
/// space between them.
///
/// - The panes and the divider always fill the splitter's length (along its orientation); the
///   panes fill its whole cross extent.
/// - The divider is a hairline in `--border-color`, `--border-width` thick. Its grab area is the
///   line plus grab_margin points on each side, over the panes' edges; over it and while dragging,
///   the pointer shows the resize cursor of the orientation.
/// - Each pane has a minimum length. The stored position (a length or a ratio, by the Resize mode)
///   gives the first pane's length through layout(); the kept pane gives way to the other's
///   minimum, and below both minimums the first keeps its minimum and the second gets the rest.
/// - Dragging converts the pointer position into a new stored position through the same clamp,
///   so a drag stops where a pane reaches its minimum.
class Splitter : public hixxy::Widget {
public:
    /// The style type `Splitter`.
    HIXXY_STYLE_TYPE("Splitter", Widget)

    /// What the splitter keeps when its own length changes.
    enum class Resize : std::uint8_t {
        KeepFirst,  ///< The first pane's length (the position is that length).
        KeepSecond, ///< The second pane's length (the position is that length).
        KeepRatio,  ///< The first pane's share of the panes' length (the position is a ratio).
    };

    /// The input of layout(), along the splitter's orientation.
    struct Geometry {
        float length = 0.0F;               ///< The splitter's length.
        float divider = 1.0F;              ///< The divider's thickness.
        float first_minimum = 0.0F;        ///< The first pane's minimum length.
        float second_minimum = 0.0F;       ///< The second pane's minimum length.
        Resize resize = Resize::KeepFirst; ///< How `position` is read.
        float position = 0.0F;             ///< The stored position: a length or a ratio, by `resize`.
    };

    /// The output of layout(): lengths along the orientation, in order. They sum to the length.
    struct Panes {
        float first = 0.0F;   ///< The first pane's length (also the divider's offset).
        float divider = 0.0F; ///< The divider's thickness (less only when the length is).
        float second = 0.0F;  ///< The second pane's length.

        /// Equal when every length is.
        /// @return Whether the panes are equal.
        constexpr auto operator==(const Panes&) const noexcept -> bool = default;
    };

    /// How far the divider's grab area extends beyond its line on each side, in points.
    static constexpr auto grab_margin = 3.0F;

    /// Lays the panes out along the orientation (see the class documentation).
    /// @param geometry The lengths, minimums and stored position.
    /// @return The panes' and the divider's lengths.
    [[nodiscard]] static constexpr auto layout(const Geometry& geometry) noexcept -> Panes {
        const auto length = std::max(geometry.length, 0.0F);
        const auto divider = std::min(std::max(geometry.divider, 0.0F), length);
        const auto available = length - divider;
        const auto first = clamp_first(geometry, available, wanted_first(geometry, available));
        return { .first = first, .divider = divider, .second = available - first };
    }

    /// The stored position after a drag that puts the divider at @p offset: the offset is
    /// clamped as layout() clamps the first pane, then converted to a position by the resize
    /// mode.
    /// @param geometry The lengths, minimums and the position before the drag.
    /// @param offset The divider's wanted offset from the start (the first pane's length).
    /// @return The new position.
    [[nodiscard]] static constexpr auto dragged(const Geometry& geometry, const float offset) noexcept -> float {
        const auto panes = layout(geometry);
        const auto available = panes.first + panes.second;
        const auto first = clamp_first(geometry, available, offset);
        switch (geometry.resize) {
        case Resize::KeepFirst:
            return first;
        case Resize::KeepSecond:
            return available - first;
        case Resize::KeepRatio:
            return available > 0.0F ? first / available : geometry.position;
        }

        return geometry.position;
    }

    /// The options of a splitter.
    struct Options {
        hixxy::Orientation orientation = hixxy::Orientation::Horizontal; ///< Side by side or stacked.
        Resize resize = Resize::KeepFirst;                               ///< What resizing keeps.
        float position = 0.0F;                                           ///< The initial position: a length or a ratio, by `resize`.
        float first_minimum = 0.0F;                                      ///< The first pane's minimum length.
        float second_minimum = 0.0F;                                     ///< The second pane's minimum length.
    };

    /// A splitter without panes.
    /// @param options The orientation, resize mode, initial position and minimums.
    explicit Splitter(const Options& options);

    /// Out of line (the divider's handle is an incomplete type here).
    ~Splitter() override;

    /// Not copyable.
    Splitter(const Splitter&) = delete;
    /// Not copyable.
    /// @return Not applicable.
    auto operator=(const Splitter&) -> Splitter& = delete;
    /// Not movable.
    Splitter(Splitter&&) = delete;
    /// Not movable.
    /// @return Not applicable.
    auto operator=(Splitter&&) -> Splitter& = delete;

    /// Creates the first pane (left or top), replacing any earlier one.
    /// @tparam T The pane's type.
    /// @tparam Args Constructor argument types.
    /// @param args Constructor arguments.
    /// @return The pane.
    template<std::derived_from<hixxy::Widget> T, class... Args>
        requires std::constructible_from<T, Args...>
    auto first(Args&&... args) -> T& {
        auto pane = std::make_unique<T>(std::forward<Args>(args)...);
        auto& added = *pane;
        set_pane(m_first, std::move(pane), 0);
        return added;
    }

    /// Creates the second pane (right or bottom), replacing any earlier one.
    /// @tparam T The pane's type.
    /// @tparam Args Constructor argument types.
    /// @param args Constructor arguments.
    /// @return The pane.
    template<std::derived_from<hixxy::Widget> T, class... Args>
        requires std::constructible_from<T, Args...>
    auto second(Args&&... args) -> T& {
        auto pane = std::make_unique<T>(std::forward<Args>(args)...);
        auto& added = *pane;
        set_pane(m_second, std::move(pane), m_first != nullptr ? 1 : 0);
        return added;
    }

    /// The orientation.
    /// @return Side by side (Horizontal) or stacked (Vertical).
    [[nodiscard]] auto orientation() const noexcept -> hixxy::Orientation { return m_options.orientation; }

    /// The stored position (a length or a ratio, by the resize mode).
    /// @return The position.
    [[nodiscard]] auto position() const noexcept -> float { return m_options.position; }

    /// The divider's rectangle in the splitter's coordinates, as last arranged.
    /// @return The rectangle (the drawn line, without the grab margin).
    [[nodiscard]] auto divider_rect() const noexcept -> hixxy::Rect;

protected:
    /// The panes' natural cross extent and their minimums plus the divider along the orientation.
    /// @param constraints The constraints.
    /// @return The size.
    auto measure_override(const hixxy::Constraints& constraints) -> hixxy::Size override;

    /// Places the panes and the divider's grab area per layout().
    /// @param size The frame size.
    auto arrange_override(hixxy::Size size) -> void override;

    /// Draws the divider's hairline.
    /// @param painter The painter.
    auto paint(hixxy::Painter& painter) -> void override;

private:
    class Handle;

    /// Stores a pane in @p slot, removing the one it held.
    /// @param slot m_first or m_second.
    /// @param pane The new pane.
    /// @param index Its position among the children.
    auto set_pane(hixxy::Widget*& slot, std::unique_ptr<hixxy::Widget> pane, std::size_t index) -> void;

    /// The geometry of the current length along the orientation.
    /// @param length The splitter's length.
    /// @return The geometry.
    [[nodiscard]] auto geometry(float length) const -> Geometry;

    /// The length along the orientation of a size.
    /// @param size The size.
    /// @return Its width or height.
    [[nodiscard]] auto along(hixxy::Size size) const noexcept -> float;

    /// Moves the divider to an offset during a drag (clamped; requests layout when it moved).
    /// @param offset The wanted offset from the splitter's start.
    auto drag_to(float offset) -> void;

    /// The first pane's length before the minimums apply.
    /// @param geometry The geometry.
    /// @param available The length of both panes.
    /// @return The wanted length.
    [[nodiscard]] static constexpr auto wanted_first(const Geometry& geometry, const float available) noexcept -> float {
        switch (geometry.resize) {
        case Resize::KeepFirst:
            return geometry.position;
        case Resize::KeepSecond:
            return available - geometry.position;
        case Resize::KeepRatio:
            return geometry.position * available;
        }

        return geometry.position;
    }

    /// Clamps the first pane's length so that both panes keep their minimums; when they cannot,
    /// the first keeps its minimum (or everything there is) and the second gets the rest.
    /// @param geometry The geometry.
    /// @param available The length of both panes.
    /// @param first The wanted length.
    /// @return The clamped length.
    [[nodiscard]] static constexpr auto clamp_first(
        const Geometry& geometry,
        const float available,
        const float first
    ) noexcept -> float {
        const auto low = std::min(std::max(geometry.first_minimum, 0.0F), available);
        const auto high = std::max(available - std::max(geometry.second_minimum, 0.0F), low);
        return std::min(std::max(first, low), high);
    }

    Options m_options;
    hixxy::Widget* m_first = nullptr;  ///< The first pane (a child), if any.
    hixxy::Widget* m_second = nullptr; ///< The second pane (a child), if any.
    Handle* m_handle = nullptr;        ///< The divider's grab area (the last child).
    Panes m_panes {};                  ///< The last arranged panes.
};

} // namespace gitrose::ui
