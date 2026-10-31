# gitrose_ui: the window's hixxy widgets. A major component (a view with its own model, rows or
# helpers) gets a folder of its own; sample/ holds the static sample data the views show until the
# git module exists.
add_library(gitrose_ui STATIC
    main_window.cpp
    repository/repository_page.cpp
    sample/sample_repository.cpp
    sidebar/sidebar_model.cpp
    sidebar/sidebar_view.cpp
    style.cpp
    toolbar/toolbar_view.cpp
    widgets/splitter.cpp
    widgets/toolbar_button.cpp
    workspace/commit_browser.cpp
    workspace/diff_placeholder.cpp
    workspace/file_list.cpp
    workspace/file_status_view.cpp
    workspace/history_view.cpp
    workspace/search_view.cpp)
gitrose_configure_target(gitrose_ui)
target_include_directories(gitrose_ui PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(gitrose_ui PUBLIC hixxy::hixxy)
