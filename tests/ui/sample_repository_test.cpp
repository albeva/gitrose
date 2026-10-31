#include <algorithm>

#include <doctest/doctest.h>

#include "ui/sample/sample_repository.hpp"

using namespace gitrose::ui;

TEST_CASE("the samples are gitrose and hixxy") {
    const auto& samples = RepositorySample::samples();
    REQUIRE(samples.size() == 2);
    CHECK(samples.at(0).name == "gitrose");
    CHECK(samples.at(1).name == "hixxy");
}

TEST_CASE("each sample has exactly one current branch and one current worktree") {
    for (const auto& sample : RepositorySample::samples()) {
        CAPTURE(sample.name);
        CHECK(std::ranges::count_if(sample.branches, &RepositorySample::Branch::current) == 1);
        CHECK(std::ranges::count_if(sample.worktrees, &RepositorySample::Worktree::current) == 1);
        CHECK(sample.worktrees.size() == 3);
        CHECK(sample.worktrees.front().current);
    }
}

TEST_CASE("each sample has thirty commits, each with changed files") {
    for (const auto& sample : RepositorySample::samples()) {
        CAPTURE(sample.name);
        CHECK(sample.commits.size() == 30);
        CHECK(std::ranges::none_of(sample.commits, [](const auto& commit) { return commit.files.empty(); }));
    }
}
