#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "binpack.h"
#include "points.h"

// Test to see if any bins are already at minimum size, so we can fail gracefully (Insufficient space to draw windows)
bool isminw(struct Input in[], struct Current c[], unsigned count) {
	for (unsigned i = 0; i < count; i++) {
		if (in[i].minw >= (c[i].w - 1)) {
			return true;
		} 
	}
	return false;
}

bool isminh(struct Input in[], struct Current c[], unsigned count) {
	for (unsigned i = 0; i < count; i++) {
		if (in[i].minh >= (c[i].h - 1)) {
			return true;
		} 
	}
	return false;
}
// Test to see if any bins are at our max size after binpack to exit binary loop
bool ismaxw(struct Input in[], struct Current c[], unsigned count) {
	for (unsigned i = 0; i < count; i++) {
		if (in[i].maxw <= (c[i].w + 1)) {
			return true;
		}
	}
	return false;
}

bool ismaxh(struct Input in[], struct Current c[], unsigned count) {
	for (unsigned i = 0; i < count; i++) {
		if (in[i].maxh <= (c[i].h + 1)) {
			return true;
		}
	}
	return false;
}

// This will iterate through a binary-search style approach, varying the size between the minw/minh and maxw/maxh to try to find the largest set of rectangles to fill the bin
void binary_bin_pack(unsigned width, unsigned height, struct Output out[], struct Input in[]) {
	
	unsigned count = sizeof(*in)/sizeof(in[0]) + 1;

	// Initialise greedily to the maximums
	struct Current c[count];
	for (int i = 0; i < count; i++) {
		c[i].w = in[i].maxw;
		c[i].h = in[i].maxh;
		c[i].wid = in[i].wid;
	}

	while (1) {
		if (bin_pack(width, height, c, out, count)) {
			if (ismaxw(in, c, count) && ismaxh(in, c, count)) {
				break;
			}
			/* Example
			minw 450 maxw 500 c.w 450
			c.w += 500 - 450 / 2
			c.w += 50 / 2
			w.c = 475 maxw = 500 minw = 475
			*/
			for (int i = 0; i < count; i++) {
				in[i].minw = c[i].w;
				in[i].minh = c[i].h;
				c[i].w += ((in[i].maxw - c[i].w) / 2); 
				c[i].h += ((in[i].maxh - c[i].h) / 2);
			}
			
		} else {
			if (isminw(in, c, count) && isminh(in, c, count)) {
				break;
			}
			/* Example
			minw 450 maxw 500 c.w 500
			c.w -= 500 - 450 / 2
			c.w -= 50 / 2
			c.w = 475 maxw = 500, minw = 450
			*/
			for (int i = 0; i < count; i++) {
				in[i].maxw = c[i].w;
				in[i].maxh = c[i].h;
				c[i].w -= ((c[i].w - in[i].minw) / 2);
				c[i].h -= ((c[i].h - in[i].minh) / 2);
			}
		}
	}
}

bool
bin_pack(unsigned width, unsigned height, struct Current c[], struct Output out[], unsigned count) {

/*

	Two special cases here require further point
	negotiation.
	|----------------|------|	|-------------|x|--|
	|                |      |	|             |x|  |
	|       #1       |      |	|             |x|  |
	|                |  #3  |	|     #1      |x|#3|
	|------------|---|      |	|             |x|  |
	|            |xxx|      |	|             |x|  |
	|     #2     |---|--|---|	|---------------|  |
	|            |  #4  |    	|       #2      |--|
	|------------|------|    	|---------------|
	
	# First window
	In the case of placing window #4, we test previous points for >= y && >= x than a point we're testing. An example where this would be true is the bottom left corner of window #3 - we attempt to place with our point starting on the same y access as the bottom of #3. In this case it fits.
	We could even test point 2, and since it's a point on the same axis, we know there's something likely obstructing.

	# Second window
	First we test #3 to fit, against the secont point (which is the bottom right of #1). It doesn't fit, as #2 is blocking.
	We now test future points for >= x && >= y points. The top right of #2 meets both reqs, We use that same x axis to place window 3.

*/

	for (unsigned i = 0; i < count; i++) {
		out[i].w = c[i].w;
		out[i].h = c[i].h;
		out[i].x = 0;
		out[i].y = 0;
		out[i].wid = c[i].wid;
	}

	return true;
}
