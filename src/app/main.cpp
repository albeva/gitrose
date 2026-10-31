// gitrose - the application executable: creates the hixxy application and opens the main window.

#include <hixxy/hixxy.hpp>

using namespace hixxy;

namespace {

/// The size of the window's content area when gitrose starts, in points.
constexpr auto initial_window_size = Size { 1280.0F, 800.0F };

} // namespace

auto main() -> int {
    auto app = Application({ .name = "gitrose" });
    auto window = app.create_window({ .title = "gitrose", .size = initial_window_size });
    if (!window) {
        return 1; // the error has been logged
    }

    auto& root = (*window)->set_content<Column>().padding(24.0F).spacing(12.0F);
    root.add<Label>("gitrose", TextRole::Headline);

    return app.run();
}
