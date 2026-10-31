#pragma once

#include <hixxy/widgets/panel.hpp>

namespace gitrose::ui {

/// The diff area until diffs are rendered: a panel with a centred, secondary message.
class DiffPlaceholder final : public hixxy::Panel {
public:
    /// The placeholder.
    DiffPlaceholder();
};

} // namespace gitrose::ui
