#pragma once

#include <hixxy/widgets/flex.hpp>

#include "ui/sample/sample_repository.hpp"

namespace gitrose::ui {

class CommitBrowser;

/// The History view: a filter bar (`Row.view-bar`: the branch filter, Show Remote Branches and the
/// ordering) above a CommitBrowser.
class HistoryView final : public hixxy::Column {
public:
    /// The view of a repository's history.
    /// @param repository The repository (must outlive the view).
    explicit HistoryView(const RepositorySample& repository);

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
