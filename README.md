# Bin Packing Meets Window Management

## * CURRENTLY BROKEN *
The previous state this was left in shouldn't really be ran anywhere, as it's too intensive for sane use - so I've pushed the new design through to master. Please don't use this until 0.1 release.

This is a small project used in tandem with [hwwm](https://github.com/halfwit/hwwm) to lay out my windows in a way that I want
It uses a relaxed variant of the 2D Bin Pack, with a caveat that the rectangles have variable sizes.
The algorithm will try to lay out windows within the given space up to the max for each rectangle.

## Explanation

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
	|     Win #1       |                            |
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

The dots here represent a bounding-box. What the bounding box does, is define a sub-section of windows that we'll attempt to place more windows into. This is done for one simple reason: when we attempt to stack windows vertically first, the aesthetic is far better. This algorithm would work with a horizontal limit, but tends to work poorly when you have no bounding boxes at all; you get a stack of windows that looks like a staircase, and very silly. 

So when we attempt to place our second window here, it won't be able to fit into the space beside win #1; it has to go below it. (Note, the algorithm will attempt to put the window beside first; this fills up odd cases where you have a narrow window, underneath a rather wide window; you'd much rather have two narrow windows stacked beside eachother, than on top of eachother)

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

This will work for most all combinations of windows; but there are a few caveats which result in weird, overlapping layouts if not dealt with.

Two special cases here require further consideration:

```    
	|-------------------|-----|---------------------|   |------|
	|                   |     |                     |   |      |
	|                   |     |                     |   |Win #4|
	|                   |     |                     |   |      |
	|      Win #1       | Win |                     |   |------|
	|                   |  #3 |                     |
	|                   |     |                     | 
	|                   |     |                     |
	|----------------|--|     |                     |
	|                |xx|-----|                     |
	|      Win #2    |                              |
	|                |                              |
	|                |                              |
	|----------------|------------------------------|

```

Here, if we wanted to place a 4th window we'll run in to a problem. Since we use an array of points to define where we have windows, the top-right of Win #2 would mistakingly report it had ample space to place the window, resulting in overlap between Win #4 and Win #3. The obvious, right way is to have it beside Win #2, and below Win #3. To do this, we need to make sure we don't consider the space demarkated by the x's.

```
	|--------------------|-------|------------------|
	|                    |       |                  |
	|                    | Win #3|                  |
	|                    |       |                  |
	|      Win #1        |       |                  |
	|                    |--|----|                  |
	|                    |xx|    |                  |
	|                    |xx|Win |                  |
	|                    |xx| #4 |                  |
	|--------------------|--|    |                  |
	|                       |----|                  |
	|       Win #2          |                       |
	|                       |                       |
	|-----------------------|-----------------------|

```

Similar issue is seen here. As we attempt to place the 4th window, we'll run in to a problem of overlapping. We'll test the point where bottom left of window #3, and window #1 meet, and it'll seem to have ample vertical space to place the window. We need to make sure we don't consider the space demarkated by the x's again, but only in the case where a window won't fit inside them.

## Points

To address the above issues, the first variant of binpack kept a running tally of all windows, and explicitely checked all windows for collisions. That was expensive, slow, and full of edge cases. 
To attempt to mitigate this, we're going to use an array of points, each point denoting the bottom right edge of a window. Using these points, we'll be able to extrapolate the entire working face of windows we've packed, inspect whether there's collisions relatively cheaply, and finally completely remove the need to hold each window in memory for subsequent comparisons. 

* The above section will be updated, based on the success of the algorithm changes in practice.
