#pragma once

#include <hixxy/widgets/flex.hpp>

#include "ui/sample/sample_repository.hpp"

namespace gitrose::ui {

/// The File Status view: a top bar (`Row.view-bar`: which files and their order), then the staged
/// files above the unstaged ones (lists named `staged-files` and `unstaged-files`, each under its
/// heading) beside the diff area, and the commit area along the bottom: a multi-line message field
/// (`commit-message`) and a Commit button.
class FileStatusView final : public hixxy::Column {
public:
    /// The file status of a repository.
    /// @param repository The repository.
    explicit FileStatusView(const RepositorySample& repository);
};

} // namespace gitrose::ui
