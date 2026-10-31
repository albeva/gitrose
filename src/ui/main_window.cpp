#include "ui/main_window.hpp"

#include <hixxy/hixxy.hpp>

#include "ui/repository/repository_page.hpp"
#include "ui/sample/sample_repository.hpp"
#include "ui/toolbar/toolbar_view.hpp"

using namespace hixxy;
using namespace gitrose::ui;

MainWindow::MainWindow() {
    spacing(0.0F);
    add<ToolbarView>();
    add<Separator>();

    m_tabs = &add<TabView>().name("repository-tabs").flex(1.0F);
    for (const auto& sample : RepositorySample::samples()) {
        m_tabs->add_tab<RepositoryPage>(String { StringView { sample.name } }, sample);
    }
}

auto MainWindow::build(Window& window) -> MainWindow& {
    return window.set_content<MainWindow>();
}
