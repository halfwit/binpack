#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "binpack.h"

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
void binary_bin_pack(unsigned width, unsigned height, struct Output out[], struct Input in[], struct Points points[]) {
	
	unsigned count = sizeof(*in)/sizeof(in[0]) + 1;

	// Initialise greedily to the maximums
	struct Current c[count];
	for (int i = 0; i < count; i++) {
		c[i].w = in[i].maxw;
		c[i].h = in[i].maxh;
		c[i].wid = in[i].wid;
	}

	while (1) {
		if (bin_pack(width, height, c, out, count, points)) {
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

void
set_bin(struct Output *out, struct Current c, unsigned x, unsigned y) {
	out->h = c.h;
	out->w = c.w;
	out->x = x;
	out->y = y;
	out->wid = c.wid;
}

bool
set_point(struct Output *out, struct Current c, struct Points p[], unsigned *count, unsigned bounds, unsigned height) {
	unsigned top = 0;
	unsigned side = 0;
	// Loop through our points to try to fit
	for (unsigned i = 0; i < *count; i++) {
		// We could set sentinals here for possible, then collision and set
		bool potential = false;
		// Fits beside
		if (p[i].x + c.w <= bounds) {
			side = p[i].x;
			potential = true;
		} 
		else if (p[i].y + c.h <= height) {
			top = p[i].y;
			// If we're at the last point, side is 0; else our window starts at the greater of either the next point or current. 
			if ((i + 1) != *count) {
				side = (p[i + 1].x > p[i].x) ? p[i + 1].x : p[i].x;
			} else {
				side = 0;
			}
			potential = true;
			
		}
		if (!(potential)) {
			top = p[i].y;
			continue;
		}
		// There's a collision, try next point
		//if (collisions(c, side, top, p)) {
		//	continue;
		//}
		set_bin(out, c, side, top);
		return true;
	}
	return false;
} 

bool
bin_pack(unsigned width, unsigned height, struct Current c[], struct Output out[], unsigned count, struct Points p[]) {

	unsigned p_count = 1;

	// Set our initial data up
	p[0].x = c[0].w;
	p[0].y = c[0].h;
	set_bin(&out[0], c[0], 0, 0);
	
	// Set our initial bounding box
	unsigned bounds = c[0].w;

	// Loop through rest of windows
	for (unsigned i = 1; i < count; i++) {
		// Walk our points, check if we fit. If we fit, all is well; we can go on to the next
		if (set_point(&out[i], c[i], p, &p_count, bounds, height)) {
			p_count++;
			continue;
		}
		// Increase our bounds to fit, go from there.
		if (bounds + c[i].w <= width) {
			bounds += c[i].w;
			if (set_point(&out[i], c[i], p, &p_count, bounds, height)) {
				continue;
			}
		}
		// Binpack failed
		return false;
	}

	return true;
}
