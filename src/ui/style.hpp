#pragma once

namespace hixxy {
class Application;
} // namespace hixxy

namespace gitrose::ui {

/// Adds gitrose's style sheet to the application: rules for gitrose's own widgets and classes
/// only (toolbar items, sidebar rows, split pane dividers), written with hixxy's design tokens so
/// both colour schemes work. Call once, at start-up, before the window is built.
/// @param app The application.
auto apply_style(hixxy::Application& app) -> void;

} // namespace gitrose::ui
