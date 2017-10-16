#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "binpack.h"

#define MAX_BIN 64

void center(unsigned width, unsigned height, struct Output out[], unsigned gaps) {
	// r = rightmost edge of bins
	// d = bottommost edge of bins
	// Move all windows (width - r) / 2
	// Move all windows (height - d) / 2
	// g = gaps / 2
	// x - g, y + g, w - g, h - g for all windows 
}

void
sort_input(struct Input r[], const size_t length)
{
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
input_read(struct Input r[]) {
	size_t length = 0;
	char line[MAX_BIN];
	for (unsigned i = 0; fgets(line, sizeof(line), stdin); ++i) {
		sscanf(line, "%d %d %d %d %lx", &r[i].minw, &r[i].minh, &r[i].maxw, &r[i].maxh, &r[i].wid
);
		length++;
	}
	return length;
}

int main(int argc, char* argv[]) {

	unsigned gaps = 0;
	unsigned width = 0;
	unsigned height = 0;
	unsigned screens = 1;
	char c;
	
	while ((c = getopt(argc, argv, "hg:w:h:s:")) != -1) {
		switch (c) {
		case 'g':
			sscanf(optarg, "%u", gaps);
	        break;
    	case 'x':
        	sscanf(optarg, "%u", width);
           	break;
        case 'y':
        	sscanf(optarg, "%u", height);
        	break;
    	case 's':
        	sscanf(optarg, "%u", screens);
        	break;
    	case 'h':
			fputs("Usage: bin_pack -x screen_width -y screen_height -g gaps -s number_of_screens\n", stderr);
			return EXIT_SUCCESS;
        }
	}

	struct Input r[MAX_BIN];
	struct Output out[MAX_BIN];
	
	const size_t length = input_read(r);
	
	/* Sanity checks */
	if (length == 0 || height == 0 || width == 0)
		return EXIT_SUCCESS;

	sort_input(r, length);

	switch (screens) {
		case 1:	   		
//	   		bin_pack(width, height, *output, *input);
//	   		center(width, height, *output, gaps);
	   		break;

		case 2:
//	   		split total width into two 'bins'
//	   		split into two Input, a and b
//	   		bin_pack a and b
//	  		center a and b
//	  		add $width to all bins in b (shift to the right fro second monitor)
	  		break;
		case 3:
//	   		split total width into three 'bins'
//	   		bin_pack a
//	    	- if bin_pack fails, split out second window into bin b
//			- sanity check that there is infact a second window
//	    	- all subsequent fails will split out second window into bin b
//	   		once success, split b into two Input, b and c
//	   		bin_pack b and c
//	   		center a b and c
//	   		add $width to everything in b, and 2 * $width to everything in c
			break;
	}
//	print everything to stdout
}

