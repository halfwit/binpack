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

	// Set first window at (0,0)
	out[0].w = c[0].w;
	out[0].h = c[0].h;
	out[0].x = 0;
	out[0].y = 0;
	out[0].wid = c[0].wid;

	// And points
	points[0].x = c[0].w;
	points[0].y = 0;
	points[1].x = c[0].w;
	points[1].y = c[0].h;
	points[2].x = 0;
	points[2].y = c[0].h;

	// Loop through rest of windows

	for (unsigned i = 0; i < count; i++) {
		out[i].w = c[i].w;
		out[i].h = c[i].h;
		out[i].x = 0;
		out[i].y = 0;
		out[i].wid = c[i].wid;
	}

	return true;
}
