#include <memory>
#include <optional>
#include <vector>

#include <doctest/doctest.h>

#include <hixxy/hixxy.hpp>
#include <hixxy/testing/ui_test.hpp>

#include "ui/sample/sample_repository.hpp"
#include "ui/sidebar/sidebar_model.hpp"
#include "ui/sidebar/sidebar_view.hpp"
#include "ui/style.hpp"

using namespace gitrose::ui;

namespace {

using Section = SidebarModel::Section;
using Workspace = SidebarModel::Workspace;

/// The row of the BRANCHES heading in the gitrose sample.
constexpr auto branches_heading = std::size_t { 4 };

auto gitrose_model() -> std::shared_ptr<SidebarModel> {
    return std::make_shared<SidebarModel>(RepositorySample::samples().front());
}

/// The names of the current entries of a section.
auto current_in(const SidebarModel& model, const Section section) -> std::vector<hixxy::String> {
    auto names = std::vector<hixxy::String> {};
    for (std::size_t row = 0; row < model.row_count(); ++row) {
        const auto& entry = model.at(row);
        if (entry.section == section && entry.current) {
            names.push_back(entry.name);
        }
    }

    return names;
}

} // namespace

TEST_CASE("sidebar model: the sections in order, each heading before its entries") {
    const auto model = gitrose_model();

    auto headings = std::vector<hixxy::String> {};
    auto section = std::optional<Section> {};
    for (std::size_t row = 0; row < model->row_count(); ++row) {
        const auto& entry = model->at(row);
        if (entry.heading) {
            headings.push_back(entry.name);
            section = entry.section;
        } else {
            CHECK(entry.section == section);
        }
    }

    const auto expected = std::vector<hixxy::String> {
        "WORKSPACE",
        "BRANCHES",
        "TAGS",
        "REMOTES",
        "STASHES",
        "WORKTREES",
        "SUBMODULES",
    };
    CHECK(headings == expected);
    CHECK(model->at(1).name == "File Status");
    CHECK(model->at(2).name == "History");
    CHECK(model->at(3).name == "Search");
}

TEST_CASE("sidebar model: the current branch and worktree are marked, worktrees show their branch") {
    const auto model = gitrose_model();
    CHECK(current_in(*model, Section::Branches) == std::vector<hixxy::String> { "main" });
    CHECK(current_in(*model, Section::Worktrees) == std::vector<hixxy::String> { "gitrose" });

    auto worktrees = std::size_t { 0 };
    for (std::size_t row = 0; row < model->row_count(); ++row) {
        const auto& entry = model->at(row);
        if (entry.section == Section::Worktrees && !entry.heading) {
            CHECK_FALSE(entry.detail.empty());
            ++worktrees;
        }
    }

    CHECK(worktrees == 3);
}

TEST_CASE("sidebar view: selecting a heading keeps the previous selection") {
    hixxy::testing::UiTest test({ .window_size = { 240.0F, 800.0F }, .render = false });
    apply_style(test.app());
    auto& sidebar = test.set_content<SidebarView>(gitrose_model());
    auto shown = std::vector<Workspace> {};
    sidebar.workspace_selected.connect([&shown](const Workspace workspace) { shown.push_back(workspace); });
    test.settle();

    sidebar.select_workspace(Workspace::History);
    REQUIRE(sidebar.selected_row() == 2);
    CHECK(shown == std::vector<Workspace> { Workspace::History });

    // A click on the BRANCHES heading.
    REQUIRE(sidebar.sidebar_model().at(branches_heading).heading);
    auto* const heading = sidebar.row_widget(branches_heading);
    REQUIRE(heading != nullptr);
    test.click(*heading);
    CHECK(sidebar.selected_row() == 2);
    CHECK(shown.size() == 1);

    // With nothing selected before, a heading leaves nothing selected.
    sidebar.clear_selection();
    sidebar.select(0);
    CHECK_FALSE(sidebar.selected_row().has_value());
}

TEST_CASE("sidebar view: selecting a branch shows no other view") {
    hixxy::testing::UiTest test({ .window_size = { 240.0F, 800.0F }, .render = false });
    apply_style(test.app());
    auto& sidebar = test.set_content<SidebarView>(gitrose_model());
    auto shown = std::vector<Workspace> {};
    sidebar.workspace_selected.connect([&shown](const Workspace workspace) { shown.push_back(workspace); });
    test.settle();
    sidebar.select_workspace(Workspace::History);

    auto* const branch = sidebar.row_widget(branches_heading + 1);
    REQUIRE(branch != nullptr);
    test.click(*branch);
    CHECK(sidebar.selected_row() == branches_heading + 1);
    CHECK(shown == std::vector<Workspace> { Workspace::History });
}
