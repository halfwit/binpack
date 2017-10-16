#include <stdbool.h>

struct Input {
	unsigned minw;
	unsigned minh;
	unsigned maxw;
	unsigned maxh;
	unsigned wid;
};

struct Output {
	unsigned w;
	unsigned h;
	unsigned x;
	unsigned y;
	unsigned id;
};

bool bin_pack(unsigned width, unsigned height, struct Output[], struct Input[]);
