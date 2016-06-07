/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Stack.hxx"

#include <limits>
#include <type_traits>

#include <cassert>

namespace Constainer {

namespace detail {
	template <typename T>
	CONSTAINER_PURE_CONST constexpr T signum(T x, STD::false_type) {
		return T(0) < x;
	}

	template <typename T>
	CONSTAINER_PURE_CONST constexpr T signum(T x, STD::true_type) {
		return (T(0) < x) - (x < T(0));
	}
}

template <typename T>
CONSTAINER_PURE_CONST constexpr T signum(T x) {
	return detail::signum(x, STD::is_signed<T>{});
}

template <typename T>
CONSTAINER_PURE_CONST constexpr require<STD::is_arithmetic<T>, T>
abs(T i) {
	return i>T(0)? i : i<T(0)? -i : T(0);
}

/**< Does FP-multiplication but covers infinity to keep the expressions constant.
     Some implementations (e.g. Clang) require that floating point arithmetic
     shall not produce infinity, since such operations
     are technically invoking undefined behavior, which is not allowed to appear
     in constant expressions. Used in Parsers.hxx. */
template <typename Float>
CONSTAINER_PURE_CONST constexpr Float safeMul( Float lhs, Float rhs ) {
	const Float infties[] {
		-STD::numeric_limits<Float>::infinity(),
		 STD::numeric_limits<Float>::infinity()
	};
	const auto infty = infties[1];

	if (abs(lhs) < abs(rhs))
		return safeMul(rhs, lhs);

	if (abs(lhs) == infty)
		return infties[signum(lhs)==signum(rhs)];

	if (abs(rhs) <= 1)
		return lhs*rhs;

	if (abs(rhs) > STD::numeric_limits<Float>::max()/abs(lhs))
		return infties[signum(lhs)==signum(rhs)];

	return lhs*rhs;
}

template <typename Float>
CONSTAINER_PURE_CONST constexpr require<STD::is_arithmetic<Float>, Float>
pow(Float f, int e) {
	if (e==0)
		return 1;
	if (f==0)
		return 0;

	bool flip = e<0;
	e = abs(e);

	Float res = 1;
	for (; e; e >>= 1) {
		if (e&1)
			res = safeMul(res, f);

		f = safeMul(f, f);
	}

	return flip? 1/res : res;
}


CONSTAINER_PURE_CONST constexpr unsigned popcount( STD::uint64_t v ) {
	#if defined __clang__ || defined __GNUG__
		return __builtin_popcountll(v);
	#else
		// From http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
		/*static*/ const STD::uint8_t table[256] =
		{
		#define B2(n) n,     n+1,     n+1,     n+2
		#define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
		#define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
			B6(0), B6(1), B6(1), B6(2)
		#undef B2
		#undef B4
		#undef B6
		};

		return table[ v           & 0xFF] +
		       table[(v >> 8)     & 0xFF] +
		       table[(v >> 16)    & 0xFF] +
		       table[(v >> 24)    & 0xFF] +
		       table[(v >> 32)    & 0xFF] +
		       table[(v >> 40)    & 0xFF] +
		       table[(v >> 48)    & 0xFF] +
		       table[ v >> 56];
	#endif
}

CONSTAINER_PURE_CONST constexpr unsigned count_trailing( STD::uint64_t v ) {
	#if defined __clang__ || defined __GNUG__
		return __builtin_ctzll(v);
	#else
		const int deBruijn[64] = {
			 0,  1,  2, 53,  3,  7, 54, 27,
			 4, 38, 41,  8, 34, 55, 48, 28,
			62,  5, 39, 46, 44, 42, 22,  9,
			24, 35, 59, 56, 49, 18, 29, 11,
			63, 52,  6, 26, 37, 40, 33, 47,
			61, 45, 43, 21, 23, 58, 17, 10,
			51, 25, 36, 32, 60, 20, 57, 16,
			50, 31, 19, 15, 30, 14, 13, 12,
		};
		return deBruijn[(v & -v) * 0x022FDD63CC95386D >> 27];
	#endif
}

/**< Obtains the fractional part of r by subtracting powers of two. */
template <typename Real>
CONSTAINER_PURE_CONST constexpr Real fractional (Real r)
{
	Real x = signum(r);
	while (x < r/2)
		x *= 2;
	while (x >= 1)
	{
		if (x <= r)
			r -= x;
		x /= 2;
	}
	return r;
}

/**< Obtains r%u. */
template <typename Real>
CONSTAINER_PURE_CONST constexpr Real remainder (Real r, STD::uintmax_t u)
{
	assert (u != 0);
	if (UINTMAX_MAX >= r/u)
		return r-STD::uintmax_t (r/u) *u;

	Stack<Real> upows ({1});
	for (Real x=1, xu = x*u; r >= xu;)
	{
		assert (xu);
		upows.push (x = xu);
		xu *= u;
	}
	do
	{
		auto value = upows.pop_return();
		if (r >= value)
			r -= STD::uintmax_t (r/value) *value;
	}
	while (UINTMAX_MAX* (Real) u <= r);
	return r-STD::uintmax_t (r/u) *u;
}

template <typename Real>
CONSTAINER_PURE_CONST constexpr STD::uintmax_t rounded_remainder (Real r, STD::uintmax_t u) {
	return remainder (r, u) + 0.5;
}

// Doesn't correctly determine negative zeroes yet.
template <typename T>
CONSTAINER_PURE_CONST constexpr bool is_negative ( T t ) {
	return t < 0 /*|| 1/t == -STD::numeric_limits<T>::infinity()*/;
}

/**< Inefficient implementation; However, constexpr forbids e.g. aliasing through char,
     so using properties of IEEE 754 is impossible. */
template <typename Real>
CONSTAINER_PURE_CONST constexpr STD::pair<Real, int> normalize (int base, Real r)
{
	int exp_sum = 0;
	int exp = 1;
	Constainer::Stack<Real> pows;
	bool negative = is_negative (r);
	if (negative)
		r = -r;
	if (r!=0 && r<1)
	{
		for (Real x=Real(1)/base; ; x*=x)
		{
			pows.push (x);
			if (r >= x*x)
				break;
			exp *= 2;
		}
		do
		{
			auto pow = pows.pop_return();
			while (r/pow < base)
			{
				r /= pow;
				exp_sum -= exp;
			}
			exp /= 2;
		}
		while (r < 1);

	}
	else if (r >= base)
	{
		for (Real x=base; ; x*=x)
		{
			pows.push (x);
			if (r/x < x)
				break;
			exp *= 2;
		}
		do
		{
			auto pow = pows.pop_return();
			while (r >= pow)
			{
				r /= pow;
				exp_sum += exp;
			}
			exp /= 2;
		}
		while (r >= base);
	}
	return {negative? -r : r, exp_sum};
}

template <typename T>
constexpr T ln10 = 2.3025850929940456840179914546843L;
template <typename Real>
CONSTAINER_PURE_CONST constexpr Real pow10(Real x) {
	Real sum=1, fact=1;
	auto y = x * ln10<Real>;
	x = y;
	for (int i=2; i < STD::numeric_limits<Real>::max_digits10+5; ++i) {
		sum += fact*x;
		x *= y;
		fact /= i;
	}
	return sum;
}

}
