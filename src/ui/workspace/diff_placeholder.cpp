#include "ui/workspace/diff_placeholder.hpp"

#include <hixxy/hixxy.hpp>

using namespace hixxy;
using namespace gitrose::ui;

DiffPlaceholder::DiffPlaceholder() {
    add_class("diff-placeholder");
    justify(Justify::Center);
    cross_align(Align::Center);
    add<Label>("Select a file to see its changes").tone(Tone::Secondary);
}
