#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <assert.h>

typedef unsigned char byte;
#define ln(x) log((x))
#define UINT_64_FMT "%"PRIu64

static inline void print_num(uint64_t num)
{
	printf(UINT_64_FMT"\n", num);
}

// <bitset>
typedef struct bit_set {
	byte * bits;
	uint64_t bits_num;
	uint64_t bytes_num;
} bit_set;

static inline uint64_t bits_bytes(uint64_t bits)
{
	return (bits / CHAR_BIT) + ((bits % CHAR_BIT) != 0);
}

static inline void bitset_make(bit_set * b, uint64_t bits)
{
	b->bits_num = bits;
	b->bytes_num = bits_bytes(bits);
	assert((b->bits = calloc(b->bytes_num, sizeof(byte))));
}
static inline void bitset_free(bit_set * b)
{
	free(b->bits);
}
static inline void bitset_zero_out(bit_set * b)
{
	memset(b->bits, 0, b->bytes_num);
}
static inline bool bitset_get(const bit_set * b, uint64_t pos)
{
	uint64_t target = pos % CHAR_BIT;
	return (b->bits[pos/CHAR_BIT] & (1 << target)) >> target;
}
static inline void bitset_set(bit_set * b, uint64_t pos)
{
	b->bits[pos/CHAR_BIT] |= (1 << (pos % CHAR_BIT));
}
static inline uint64_t bitset_num_zeroes(const bit_set * b)
{
	uint64_t acc = 0;
	for (uint64_t i = 0, end = b->bits_num; i < end; ++i)
		acc += !bitset_get(b, i);
	return acc;
}
// <bitset>

// <array>
typedef struct narray {
	uint64_t * numbers;
	uint64_t size;
} narray;

void narr_make(narray * na, uint64_t len)
{
	assert((na->numbers = malloc(len*sizeof(*na))));
	na->size = len;
}
static inline void narr_free(narray * na)
{
	free(na->numbers);
}
static inline uint64_t * narr_data(narray * na)
{
	return na->numbers;
}
static inline uint64_t narr_len(narray * na)
{
	return na->size;
}
// </array>

// <sieve>
void sieve_of_Eratosthenes(narray * primes_out, uint64_t limit)
{
	bit_set primes_, * primes = &primes_;
	bitset_make(primes, limit+1);
	
	bitset_set(primes, 0);
	bitset_set(primes, 1);
	
	uint64_t max = (uint64_t)sqrt((double)limit);
	for (uint64_t p = 2; p <= max; ++p)
	{
		if (!bitset_get(primes, p))
		{
			for (uint64_t i = p*p; i <= limit; i += p)
				bitset_set(primes, i);
		}
	}
	
	narr_make(primes_out, bitset_num_zeroes(primes));
	
	uint64_t n = 0;
	uint64_t * data = narr_data(primes_out);
	for (uint64_t i = 0; i <= limit; ++i)
	{
		if (!bitset_get(primes, i))
			data[n++] = i;
	}
	
	bitset_free(primes);
}

void sieve_segmented(uint64_t start, uint64_t stop)
{	
	narray primes_out_, * primes_out = &primes_out_;
	uint64_t sqr_primes = (uint64_t)sqrt((double)stop);
	
	sieve_of_Eratosthenes(primes_out, sqr_primes);
	
	uint64_t prime = 0;
	uint64_t * first_primes = narr_data(primes_out);
	for (uint64_t i = 0, end = narr_len(primes_out); i < end; ++i)
	{
		prime = first_primes[i];
		if (prime >= start)
			print_num(prime);
	}
	
	bit_set primes_seg_, * primes_seg = &primes_seg_;
	bitset_make(primes_seg, sqr_primes+1);
	
	uint64_t low = (start > sqr_primes) ? start : sqr_primes;
	uint64_t high = low + sqr_primes;
	
	uint64_t window_start = 0;
	while (low < stop)
	{
		if (high >= stop)
			high = stop;
		
		for (uint64_t i = 0, end = narr_len(primes_out); i < end; ++i)
		{
			prime = first_primes[i];
			if ((window_start = low/prime * prime) < low)
				window_start += prime;
				
			for (uint64_t j = window_start; j < high; j += prime)
				bitset_set(primes_seg, j-low);
		}
		
		
		for (uint64_t i = 0, end = high-low; i < end; ++i)
		{
			if (!bitset_get(primes_seg, i))
				print_num(i+low);
		}
		
		low += sqr_primes;
		high += sqr_primes;
		bitset_zero_out(primes_seg);
	}
	
	bitset_free(primes_seg);
	narr_free(primes_out);
}
// </sieve>

// <options>
const char prog_name[] = "primes";
const char prog_ver[] = "1.0";
const char opt_help[] = "--help";
const char opt_ver[] = "--version";
const char opt_mem[] = "--memory";
const char opt_lim[] = "--limit";
const char opt_count[] = "--count";

#define print_use() printf("Use: %s [option] [start] <stop>\n", prog_name)
#define nl() putchar('\n')

void print_use_and_quit(void)
{
	print_use();
	printf("Try: %s --help\n", prog_name);
	exit(EXIT_FAILURE);
}

void print_help_and_quit(void)
{
printf("%s -- generates prime numbers in a range up to uint64_t max\n", prog_name);
print_use();
nl();
printf("If [start] is not given, or is less than 2, 2 is used.\n");
nl();
printf("Options:\n");
printf("   %s   - print the approx. number of primes from 2 up to <stop>\n", opt_count);
printf("   %s  - shows approx. how much memory is needed for the given range\n", opt_mem);
printf("   %s   - shows the max number limit\n", opt_lim);
printf("   %s    - this screen\n", opt_help);
printf("   %s - version info\n", opt_ver);

	exit(EXIT_SUCCESS);
}

void print_ver_and_quit(void)
{
	printf("%s %s\n", prog_name, prog_ver);
	exit(EXIT_SUCCESS);
}

void print_limit_and_quit(void)
{
	printf(UINT_64_FMT "\n", UINT64_MAX);
	exit(EXIT_SUCCESS);
}

static inline uint64_t num_of_primes(uint64_t n)
{
	double dn = (double)n;
	return (uint64_t)(dn/ln(dn));
}

void print_num_of_primes_and_quit(uint64_t n)
{
	printf(UINT_64_FMT"\n", num_of_primes(n));
	exit(EXIT_SUCCESS);
}

void print_mem_and_quit(uint64_t n)
{
#define PAD "8"
	uint64_t sqr_of_max = (uint64_t)sqrt((double)n);
	uint64_t approx_num_primes = num_of_primes(sqr_of_max);
	uint64_t bitset_bytes = bits_bytes(sqr_of_max+1);
	uint64_t first_primes_bytes = approx_num_primes * sizeof(uint64_t);
	
	double all = bitset_bytes + first_primes_bytes;
	printf("%-" PAD ".0f bytes\n", all);
	printf("%-" PAD ".2f kb\n", all/1024);
	printf("%-" PAD ".2f mb\n", all/1024/1024);
	
	exit(EXIT_SUCCESS);
#undef PAD
}
// </options>

// <main>
#define pop_arg() --argc; ++argv
int main(int argc, char * argv[])
{
	bool check_mem = false;
	bool print_num_primes = false;
	uint64_t start = 0;
	uint64_t stop = 0;
	
	if (argc > 1)
	{
		if (strcmp(argv[1], opt_mem) == 0)
		{
			check_mem = true;
			pop_arg();
		}
		else if (strcmp(argv[1], opt_count) == 0)
		{
			print_num_primes = true;
			pop_arg();
		}
		else if (strcmp(argv[1], opt_help) == 0)
			print_help_and_quit();
		else if (strcmp(argv[1], opt_ver) == 0)
			print_ver_and_quit();
		else if (strcmp(argv[1], opt_lim) == 0)
			print_limit_and_quit();
	}
	
	if (!((argc == 2 && sscanf(argv[1], UINT_64_FMT, &stop) == 1) ||
		(argc == 3 && sscanf(argv[1], UINT_64_FMT, &start) == 1 &&
			sscanf(argv[2], UINT_64_FMT, &stop) == 1)))
	{
		print_use_and_quit();
	}
	
	if (print_num_primes)
		print_num_of_primes_and_quit(stop);
	
	if (check_mem)
		print_mem_and_quit(stop);
	
	if (start < 2)
		start = 2;
		
	sieve_segmented(start, stop);
	return 0;
}
// </main>
