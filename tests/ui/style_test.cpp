#include <doctest/doctest.h>

#include <hixxy/testing/ui_test.hpp>

#include "ui/style.hpp"

TEST_CASE("the gitrose style sheet installs") {
    hixxy::testing::UiTest test;
    gitrose::ui::apply_style(test.app());
    CHECK(test.settle());
}
