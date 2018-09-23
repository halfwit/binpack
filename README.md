# Bin Packing Meets Window Management

* CURRENTLY BROKEN *
The previous state this was left in shouldn't really be ran anywhere, as it's too intensive for sane use - so I've pushed the new design through to master. Please don't use this until 0.1 release.

This is a small project used in tandem with [hwwm](https://github.com/halfwit/hwwm) to lay out my windows in a way that I want
It uses a relaxed variant of the 2D Bin Pack, with a caveat that the rectangles have variable sizes.
The algorithm will try to lay out windows within the given space up to the max for each rectangle.

## Programming notes

We use a list of points to define a face, the face and bounding box that it bisects representing all available space to place subsequent windows (rectangles).

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
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|                                               |
	|-----------------------------------------------|
	Points are (18,0) (18,7) and (0,7)
```

This essentially creates 3 new points of interest in our available space. The top right, bottom right, and bottom left points of the window we just placed - we're going to keep track of these, and these alone.

```
	|------------------|----------------------------|
	|                  |                            |
	|                  |                            |
	|     Win #1       |                            |
	|                  |                            |
	|                  |                            |
	|----------------|-|                            |
	|                |                              |
	|                |                              |
	|     Win #2     |                              |
	|                |                              |
	|----------------|                              |
	|                                               |
	|-----------------------------------------------|
	Points are (18,0) (18,7) (16,7) (16,12) (0,12)
```

Notice we removed the point (0,7) - this requires explanation.

```
	|------------------|-----------|----------------|
	|                  |           |                |
	|                  |   Win #3  |                |
	|     Win #1       |           |                |
	|                  |-----------|                |
	|                  |                            |
	|----------------|-|                            |
	|                |                              |
	|                |                              |
	|     Win #2     |                              |
	|                |                              |
	|----------------|                              |
	|                                               |
	|-----------------------------------------------|

```

Placing window #3 would see that there is a point, (0,7) with enough space both horizontally and vertically to fit and overlap window #2. Previous versions of binpack checked for windows occupying the same space explicitely, but this is an expensive operation. using and updating an accurate list of applicable points is a means to an end to have a highly efficient packing algorithm. 

## Two special cases here require further consideration

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

### First window

In the case of placing window #4, we test previous points for >= y && >= x than a point we're testing. An example where this would be true is the bottom lef
t corner of window #3 - we attempt to place with our point starting on the same y access as the bottom of #3. In this case it fits. We could even test point 2, and since it's a point on the same axis, we know there's something likely obstructing.

### Second window

First we test #3 to fit, against the secont point (which is the bottom right of #1). It doesn't fit, as #2 is blocking. We now test future points for >= x && >= y points. The top right of #2 meet both reqs, We use that same x axis to place window 3.
