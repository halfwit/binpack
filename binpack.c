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
create_rect(struct input r[]) {
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
center(const size_t length, struct output r[], struct mbin mb) {
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

/* arrange rectangles smallest to largest w*h */
void
sort_bins(struct bins b[], size_t *bin_count) {
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

void
scrub_bins(struct bins b[], size_t *bin_count) {
	if (*bin_count == 1)
		return;
		
	for (unsigned i = 1; i < *bin_count; i++) {
		if(b[i].w == 0 || b[i].h == 0)
			continue;
		for (unsigned j = 0; j < *bin_count - 1; j++) {
			
			/* Skip if same bin */
			if (i == j)
				continue;
				
			/* Same row */
			if (b[j].y == b[i].y && b[j].h == b[i].h) {
				b[j].w += b[i].w;
				b[i].h = 0;
				b[i].w = 0;
			}
			
			/* Same column */
			if (b[j].x == b[i].x && b[j].w == b[i].w) {
				b[j].h += b[i].h;
				b[i].h = 0;
				b[i].w = 0;
			}
		}
	}	
}

/* arrange rectangles largest to smallest */
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
	/* Local variables */
	unsigned x, y, w, h;
	x = bin[j].x;
	y = bin[j].y;
	w = bin[j].w;
	h = bin[j].h;

	/* rect smaller, make two sub bins */
	if (out[i].h <= h && out[i].w <= w) {
		bin[*bin_count] = (struct bins){x + out[i].w + mb.gaps, y, w - out[i].w - mb.gaps, h};
		bin[j].y += (out[i].h + mb.gaps);
		bin[j].h -= (out[i].h - mb.gaps);
		*bin_count += 1;
	}
	
	/* rect same height */
	else if (out[i].h == h) {
		bin[j].y += (out[i].h + mb.gaps);
		bin[j].h -= (out[i].h - mb.gaps);
	}

	/* rect same width */
	else if (out[i].w == w) {
		bin[j].x += (out[i].w + mb.gaps);
		bin[j].w -= (out[i].w - mb.gaps);
	}

	/* rect fills space, lose a bin */
	else {
		bin[j].w = 0;
		bin[j].h = 0;
	}
}

void
save_rect(struct bins bin[], struct output out[], struct input r[], size_t i, size_t j, struct mbin mb)
{
	/* Store rect x y w h wid */
	out[i] = (struct output){bin[j].x + (mb.gaps / 2), bin[j].y + (mb.gaps / 2), (r[i].min_w + r[i].max_w) / 2, (r[i].min_h + r[i].max_h) / 2, r[i].wid};
}

bool
pack_bin(const size_t length, struct output out[], struct mbin mb, struct input r[])
{
	/* initialize bins */
	struct bins bin[100];
	for (int i = 0; i < 100; i++) {
		bin[i] = (struct bins){0, 0, 0, 0};
	}

	/* default bin */
	bin[0] = (struct bins){ 0, 0, mb.x, mb.y};
	size_t bin_count = 1;
	size_t packed = 0;

	/* loop through each rect */
	for (size_t i = 0; i < length; i++) {

		/* loop through each bin */
		for (size_t j = 0; j < bin_count; j++) {
			/* If rect fits bin, store it */
			if ((r[i].min_w + r[i].max_w) / 2 <= bin[j].w && (r[i].min_h + r[i].max_h) / 2 <= bin[j].h) {
				packed++;
				save_rect(bin, out, r, i, j, mb);
				create_bins(bin, out, i, j, &bin_count, mb);
				scrub_bins(bin, &bin_count);
				sort_bins(bin, &bin_count);
				break;
			}
		}
	}
	if (packed == length) {
		return true;
	}
	return false;
}

bool
resizewidth(const size_t length, struct output out[], struct mbin mb, struct input r[])
{
	size_t packed = 0;
	for (size_t i = 0; i < length; i++) {

		/* If we're within 1, bin is fully packed */
		if (r[i].max_w - r[i].min_w < 2) {
			packed++;
			continue;
		}
		
		/* In case something goes wonky */
		if (r[i].min_w > r[i].max_w) {
			r[i].max_w = r[i].min_w;
			r[i].min_w--;
		}

		/* if binpack succeeds, we can grow --> set min as current, else set max as current */
		if(pack_bin(length, out, mb, r)) {
			r[i].min_w = (r[i].min_w + r[i].max_w) / 2;
		} else {
			r[i].max_w = (r[i].min_w + r[i].max_w) / 2;
		}
	}

	if (packed == length) {
		return false;
	}
	return true;
}
bool
resizeheight(const size_t length, struct output out[], struct mbin mb, struct input r[])
{
	size_t packed_bin = 0;
	for (size_t i = 0; i < length; i++) {

		/* If we're within 1, bin is fully packed */
		if (r[i].max_h - r[i].min_h < 2) {
			packed_bin++;
			continue;
		}
		

		/* In case something goes wonky */
		if (r[i].min_h > r[i].max_h) {
			r[i].max_h = r[i].min_h;
			r[i].min_h--;
		}
		
		/* if binpack succeeds, we can grow --> set min as current, else set max as current */
		if(pack_bin(length, out, mb, r)) {
			r[i].min_h = (r[i].min_h + r[i].max_h) / 2;
		} else {
			r[i].max_h = (r[i].min_h + r[i].max_h) / 2;
		}
	}

	if (packed_bin == length) {
		return false;
	}
	return true;
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
		case 'g':
			sscanf(optarg, "%u", &mb.gaps);
			break;
		case 'x':
			sscanf(optarg, "%u", &mb.x);
			break;
		case 'y':
			sscanf(optarg, "%u", &mb.y);
			break;
		case 'h':
			fprintf(stderr, "Usage: %s -x screen_width -y screen_height -g gaps\n", argv[0]);
			return EXIT_SUCCESS;
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

	/* Initial pack to establish out bins */
	pack_bin(length, out, mb, r);

	int foo = 0;
	while(resizeheight(length, out, mb, r) && resizewidth(length, out, mb, r)) {
		printf("%d\n", ++foo);
	}

	unsigned area = 0;
	for (size_t i = 0; i < length; i++) {
		area+=out[i].w * out[i].h;
	}

	/* If we have more windows than space */
	if (area > (mb.x - mb.gaps) * (mb.y - mb.gaps))
		return EXIT_SUCCESS;

	center(length, out, mb);

	for (size_t i = 0; i < length; i++) {
		printf("%d %d %d %d %lx\n", out[i].x, out[i].y, out[i].w, out[i].h, out[i].wid);
	}

	return EXIT_SUCCESS;
}
