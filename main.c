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
	printf("here %d\n", input[0].minw);
	
	/* Sanity checks */
	if (length == 0 || height == 0 || width == 0)
		return EXIT_SUCCESS;

	sort_bins(input, length);
	
	// Normal function	
	if (screens == 1) {
	   	binary_bin_pack(width, height, output, input);
	   	center(width, height, output, gaps);
		print_bin(output, length);
		return EXIT_SUCCESS;
	}
	// More than one screen will need a few more data types
	struct Input in_a[MAX_BIN/2], in_b[MAX_BIN/2];
	struct Output out_a[MAX_BIN/2], out_b[MAX_BIN/2];
		
	if (screens == 2) {
		split(input, in_a, in_b, length);
		binary_bin_pack(width/2, height, out_a, in_a);
		binary_bin_pack(width/2, height, out_b, in_b);
	  	center(width/2, height, out_a, gaps);
		center(width/2, height, out_b, gaps);
		offset(out_b, width/2);			
		print_bin(out_a, sizeof(out_a)/sizeof(out_a[0]));
		print_bin(out_b, sizeof(out_b)/sizeof(out_b[0]));
	 }

	if (screens == 3) {
		struct Input temp;
		bool bin_switch = true;
		// Ugly logic - while bin_pack fails, we pop out into flanking bins
		// bin_pack will pass if it has <= 1 element to keep logic clean
		while (!bin_pack(width/3, height, output, input)) {
			temp = pop(input);
			(bin_switch) ? push(in_a, temp) : push(in_b, temp);
			bin_switch = !bin_switch;
		}
		binary_bin_pack(width/3, height, out_a, in_a);
		binary_bin_pack(width/3, height, out_b, in_b);
		center(width/3, height, output, gaps);
		center(width/3, height, out_a, gaps);
		center(width/3, height, out_b, gaps);
		offset(output, width/3);
		offset(out_b, width*2/3);
		print_bin(output, sizeof(output)/sizeof(output[0]));
		print_bin(out_a, sizeof(out_a)/sizeof(out_a[0]));
		print_bin(out_b, sizeof(out_b)/sizeof(out_b[0]));
	}

}
