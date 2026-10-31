#include <vector>

#include <doctest/doctest.h>

#include <hixxy/hixxy.hpp>
#include <hixxy/testing/ui_test.hpp>

#include "ui/style.hpp"
#include "ui/toolbar/toolbar_view.hpp"
#include "ui/widgets/toolbar_button.hpp"

using namespace gitrose::ui;

namespace {

auto items(const ToolbarView& toolbar) -> std::vector<const ToolbarButton*> {
    auto found = std::vector<const ToolbarButton*> {};
    for (const auto& child : toolbar.children()) {
        if (const auto* const item = dynamic_cast<const ToolbarButton*>(child.get())) {
            found.push_back(item);
        }
    }

    return found;
}

} // namespace

TEST_CASE("toolbar: the items in Sourcetree's order") {
    hixxy::testing::UiTest test({ .window_size = { 1000.0F, 100.0F }, .render = false });
    apply_style(test.app());
    const auto& toolbar = test.set_content<ToolbarView>();

    auto captions = std::vector<hixxy::String> {};
    for (const auto* const item : items(toolbar)) {
        captions.push_back(item->text());
    }

    const auto expected = std::vector<hixxy::String> {
        "Commit",
        "Pull",
        "Push",
        "Fetch",
        "Branch",
        "Merge",
        "Stash",
        "Discard",
        "Tag",
        "Show in Finder",
        "Terminal",
        "Settings",
    };
    CHECK(captions == expected);
}

TEST_CASE("toolbar: widening the window grows only the flexible space") {
    hixxy::testing::UiTest test({ .window_size = { 1000.0F, 100.0F }, .render = false });
    apply_style(test.app());
    const auto& toolbar = test.set_content<ToolbarView>();
    test.window().layout();

    const auto& space = test.find("toolbar-flexible-space");
    const auto& settings = test.find("toolbar-settings");
    auto widths = std::vector<float> {};
    for (const auto* const item : items(toolbar)) {
        widths.push_back(item->frame().width);
    }

    const auto space_width = space.frame().width;
    CHECK(settings.window_rect().right() <= 1000.0F);
    CHECK(settings.window_rect().right() >= 980.0F);

    test.resize({ 1200.0F, 100.0F });
    test.window().layout();

    CHECK(space.frame().width == doctest::Approx(space_width + 200.0F));
    auto index = std::size_t { 0 };
    for (const auto* const item : items(toolbar)) {
        CHECK(item->frame().width == doctest::Approx(widths.at(index)));
        ++index;
    }
}

TEST_CASE("toolbar: every item fits the minimum window width") {
    hixxy::testing::UiTest test({ .window_size = { 900.0F, 100.0F }, .render = false });
    apply_style(test.app());
    test.set_content<ToolbarView>();
    test.window().layout();

    CHECK(test.find("toolbar-flexible-space").frame().width > 0.0F);
    CHECK(test.find("toolbar-settings").window_rect().right() <= 900.0F);
}
