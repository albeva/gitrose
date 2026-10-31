#pragma once

#include <hixxy/widgets/flex.hpp>

#include "ui/sample/sample_repository.hpp"

namespace gitrose::ui {

class CommitBrowser;

/// The Search view: a search bar (`Row.view-bar`: the search field and what to search: commit
/// messages, file changes or authors) above a CommitBrowser.
class SearchView final : public hixxy::Column {
public:
    /// The search view of a repository.
    /// @param repository The repository (must outlive the view).
    explicit SearchView(const RepositorySample& repository);

    /// The commit table and details.
    /// @return The browser.
    [[nodiscard]] auto browser() const noexcept -> CommitBrowser& { return *m_browser; }

private:
    /// Adds the bar, a separator and the commit browser.
    /// @param repository The repository.
    /// @return The browser.
    auto add_content(const RepositorySample& repository) -> CommitBrowser&;

    CommitBrowser* m_browser;
};

} // namespace gitrose::ui
