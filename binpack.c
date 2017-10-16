#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "binpack.h"

void center(int width, int height, struct Output out[], int gaps) {
	// r = rightmost edge of bins
	// d = bottommost edge of bins
	// Move all windows (width - r) / 2
	// Move all windows (height - d) / 2
	// g = gaps / 2
	// x - g, y + g, w - g, h - g for all windows 
}

int 
main(int argc, char* argv[]) {

	int gaps = 0;
	int width = 0;
	int height = 0;
	int screens = 1;
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

