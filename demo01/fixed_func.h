/*
 *
 */

#ifndef FIXEDP_FUNC_H_INCLUDED
#define FIXEDP_FUNC_H_INCLUDED

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _MSC_VER
#include <stdint.h>
#else
#include "stdint.h"
#endif

namespace fixedpoint {

// The template argument p in all of the following functions refers to the
// fixed point precision (e.g. p = 8 gives 24.8 fixed point functions).

// Perform a fixed point multiplication without a 64-bit intermediate result.
// This is fast but beware of overflow!
template <int p>
inline int32_t fixmulf(int32_t a, int32_t b)
{
	return (a * b) >> p;
}

// Perform a fixed point multiplication using a 64-bit intermediate result to
// prevent overflow problems.
template <int p>
inline int32_t fixmul(int32_t a, int32_t b)
{
	return (int32_t)(((int64_t)a * b) >> p);
}

// Fixed point division
template <int p>
inline int fixdiv(int32_t a, int32_t b)
{
#if 0
	return (int32_t)((((int64_t)a) << p) / b);
#else
	// The following produces the same results as the above but gcc 4.0.3
	// generates fewer instructions (at least on the ARM processor).
	union {
		int64_t a;
		struct {
			int32_t l;
			int32_t h;
		};
	} x;

	x.l = a << p;
	x.h = a >> (sizeof(int32_t) * 8 - p);
	return (int32_t)(x.a / b);
#endif
}

namespace detail {
	inline uint32_t CountLeadingZeros(uint32_t x)
	{
		uint32_t exp = 31;

		if (x & 0xffff0000) {
			exp -= 16;
			x >>= 16;
		}

		if (x & 0xff00) {
			exp -= 8;
			x >>= 8;
		}

		if (x & 0xf0) {
			exp -= 4;
			x >>= 4;
		}

		if (x & 0xc) {
			exp -= 2;
			x >>= 2;
		}

		if (x & 0x2) {
			exp -= 1;
		}

		return exp;
	}
}

// q is the precision of the input
// output has 32-q bits of fraction
template <int q>
inline int fixinv(int32_t a)
{
	int32_t x;

	bool sign = false;

	if (a < 0) {
		sign = true;
		a = -a;
	}

	static const uint16_t rcp_tab[] = {
		0x8000, 0x71c7, 0x6666, 0x5d17, 0x5555, 0x4ec4, 0x4924, 0x4444
	};

	int32_t exp = detail::CountLeadingZeros(a);
	x = ((int32_t)rcp_tab[(a>>(28-exp))&0x7]) << 2;
	exp -= 16;

	if (exp <= 0)
		x >>= -exp;
	else
		x <<= exp;

	/* two iterations of newton-raphson  x = x(2-ax) */
	x = fixmul<(32-q)>(x,((2<<(32-q)) - fixmul<q>(a,x)));
	x = fixmul<(32-q)>(x,((2<<(32-q)) - fixmul<q>(a,x)));

	if (sign)
		return -x;
	else
		return x;
}

// Conversion from and to float

template <int p>
float fix2float(int32_t f)
{
	return (float)f / (1 << p);
}

template <int p>
int32_t float2fix(float f)
{
	return (int32_t)(f * (1 << p));
}

int32_t fixcos16(int32_t a);
int32_t fixsin16(int32_t a);
int32_t fixrsqrt16(int32_t a);
int32_t fixsqrt16(int32_t a);

} // end namespace fixedpoint

#endif
