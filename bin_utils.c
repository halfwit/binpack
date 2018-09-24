#include <stdio.h>
#include <stdlib.h>

#include "binpack.h"

void
scrub_output(struct Output out[]) {
	for (unsigned i = 0; i < sizeof(*out)/sizeof(out[0]); i++) {
		out[i].w = 0;
		out[i].h = 0;
		out[i].x = 0;
		out[i].y = 0;
		out[i].wid = 0;
	}
}

struct Current
pop(struct Current c[]) {
	unsigned offset = sizeof(*c)/sizeof(c[0]) - 1;
	struct Current tmp;
	tmp.h = c[offset].h;
	tmp.w = c[offset].w;
	tmp.wid = c[offset].wid;
	c[offset] = c[offset + 1];
	return tmp;
}

void
push(struct Input in[], struct Current temp) {
	unsigned offset = sizeof(*in)/sizeof(in[0]);
	in[offset].maxw = temp.w;
	in[offset].maxh = temp.h;
	in[offset].minw = temp.w;
	in[offset].minh = temp.h;
	in[offset].wid = temp.wid;
}

// center all windows on given screen
void center(unsigned width, unsigned height, struct Output out[], unsigned gaps) {
	unsigned edge = 0;
	unsigned bott = 0;
	unsigned g = gaps / 2;
	// Find our edges
	for (size_t i = 0; i <= sizeof(*out)/sizeof(out[0]); i++) {
		if ((out[i].x + out[i].w) > edge) {
			edge = (out[i].x + out[i].w);
		}
		if ((out[i].y + out[i].h) > bott) {
			bott = (out[i].y + out[i].h);
		}
	}
	// Bump over
	for (size_t i = 0; i <= sizeof(*out)/sizeof(out[0]); i++) {
		out[i].x += ((width - edge) / 2);
		out[i].y += ((height - bott) / 2);
		out[i].x -= g;
		out[i].y += g;
		out[i].w -= g;
		out[i].h -= g;
	}
}

void sort_bins(struct Input r[], const size_t length) {
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

// read from stdin, setting up our initial data
size_t init_bins(struct Input r[], unsigned gaps) {
	size_t length = 0;
	unsigned g = (gaps / 2);
	char line[MAX_BIN];
	for (unsigned i = 0; fgets(line, sizeof(line), stdin); ++i) {
		sscanf(line, "%u %u %u %u %X", &r[i].minw, &r[i].minh, &r[i].maxw, &r[i].maxh, &r[i].wid);
		r[i].maxw += g;
		r[i].maxh += g;
		r[i].minw += g;
		r[i].minh += g;
		length++;
	}
	return length;
}

// given in[], seperate by even and odd into a[] and b[]
void split(struct Input *in, struct Input *a, struct Input *b, size_t length) {
	bool bin_switch = true;
	for (size_t i = 0; i < length; i++) {
		if (bin_switch) {
			a[i/2].minw = in[i].minw;
			a[i/2].minh = in[i].minh;
			a[i/2].maxw = in[i].maxw;
			a[i/2].maxh = in[i].maxh;
			a[i/2].wid  = in[i].wid;
		} else {
			b[i/2].minw = in[i].minw;
			b[i/2].minh = in[i].minh;
			b[i/2].maxw = in[i].maxw;
			b[i/2].maxh = in[i].maxh;
			b[i/2].wid  = in[i].wid;
		}
		bin_switch = !bin_switch;
	}
}

// Add w width to each out[n].x (moving the window on to the next monitor)
void offset(struct Output out[], unsigned w) {
	for (size_t i = 0; i < sizeof(*out)/sizeof(out[0]); i++) {
		out[i].x += w;
	}
}

// Loop through a given bin, printing all to stdout if values are present
void print_bin(struct Output out[], size_t length) {
	for (size_t i=0; i < length; i++) {
		if (out[i].w > 0 && out[i].h > 0) {
			printf("%u %u %u %u 0x%.8X\n", out[i].x, out[i].y, out[i].w, out[i].h, out[i].wid);
		}
    }
}

