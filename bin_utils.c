#include <stdio.h>
#include <stdlib.h>

#include "binpack.h"

void center(unsigned width, unsigned height, struct Output out[], unsigned gaps) {
	// r = rightmost edge of bins
	// d = bottommost edge of bins
	// Move all windows (width - r) / 2
	// Move all windows (height - d) / 2
	// g = gaps / 2
	// x - g, y + g, w - g, h - g for all windows 
}

void
sort_bins(struct Input r[], const size_t length) {
	/* arrange rectangles largest to smallest, normalized some over min/max */
	struct Input temp;
	for (size_t i = 1; i < length; i++) {
		for (size_t j = 0; j < length - i; j++) {
			if ((r[j + 1].maxw * r[j + 1].minh) > (r[j].maxw * r[j].minh)) {
				temp = r[j];
				r[j] = r[j + 1];
				r[j + 1] = temp;
			}
		}
	}
}

size_t
init_bins(struct Input r[]) {
	size_t length = 0;
	char line[MAX_BIN];
	for (unsigned i = 0; fgets(line, sizeof(line), stdin); ++i) {
		sscanf(line, "%d %d %d %d %lx", &r[i].minw, &r[i].minh, &r[i].maxw, &r[i].maxh, &r[i].wid
);
		length++;
	}
	return length;
}