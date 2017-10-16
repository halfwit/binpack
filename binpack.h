#include <stdbool.h>

#define MAX_BIN 64

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

void bin_pack(unsigned width, unsigned height, struct Output out[], struct Input in[]);
void center(unsigned width, unsigned height, struct Output out[], unsigned gaps);
void sort_bins(struct Input r[], const size_t length);
size_t init_bins(struct Input r[]);
