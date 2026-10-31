# ui/split-panes Specification

## Purpose

Two panes side by side or one above the other, with a divider the user drags to share the space
between them: the sidebar beside the content and the panes inside each workspace view.

## Requirements

### Requirement: Divider dragging
A split pane SHALL place its two panes along its orientation (side by side or one above the
other) with a thin divider between them. Dragging the divider SHALL move it with the pointer and
resize both panes, so that the panes and the divider always fill the split pane exactly.

#### Scenario: Drag the divider
- **WHEN** the user drags the divider of a side-by-side split pane 40 points to the right
- **THEN** the first pane is 40 points wider, the second 40 points narrower

#### Scenario: Panes fill the split pane
- **WHEN** a split pane is laid out at any size and divider position
- **THEN** the first pane, the divider and the second pane together cover its whole length,
  without gaps or overlap

### Requirement: Divider grab area
The divider's grab area SHALL extend beyond its drawn line on both sides, so a line thinner than
a pointer is easy to hit. While the pointer is over the grab area or dragging it, the pointer
SHALL show the resize cursor for the split pane's orientation.

#### Scenario: Grab beside the line
- **WHEN** the user presses 3 points beside the drawn divider line and drags
- **THEN** the divider moves

#### Scenario: Resize cursor
- **WHEN** the pointer moves over the divider of a side-by-side split pane
- **THEN** it shows the horizontal resize cursor

### Requirement: Minimum pane sizes
Each pane SHALL have a minimum size. Dragging SHALL stop the divider where either pane would get
smaller than its minimum. When the split pane is smaller than both minimums and the divider
together, the first pane SHALL keep its minimum and the second SHALL get what remains.

#### Scenario: Drag past a minimum
- **WHEN** the user drags the divider towards the first pane, past the point where it would be
  smaller than its minimum
- **THEN** the divider stops with the first pane at its minimum size

#### Scenario: Too small for both
- **WHEN** a split pane is given less length than both minimums and the divider
- **THEN** the first pane has its minimum size and the second pane the rest, possibly nothing

### Requirement: Resize behaviour
Each split pane SHALL have one of three resize behaviours for when its own size changes: keep
the first pane's size, keep the second pane's size, or keep the ratio between them. The pane
whose size is kept SHALL still give way when the other would get smaller than its minimum.

#### Scenario: Sidebar keeps its width
- **WHEN** the window is made 200 points wider
- **THEN** the sidebar keeps its width and the content area grows by 200 points

#### Scenario: Ratio is kept
- **WHEN** a split pane that keeps its ratio, with its divider at 60 % of its length, is resized
- **THEN** the divider stays at 60 % of the new length, within the panes' minimums

#### Scenario: Kept pane gives way
- **WHEN** a split pane that keeps its first pane's size shrinks until the second pane would be
  smaller than its minimum
- **THEN** the first pane shrinks so that the second keeps its minimum

### Requirement: Initial divider position
Each split pane SHALL start with its divider at a position set by the view that uses it: a
length for the pane it keeps, or a ratio for one that keeps its ratio. Divider positions SHALL
NOT be saved between runs in this version.

#### Scenario: Restart
- **WHEN** the user moves a divider, quits and starts gitrose again
- **THEN** the divider is back at its initial position
