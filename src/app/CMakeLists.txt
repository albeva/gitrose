add_executable(gitrose main.cpp)
gitrose_configure_target(gitrose)
target_link_libraries(gitrose PRIVATE hixxy::hixxy)
