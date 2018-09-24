# Bin Packing Meets Window Management

## * CURRENTLY BROKEN *
The previous state this was left in shouldn't really be ran anywhere, as it's too intensive for sane use - so I've pushed the new design through to master. Please don't use this until 0.1 release.

This is a small project used in tandem with [hwwm](https://github.com/halfwit/hwwm) to lay out my windows in a way that I want
It uses a relaxed variant of the 2D Bin Pack, with a caveat that the rectangles have variable sizes.
The algorithm will try to lay out windows within the given space up to the max for each rectangle.

## Layout

Examples:

```
	Our window
	|-----------------------------------------------|
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|-----------------------------------------------|
```
	
We place the first window inside, starting at (0,0) (top left, this is graphics after all)

```
	|------------------|----------------------------|
	|                  |                            |
	|                  |                            |
	|                  |                            |
	|                  |                            |
	|                  |                            |
	|------------------|                            |
	|                  .                            |
	|                  .                            |
	|                  .                            |
	|                  .                            |
	|                  .                            |
	|                  .                            |
	|-----------------------------------------------|

```

The dots here represent the total bounding-box that we'll attempt to place more windows into. This is done for one simple reason: when we attempt to stack windows vertically first, the aesthetic is far better.

```
	|------------------|----------------------------|
	|                  |                            |
	|                  |                            |
	|     Win #1       |                            |
	|                  |                            |
	|                  |                            |
	|----------------|-|                            |
	|                | .                            |
	|                | .                            |
	|     Win #2     | .                            |
	|                | .                            |
	|----------------| .                            |
	|                  .                            |
	|-----------------------------------------------|
	We can see the bounding box doesn't need to expand to
	accomidate the second window here
```

As we'll fill up the vertical space fairly fast, we inevitably will require moving the bounding box to accomidate new windows. 
When we stack the windows, we arrive at some ambiguity about how far to expand the bounding box, to accomidate the next window. We can do it one of two ways:

```
	|------------------|-----------|----------------|    |------------------|----------------------------|
	|                  |           |                |    |                  |         .                  |
	|                  |   Win #3  |                |    |      Win #1      |         .                  |
	|     Win #1       |           |                |    |                  |         .                  |
	|                  |-----------|                |    |                  |         .                  |
	|                  |           .                |    |                  |         .                  |
	|----------------|-|           .                |    |----------------|-|---------|                  |
	|                |             .                |    |                |           |                  |
	|                |             .                |    |                |  Win #3   |                  |
	|     Win #2     |             .                |    |      Win #2    |           |                  |
	|                |             .                |    |                |-----------|                  |
	|----------------|             .                |    |----------------|           .                  |
	|                              .                |    |                            .                  |
	|-----------------------------------------------|    |-----------------------------------------------|

```

The first option just takes the right-most point, and adds the width of window #3. This assures it'll fit. (there's also a test done to ensure that the bounding box we define isn't wider than the actual screen we're using; so for example in a 1920x1080 window, we'll never try to draw windows that go beyond 1920)
The second option goes through our point list, and finds the point which will allow us to expand the bounding box the least, while still fitting the window for both width and height. 
We choose the second option, and future versions of binpack will flag the choice to the users' preference outright. 

Two special cases here require further consideration:

```    
    |----------------|------|   |-------------|x|--|
    |                |      |   |             |x|  |
    |       #1       |      |   |             |x|  |
    |                |  #3  |   |     #1      |x|#3|
    |------------|---|      |   |             |x|  |
    |            |xxx|      |   |             |x|  |
    |     #2     |---|--|---|   |---------------|  |
    |            |  #4  |       |       #2      |--|
    |------------|------|       |---------------|

```

First window:
In the case of placing window #4, we test previous points for >= y && >= x than a point we're testing. An example where this would be true is the bottom lef
t corner of window #3 - we attempt to place with our point starting on the same y access as the bottom of #3. In this case it fits. We could even test point 2, and since it's a point on the same axis, we know there's something likely obstructing.

Second window:
First we test #3 to fit, against the secont point (which is the bottom right of #1). It doesn't fit, as #2 is blocking. We now test future points for >= x && >= y points. The top right of #2 meet both reqs, We use that same x axis to place window 3.
