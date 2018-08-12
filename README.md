# Bin Packing Meets Window Management

* CURRENTLY BROKEN *
The previous state this was left in shouldn't really be ran anywhere, as it's too intensive for sane use - so I've pushed the new design through to master. Please don't use this until 0.1 release.

This is a small project used in tandem with [hwwm](https://github.com/halfwit/hwwm) to lay out my windows in a way that I want
It uses a relaxed variant of the 2D Bin Pack, with a caveat that the rectangles have variable sizes.
The algorithm will try to lay out windows within the given space up to the max for each rectangle.
