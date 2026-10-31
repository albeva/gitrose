#include <doctest/doctest.h>

#include <hixxy/hixxy.hpp>
#include <hixxy/testing/ui_test.hpp>

#include "ui/widgets/splitter.hpp"

using namespace gitrose::ui;

namespace {

using Geometry = Splitter::Geometry;
using Panes = Splitter::Panes;
using Resize = Splitter::Resize;

/// A side-by-side geometry 1000 long with a 1-point divider and minimums of 100 and 200.
constexpr auto geometry(const Resize resize, const float position, const float length = 1000.0F) -> Geometry {
    return {
        .length = length,
        .divider = 1.0F,
        .first_minimum = 100.0F,
        .second_minimum = 200.0F,
        .resize = resize,
        .position = position,
    };
}

constexpr auto sum(const Panes& panes) -> float {
    return panes.first + panes.divider + panes.second;
}

/// Checks that the panes and the divider of one layout fill its length.
auto check_fills(const Resize resize, const float length, const float position) -> void {
    const auto panes = Splitter::layout(geometry(resize, position, length));
    CAPTURE(length);
    CAPTURE(position);
    CHECK(sum(panes) == doctest::Approx(length));
    CHECK(panes.first >= 0.0F);
    CHECK(panes.second >= 0.0F);
}

} // namespace

TEST_CASE("splitter layout: each resize mode places the divider") {
    static_assert(Splitter::layout(geometry(Resize::KeepFirst, 300.0F)) == Panes { 300.0F, 1.0F, 699.0F });
    static_assert(Splitter::layout(geometry(Resize::KeepSecond, 300.0F)) == Panes { 699.0F, 1.0F, 300.0F });
    static_assert(Splitter::layout(geometry(Resize::KeepRatio, 0.6F, 1001.0F)).first == 600.0F);
}

TEST_CASE("splitter layout: resizing keeps the first pane, the second pane or the ratio") {
    CHECK(Splitter::layout(geometry(Resize::KeepFirst, 300.0F, 1200.0F)).first == 300.0F);
    CHECK(Splitter::layout(geometry(Resize::KeepSecond, 300.0F, 1200.0F)).second == 300.0F);
    CHECK(Splitter::layout(geometry(Resize::KeepRatio, 0.6F, 1201.0F)).first == doctest::Approx(720.0F));
}

TEST_CASE("splitter layout: the kept pane gives way to the other's minimum") {
    // Keeping 700 of 800: the second pane would get 99, below its minimum of 200.
    const auto first = Splitter::layout(geometry(Resize::KeepFirst, 700.0F, 800.0F));
    CHECK(first == Panes { 599.0F, 1.0F, 200.0F });

    const auto second = Splitter::layout(geometry(Resize::KeepSecond, 700.0F, 800.0F));
    CHECK(second == Panes { 100.0F, 1.0F, 699.0F });

    const auto ratio = Splitter::layout(geometry(Resize::KeepRatio, 0.05F, 501.0F));
    CHECK(ratio.first == 100.0F);
}

TEST_CASE("splitter layout: too small for both minimums, the first keeps its minimum") {
    CHECK(Splitter::layout(geometry(Resize::KeepFirst, 300.0F, 250.0F)) == Panes { 100.0F, 1.0F, 149.0F });
    CHECK(Splitter::layout(geometry(Resize::KeepSecond, 300.0F, 101.0F)) == Panes { 100.0F, 1.0F, 0.0F });
    CHECK(Splitter::layout(geometry(Resize::KeepRatio, 0.5F, 60.0F)) == Panes { 59.0F, 1.0F, 0.0F });
    CHECK(Splitter::layout(geometry(Resize::KeepFirst, 300.0F, 0.5F)) == Panes { 0.0F, 0.5F, 0.0F });
}

TEST_CASE("splitter layout: the panes and the divider always fill the length") {
    for (const auto resize : { Resize::KeepFirst, Resize::KeepSecond, Resize::KeepRatio }) {
        for (const auto length : { 0.0F, 0.5F, 50.0F, 150.0F, 301.0F, 777.0F, 4000.0F }) {
            for (const auto position : { -10.0F, 0.0F, 0.3F, 0.9F, 150.0F, 5000.0F }) {
                check_fills(resize, length, position);
            }
        }
    }
}

TEST_CASE("splitter drag: within the limits the divider follows the pointer") {
    CHECK(Splitter::dragged(geometry(Resize::KeepFirst, 300.0F), 340.0F) == 340.0F);
    CHECK(Splitter::dragged(geometry(Resize::KeepSecond, 300.0F), 340.0F) == 659.0F);
    CHECK(Splitter::dragged(geometry(Resize::KeepRatio, 0.5F, 1001.0F), 400.0F) == doctest::Approx(0.4F));
}

TEST_CASE("splitter drag: the divider stops where a pane reaches its minimum") {
    CHECK(Splitter::dragged(geometry(Resize::KeepFirst, 300.0F), 20.0F) == 100.0F);
    CHECK(Splitter::dragged(geometry(Resize::KeepFirst, 300.0F), 950.0F) == 799.0F);
    CHECK(Splitter::dragged(geometry(Resize::KeepSecond, 300.0F), 950.0F) == 200.0F);
}

TEST_CASE("splitter: dragging the divider resizes both panes, also from beside the line") {
    hixxy::testing::UiTest test({ .window_size = { 600.0F, 300.0F }, .render = false });
    auto& splitter = test.set_content<Splitter>(Splitter::Options {
        .orientation = hixxy::Orientation::Horizontal,
        .resize = Splitter::Resize::KeepFirst,
        .position = 200.0F,
        .first_minimum = 100.0F,
        .second_minimum = 100.0F,
    });
    const auto& first = splitter.first<hixxy::Stack>().name("first");
    const auto& second = splitter.second<hixxy::Stack>().name("second");
    test.window().layout();

    const auto line = splitter.divider_rect();
    REQUIRE(line.x == 200.0F);
    CHECK(first.frame().width == 200.0F);
    CHECK(second.frame().x == 200.0F + line.width);

    // 3 points left of the line (the press lands inside the first pane's frame).
    const auto beside = hixxy::Point { line.x - 2.75F, 150.0F };
    test.drag(beside, { beside.x + 40.0F, beside.y });
    test.window().layout();
    CHECK(first.frame().width == doctest::Approx(240.0F));
    CHECK(second.frame().x == doctest::Approx(240.0F + line.width));
    CHECK(second.frame().right() == doctest::Approx(600.0F));

    // 3 points right of the line, back by 40.
    const auto right = hixxy::Point { splitter.divider_rect().right() + 2.75F, 150.0F };
    test.drag(right, { right.x - 40.0F, right.y });
    test.window().layout();
    CHECK(first.frame().width == doctest::Approx(200.0F));

    // Past the second pane's minimum: the divider stops.
    const auto middle = splitter.divider_rect().center();
    test.drag(middle, { 590.0F, middle.y });
    test.window().layout();
    CHECK(second.frame().width == doctest::Approx(100.0F));
}

TEST_CASE("splitter: the divider shows the resize cursor of its orientation") {
    hixxy::testing::UiTest test({ .window_size = { 600.0F, 300.0F }, .render = false });
    auto& splitter = test.set_content<Splitter>(Splitter::Options {
        .orientation = hixxy::Orientation::Vertical,
        .resize = Splitter::Resize::KeepRatio,
        .position = 0.5F,
    });
    splitter.first<hixxy::Stack>();
    splitter.second<hixxy::Stack>();
    test.window().layout();

    test.mouse_move(splitter.divider_rect().center());
    CHECK(test.platform().cursor() == hixxy::Cursor::ResizeVertical);
}
