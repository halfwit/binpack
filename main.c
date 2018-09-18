#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "binpack.h"

int main(int argc, char* argv[]) {

	unsigned gaps = 0;
	unsigned width = 0;
	unsigned height = 0;
	unsigned screens = 1;
	char c;
	while ((c = getopt(argc, argv, "hg:s:x:y:")) != -1) {
		switch (c) {
		case 'g':
			sscanf(optarg, "%u", &gaps);
	        break;
    	case 'x':
        	sscanf(optarg, "%u", &width);
           	break;
        case 'y':
        	sscanf(optarg, "%u", &height);
        	break;
    	case 's':
        	sscanf(optarg, "%u", &screens);
        	break;
    	case 'h':
			fputs("Usage: bin_pack -x screen_width -y screen_height -g gaps -s number_of_screens\n", stderr);
			return EXIT_SUCCESS;
        }
	}

	struct Input input[MAX_BIN];
	struct Output output[MAX_BIN];
	
	const size_t length = init_bins(input);
	
	/* Sanity checks */
	if (length == 0 || height == 0 || width == 0)
		return EXIT_SUCCESS;

	sort_bins(input, length);

	if (screens > 3) {
		//usage;
		return EXIT_FAILURE;
	
	/* Normal function */	
	if (screens == 1) {
		/* binpack.c */
	   	binary_bin_pack(width, height, output, input);

		/* bin_utils.c */
	   	center(width, height, output, gaps);
		print_bin(output, length);
		return EXIT_SUCCESS;
	}
	
	/* More than one screen will need a few more data types */
	struct Input in_a[MAX_BIN/2 + 1], in_b[MAX_BIN/2 + 1];
	struct Output out_a[MAX_BIN/2 + 1], out_b[MAX_BIN/2 + 1];
	
	/* Sort into two bins and pack each seperately */
	if (screens == 2) {
		/* binpack.c */
		split(input, in_a, in_b, length);
		binary_bin_pack(width/2, height, out_a, in_a);
		binary_bin_pack(width/2, height, out_b, in_b);
	  	
		/* bin_utils.c */
		center(width/2, height, out_a, gaps);
		center(width/2, height, out_b, gaps);
		offset(out_b, width/2);			
		print_bin(out_a, sizeof(out_a)/sizeof(out_a[0]));
		print_bin(out_b, sizeof(out_b)/sizeof(out_b[0]));
	 }


	// TODO: Implement triple head sorting and splitting
	/*
	if (screens == 3) {
		bool bin_switch = true;

		struct Current temp;
		struct Current c[MAX_BIN];
		unsigned count = sizeof(input)/sizeof(input[0]);
		for (unsigned i = 0; i < count; i++) {
			c[i].w = input[i].maxw; 
			c[i].h = input[i].maxh;
			c[i].wid = input[i].wid;
		}
		
		// binpack.c 
		while (!bin_pack(width/3, height, c, output, count)) {
			temp = pop(c);
			(bin_switch) ? push(in_a, temp) : push(in_b, temp);
			bin_switch = !bin_switch;
		}

		// TODO: Guard against empty structs
		binary_bin_pack(width/3, height, out_a, in_a);
		binary_bin_pack(width/3, height, out_b, in_b);
	
		// bin_utils.c
		center(width/3, height, output, gaps);
		center(width/3, height, out_a, gaps);
		center(width/3, height, out_b, gaps);
		offset(output, width/3);
		offset(out_b, width*2/3);
		print_bin(output, sizeof(output)/sizeof(output[0]));
		print_bin(out_a, sizeof(out_a)/sizeof(out_a[0]));
		print_bin(out_b, sizeof(out_b)/sizeof(out_b[0]));
		
	}*/

}
