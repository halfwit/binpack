/* Simple bin packing algorithm implementation for use in window management */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

/* from -x -y -g flags */
struct mbin {
	unsigned x, y, gaps;
};

struct input {
	unsigned min_w, min_h, max_w, max_h;
	unsigned long wid;
};

struct output {
	unsigned x, y, w, h;
	unsigned long wid;
};

/* temporary storage of available sub-bins */
struct bins {
	unsigned x, y, w, h;
};

size_t 
create_rect(struct input r[]) 
{
	size_t length = 0;
	char line[50] = "";
	
	for (unsigned i = 0; fgets(line, sizeof(line), stdin); ++i) {
		sscanf(line, "%d %d %d %d %lx", &r[i].min_w, &r[i].min_h, &r[i].max_w, &r[i].max_h, &r[i].wid);
		length++;
	}
	
	return length;
}

/* (max bin size - blob size) / 2 */
void 
center(const size_t length, struct output r[], struct mbin mb)
{
	unsigned w = 0, h = 0;
	
	for (size_t i = 0; i < length; i++) {
		if (w < (r[i].w + r[i].x))
			w = r[i].w + r[i].x;
		if (h < (r[i].h + r[i].y))
			h = r[i].h + r[i].y;
	}
	for (size_t i = 0; i < length; i++) {
		r[i].x += (mb.x - w) / 2;
		r[i].y += (mb.y - h) / 2;
	}
}

/* given set of bins, arrange them smallest to largest w*h */
void
sort_bins(struct bins b[], size_t *bin_count)
{
	struct bins temp;
	
	for (unsigned i = 1; i < *bin_count; i++) {
		for (unsigned j = 0; j < *bin_count - i; j++) {
			if ((b[j + 1].w * b[j + 1].h) > (b[j].w * b[j].h)) {
				temp = b[j];
				b[j] = b[j + 1];
				b[j + 1] = temp;
			}
		}
	}
}

/* arrange rectangles largest to smallest, normalized some over min/max */
void
sort_input(struct input r[], const size_t length)
{
	struct input temp;

	for (size_t i = 1; i < length; i++) {
		for (size_t j = 0; j < length - i; j++) {
			if ((r[j + 1].max_w * r[j + 1].min_h) > (r[j].max_w * r[j].min_h)) {
				temp = r[j];
				r[j] = r[j + 1];
				r[j + 1] = temp;
			}
		}
	}
}

void
create_bins(struct bins bin[], struct output out[], size_t i, size_t j, size_t *bin_count, struct mbin mb)
{
	/* New bins based on subsection of old  */
	unsigned x, y, w, h;
	x = bin[j].x;
	y = bin[j].y;
	w = bin[j].w;
	h = bin[j].h;

	/* rect smaller, make two sub bins */
	if (out[i].h + mb.gaps < h && out[i].w + mb.gaps < w) {
		bin[*bin_count] = (struct bins){x + out[i].w + mb.gaps, y, w - out[i].w - mb.gaps, h};
		bin[j].y += (out[i].h + mb.gaps);
		bin[j].h -= (out[i].h - mb.gaps);
		*bin_count += 1;
	}

	/* rect same height */
	else if (out[i].h + mb.gaps < h) {
		bin[j].y += (out[i].h + mb.gaps);
		bin[j].h -= (out[i].h - mb.gaps);
	}

	/* rect same width */
	else if (out[i].w + mb.gaps < w) {
		bin[j].x += (out[i].w + mb.gaps);
		bin[j].w -= (out[i].w - mb.gaps);
	}

	/* rect fills space, lose a bin */
	else {
		bin[j].w = 0;
		bin[j].h = 0;
		*bin_count -= 1;
	}
}

void 
save_rect(struct bins bin[], struct output out[], struct input r[], size_t i, size_t j, struct mbin mb)
{
	/* Store rect x y w h wid */
	out[i] = (struct output){bin[j].x + (mb.gaps / 2), bin[j].y + (mb.gaps / 2), r[i].min_w, r[i].min_h, r[i].wid};
}

bool 
pack_bin(struct output out[], struct input r[], const size_t length, unsigned *bin_width, unsigned *bin_height, struct mbin mb)
{
	/* Main algorithm */
	struct bins bin[100];
	for (int i = 0; i < 100; i++) {
		bin[i] = (struct bins){0, 0, 0, 0};
	}

	/* default bin */
    bin[0] = (struct bins){ 0, 0, *bin_width + mb.gaps, *bin_height + mb.gaps};
	size_t bin_count = 1;
	bool rect_fits;

	/* loop through each rect */
	for (size_t i = 0; i < length; i++) {
		rect_fits = false;

		/* loop through each bin */
		for (size_t j = 0; j < bin_count; j++) {
			/* rect fits in current bin */
			if (r[i].min_w + mb.gaps <= bin[j].w && r[i].min_h + mb.gaps <= bin[j].h) {
				rect_fits = true;
				save_rect(bin, out, r, i, j, mb);
				create_bins(bin, out, i, j, &bin_count, mb);
				sort_bins(bin, &bin_count);
				break;
			}
		}
		
		/* Grow main bin if possible */
		if (rect_fits == false) {
			if (mb.x > *bin_width)
				*bin_width += 2;
			if (mb.y > *bin_height)
				*bin_height += 2;
			return true;
		}
	}
	return false;
}

bool
resize(struct output out[], struct input r[], const size_t length, unsigned *bin_width, unsigned *bin_height, struct mbin mb, size_t index)
{
	/* When a bin fills all the space available, pack_bin */
	for (size_t i = index; i < length; i++) {
		unsigned limitcheck = 0;
		while (pack_bin(out, r, length, bin_width, bin_height, mb)) {
			if (limitcheck == mb.x)
				return false;
			limitcheck++;
		}

		/* If rect can grow */
		if (out[i].h < r[i].max_h)
			r[i].min_h += 1;
		if (out[i].w < r[i].max_w)
			r[i].min_w += 1;

		sort_input(r, length);
	}

	/* max_h to handle cases of smaller windows */
	unsigned w = 0, h = 0;

	for (size_t i = index; i < length; i++) {
		if (w < (out[i].w + out[i].x))
			w = out[i].w + out[i].x + mb.gaps;
		if (h < (out[i].h + out[i].y))
			h = out[i].h + out[i].y + mb.gaps;
	}

	return ((h < mb.y + (mb.gaps / 2)) && (w < mb.x + (mb.gaps / 2)));
	/*
	if (h >= mb.y + (mb.gaps / 2))
		return false;

	if (w >= mb.x + (mb.gaps / 2))
		return false;

	return true;
	*/
}

int
main(int argc, char *argv[])
{
	struct mbin mb;
	mb.gaps = 0;
	int c;
	while ((c = getopt(argc, argv, "hg:x:y:")) != -1) {
		switch (c) {
			/* read in, all vars unsigned int */
			case 'g': {
				sscanf(optarg, "%u", &mb.gaps);
				break;
			}
			case 'x': {
				sscanf(optarg, "%u", &mb.x);
				break;
			}
			case 'y': {
				sscanf(optarg, "%u", &mb.y);
				break;
			}
			case 'h': {
				fputs("Usage: bin_pack -x screen_width -y screen_height -g gaps\n", stderr);
				return EXIT_SUCCESS;
			}
		}
	}

	struct input r[100];
	struct output out[100];

	const size_t length = (create_rect(r));

	/* If we have no windows, exit */
	if (length == 0) {
		return EXIT_SUCCESS;
	}

	sort_input(r, length);

	unsigned bin_width = mb.x;
	unsigned bin_height = mb.y;

	/* If we have no large windows */
	bool no_large_windows = true;
	for (size_t i = 0; i < length; i++) {
		struct input temp = r[i];
		r[i].min_w = r[i].max_w;
		r[i].min_h = r[i].max_h;

		if (pack_bin(out, r, length, &bin_width, &bin_height, mb)) {
			r[i] = temp;
			no_large_windows = false;
		}
	}

	unsigned limitcheck = 0;

	if (no_large_windows == false) {
		bin_width = r[0].min_w;
		bin_height = r[0].min_h;

		while (pack_bin(out, r, length, &bin_width, &bin_height, mb)) {
			/* if we've ran this long, something is up */
			if (limitcheck == mb.x)
				return EXIT_FAILURE;

			limitcheck++;
		}

		limitcheck = 0;

		for (size_t i = 0; i < length; i++) {
			/* Square out the blob as best as we can */
			while (resize(out, r, length, &bin_width, &bin_height, mb, i)) {
				if (limitcheck == mb.x)
					return EXIT_FAILURE;

				limitcheck++;
			}
		}
	}

	center(length, out, mb);

	for (size_t i = 0; i < length; i++) {
		printf("%d %d %d %d %lx\n", out[i].x, out[i].y, out[i].w, out[i].h, out[i].wid);
	}
	
	return EXIT_SUCCESS;
}
