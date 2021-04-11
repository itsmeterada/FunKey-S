/*
 *
 */

#ifndef FIXEDP_CLASS_H_INCLUDED
#define FIXEDP_CLASS_H_INCLUDED

#ifdef _MSC_VER
#pragma once
#endif

#include "fixed_func.h"

namespace fixedpoint {

// The template argument p in all of the following functions refers to the
// fixed point precision (e.g. p = 8 gives 24.8 fixed point functions).

template <int p>
struct fixed_point {
	int32_t intValue;

	fixed_point() {}
	/*explicit*/ fixed_point(int32_t i) : intValue(i << p) {}
	/*explicit*/ fixed_point(float f) : intValue(float2fix<p>(f)) {}
	/*explicit*/ fixed_point(double f) : intValue(float2fix<p>((float)f)) {}

	fixed_point& operator += (fixed_point r) { intValue += r.intValue; return *this; }
	fixed_point& operator -= (fixed_point r) { intValue -= r.intValue; return *this; }
	fixed_point& operator *= (fixed_point r) { intValue = fixmul<p>(intValue, r.intValue); return *this; }
	fixed_point& operator /= (fixed_point r) { intValue = fixdiv<p>(intValue, r.intValue); return *this; }

	fixed_point& operator *= (int32_t r) { intValue *= r; return *this; }
	fixed_point& operator /= (int32_t r) { intValue /= r; return *this; }

	fixed_point operator - () const { fixed_point x; x.intValue = -intValue; return x; }
	fixed_point operator + (fixed_point r) const { fixed_point x = *this; x += r; return x;}
	fixed_point operator - (fixed_point r) const { fixed_point x = *this; x -= r; return x;}
	fixed_point operator * (fixed_point r) const { fixed_point x = *this; x *= r; return x;}
	fixed_point operator / (fixed_point r) const { fixed_point x = *this; x /= r; return x;}

	bool operator == (fixed_point r) const { return intValue == r.intValue; }
	bool operator != (fixed_point r) const { return !(*this == r); }
	bool operator <  (fixed_point r) const { return intValue < r.intValue; }
	bool operator >  (fixed_point r) const { return intValue > r.intValue; }
	bool operator <= (fixed_point r) const { return intValue <= r.intValue; }
	bool operator >= (fixed_point r) const { return intValue >= r.intValue; }

	fixed_point operator + (int32_t r) const { fixed_point x = *this; x += r; return x;}
	fixed_point operator - (int32_t r) const { fixed_point x = *this; x -= r; return x;}
	fixed_point operator * (int32_t r) const { fixed_point x = *this; x *= r; return x;}
	fixed_point operator / (int32_t r) const { fixed_point x = *this; x /= r; return x;}
};

// Specializations for use with plain integers
template <int p>
inline fixed_point<p> operator + (int32_t a, fixed_point<p> b)
{ return b + a; }

template <int p>
inline fixed_point<p> operator - (int32_t a, fixed_point<p> b)
{ return -b + a; }

template <int p>
inline fixed_point<p> operator * (int32_t a, fixed_point<p> b)
{ return b * a; }

template <int p>
inline fixed_point<p> operator / (int32_t a, fixed_point<p> b)
{ fixed_point<p> r(a); r /= b; return r; }

// math functions
// no default implementation

template <int p>
inline fixed_point<p> sin(fixed_point<p> a);

template <int p>
inline fixed_point<p> cos(fixed_point<p> a);

template <int p>
inline fixed_point<p> sqrt(fixed_point<p> a);

template <int p>
inline fixed_point<p> rsqrt(fixed_point<p> a);

template <int p>
inline fixed_point<p> inv(fixed_point<p> a);

template <int p>
inline fixed_point<p> abs(fixed_point<p> a)
{
	fixed_point<p> r;
	r.intValue = a.intValue > 0 ? a.intValue : -a.intValue;
	return r;
}

// specializations for 16.16 format

template <>
inline fixed_point<16> sin(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixsin16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> cos(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixcos16(a.intValue);
	return r;
}


template <>
inline fixed_point<16> sqrt(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixsqrt16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> rsqrt(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixrsqrt16(a.intValue);
	return r;
}

template <>
inline fixed_point<16> inv(fixed_point<16> a)
{
	fixed_point<16> r;
	r.intValue = fixinv<16>(a.intValue);
	return r;
}

// The multiply accumulate case can be optimized.
template <int p>
inline fixed_point<p> multiply_accumulate(
	int count,
	const fixed_point<p> *a,
	const fixed_point<p> *b)
{
	long long result = 0;
	for (int i = 0; i < count; ++i)
		result += static_cast<long long>(a[i].intValue) * b[i].intValue;
	fixed_point<p> r;
	r.intValue = static_cast<int>(result >> p);
	return r;
}

} // end namespace fixedpoint

#endif
