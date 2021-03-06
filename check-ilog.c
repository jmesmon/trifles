#include <math.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>

#include <penny/math.h>

#define __u64 uint64_t
#define __u32 uint32_t
#include <xfs/bitops.h> /* fls & fls64 */

static uint32_t log2_3(uint32_t v)
{
	const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
	const unsigned int S[] = {1, 2, 4, 8, 16};
	int i;

	unsigned int r = 0; // result of log2(v) will go here
	for (i = 4; i >= 0; i--) // unroll for speed...
	{
		if (v & b[i])
		{
			v >>= S[i];
			r |= S[i];
		} 
	}

	return r;
}

static uintmax_t
slow_log2(uintmax_t x)
{
	uintmax_t i = 0;
	while (x >>= 1)
		i++;
	return i;
}

static uintmax_t
fast_fls(uintmax_t x)
{
	return sizeof(unsigned long long) * CHAR_BIT - __builtin_clzll(x);
}

int
main(void)
{
	uintmax_t e = 0;
	uintmax_t i = 1;

	do {
		uintmax_t f1 = ((uintmax_t)floorl(log2l(i)));
		uintmax_t c  = ceill(log2l(i + 1)) - 1;
		uintmax_t il = fast_fls(i) - 1;
		uintmax_t ils = slow_log2(i);
		uintmax_t flsx = fls64(i) - 1;
		uintmax_t flsp = p_fls(i) - 1;

#if 0
		printf("<%5ju> f1=%2ju c=%2ju il=%2ju ils=%2ju flsx=%2ju flsp=%2ju\n",
				i, f1, c, il, ils, flsx, flsp);
#endif

		if (f1 != c || il != f1 || f1 != ils || ils != flsx || flsx != flsp) {
			fprintf(stderr, "MISMATCH: %5ju: f1=%2ju c=%2ju il=%2ju ils=%2ju flsx=%2ju flsp=%2ju\n",
					i, f1, c, il, ils, flsx, flsp);
			e++;
			if (e > 10)
				return -1;
		}
	} while
#if 0
	(i && (i <<= 1) < UINTMAX_MAX)
#else
	(i++ < UINTMAX_MAX)
#endif
	;

	return 0;
}
