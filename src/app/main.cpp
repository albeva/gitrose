// gitrose - the application executable: creates the hixxy application and opens the main window.

#include <hixxy/hixxy.hpp>

#include "ui/main_window.hpp"
#include "ui/style.hpp"

using namespace hixxy;
using namespace gitrose::ui;

auto main() -> int {
    auto app = Application({ .name = "gitrose" });
    apply_style(app);

    auto window = app.create_window({
        .title = "gitrose",
        .size = MainWindow::initial_size,
        .min_size = MainWindow::minimum_size,
    });
    if (!window) {
        return 1; // the error has been logged
    }

    MainWindow::build(**window);
    return app.run();
}
