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
	unsigned wid;
};

void binary_bin_pack(unsigned width, unsigned height, struct Output out[], struct Input in[]);
bool bin_pack(unsigned width, unsigned height, struct Output out[], struct Input in[]);
void split(struct Input in[], struct Input a[], struct Input b[], size_t length);
void center(unsigned width, unsigned height, struct Output out[], unsigned gaps);
void sort_bins(struct Input r[], const size_t length);
void print_bin(struct Output out[], size_t length);
void offset(struct Output out[], unsigned w);
size_t init_bins(struct Input r[]);
