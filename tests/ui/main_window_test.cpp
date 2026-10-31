#include <doctest/doctest.h>

#include <hixxy/hixxy.hpp>
#include <hixxy/testing/ui_test.hpp>

#include "ui/main_window.hpp"
#include "ui/repository/repository_page.hpp"
#include "ui/sidebar/sidebar_view.hpp"
#include "ui/style.hpp"

using namespace gitrose::ui;

namespace {

using Workspace = RepositoryPage::Workspace;

auto page(const MainWindow& window, const std::size_t tab) -> RepositoryPage& {
    auto* const found = dynamic_cast<RepositoryPage*>(window.tabs().page(tab));
    REQUIRE(found != nullptr);
    return *found;
}

auto make_options() -> hixxy::testing::UiTestOptions {
    return { .window_size = MainWindow::initial_size, .render = false };
}

} // namespace

TEST_CASE("main window: a tab per sample, the first current, each showing History") {
    hixxy::testing::UiTest test(make_options());
    apply_style(test.app());
    const auto& window = MainWindow::build(test.window());
    test.settle();

    REQUIRE(window.tabs().tab_count() == 2);
    CHECK(window.tabs().tab_title(0) == "gitrose");
    CHECK(window.tabs().tab_title(1) == "hixxy");
    CHECK(window.tabs().current_index() == 0);
    CHECK(page(window, 0).workspace() == Workspace::History);
    CHECK(page(window, 0).view(Workspace::History).visible());
    CHECK_FALSE(page(window, 0).view(Workspace::FileStatus).visible());
}

TEST_CASE("main window: each tab keeps its workspace view") {
    hixxy::testing::UiTest test(make_options());
    apply_style(test.app());
    const auto& window = MainWindow::build(test.window());
    test.settle();

    const auto& first = page(window, 0);
    const auto file_status = first.sidebar().sidebar_model().row_of(Workspace::FileStatus);
    REQUIRE(file_status.has_value());
    auto* const row = first.sidebar().row_widget(file_status.value_or(0));
    REQUIRE(row != nullptr);
    test.click(*row);
    CHECK(first.workspace() == Workspace::FileStatus);
    CHECK(first.view(Workspace::FileStatus).visible());
    CHECK_FALSE(first.view(Workspace::History).visible());

    window.tabs().current_index(1);
    test.settle();
    CHECK(page(window, 1).workspace() == Workspace::History);
    CHECK(page(window, 1).view(Workspace::History).visible());

    window.tabs().current_index(0);
    test.settle();
    CHECK(first.workspace() == Workspace::FileStatus);
    CHECK(first.view(Workspace::FileStatus).visible());
}

TEST_CASE("main window: the sidebar keeps its width when the window grows") {
    hixxy::testing::UiTest test(make_options());
    apply_style(test.app());
    const auto& window = MainWindow::build(test.window());
    test.settle();

    const auto& sidebar = page(window, 0).sidebar();
    const auto& content = page(window, 0).view(Workspace::History);
    const auto width = content.frame().width;
    CHECK(sidebar.frame().width == doctest::Approx(RepositoryPage::sidebar_width));

    test.resize({ MainWindow::initial_size.width + 200.0F, MainWindow::initial_size.height });
    test.window().layout();
    CHECK(sidebar.frame().width == doctest::Approx(RepositoryPage::sidebar_width));
    CHECK(content.frame().width == doctest::Approx(width + 200.0F));
}
