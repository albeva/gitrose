#include "ui/sidebar/sidebar_model.hpp"

#include <algorithm>
#include <iterator>

#include <hixxy/gfx/icons/lucide.hpp>
#include <hixxy/hixxy.hpp>

using namespace hixxy;
using namespace gitrose::ui;

namespace {

auto to_string(const std::string& text) -> String {
    return String { StringView { text } };
}

} // namespace

SidebarModel::SidebarModel(const RepositorySample& repository) {
    add_heading(Section::Workspace);
    add_entry(Section::Workspace, "File Status", lucide::FilePen).workspace = Workspace::FileStatus;
    add_entry(Section::Workspace, "History", lucide::History).workspace = Workspace::History;
    add_entry(Section::Workspace, "Search", lucide::Search).workspace = Workspace::Search;

    add_heading(Section::Branches);
    for (const auto& branch : repository.branches) {
        add_entry(Section::Branches, to_string(branch.name), lucide::GitBranch).current = branch.current;
    }

    add_heading(Section::Tags);
    for (const auto& tag : repository.tags) {
        add_entry(Section::Tags, to_string(tag), lucide::Tag);
    }

    add_heading(Section::Remotes);
    for (const auto& remote : repository.remotes) {
        add_entry(Section::Remotes, to_string(remote), lucide::Cloud);
    }

    add_heading(Section::Stashes);
    for (const auto& stash : repository.stashes) {
        add_entry(Section::Stashes, to_string(stash), lucide::Archive);
    }

    add_heading(Section::Worktrees);
    for (const auto& worktree : repository.worktrees) {
        auto& entry = add_entry(Section::Worktrees, to_string(worktree.name), lucide::FolderGit2);
        entry.detail = to_string(worktree.branch);
        entry.current = worktree.current;
    }

    add_heading(Section::Submodules);
    for (const auto& submodule : repository.submodules) {
        add_entry(Section::Submodules, to_string(submodule), lucide::Package);
    }
}

auto SidebarModel::text(const std::size_t row) const -> String {
    return row < m_rows.size() ? m_rows.at(row).name : String {};
}

auto SidebarModel::row_of(const Workspace workspace) const noexcept -> std::optional<std::size_t> {
    const auto found = std::ranges::find(m_rows, std::optional { workspace }, &Row::workspace);
    if (found == m_rows.end()) {
        return std::nullopt;
    }

    return static_cast<std::size_t>(std::distance(m_rows.begin(), found));
}

auto SidebarModel::heading(const Section section) noexcept -> StringView {
    switch (section) {
    case Section::Workspace:
        return "WORKSPACE";
    case Section::Branches:
        return "BRANCHES";
    case Section::Tags:
        return "TAGS";
    case Section::Remotes:
        return "REMOTES";
    case Section::Stashes:
        return "STASHES";
    case Section::Worktrees:
        return "WORKTREES";
    case Section::Submodules:
        return "SUBMODULES";
    }

    return {};
}

auto SidebarModel::add_heading(const Section section) -> void {
    m_rows.push_back({ .section = section, .heading = true, .name = heading(section) });
}

auto SidebarModel::add_entry(const Section section, String name, const Icon& icon) -> Row& {
    return m_rows.emplace_back(Row { .section = section, .name = std::move(name), .icon = icon });
}
