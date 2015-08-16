/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#ifndef MATH_HXX_INCLUDED
#define MATH_HXX_INCLUDED

#include "impl/Fundamental.hxx"

#include <type_traits>

namespace Constainer {

namespace detail {
	template <typename T>
	constexpr T signum(T x, std::false_type) {
		return T(0) < x;
	}

	template <typename T>
	constexpr T signum(T x, std::true_type) {
		return (T(0) < x) - (x < T(0));
	}
}

template <typename T>
CONSTAINER_PURE_CONST constexpr T signum(T x) {
	return detail::signum(x, std::is_signed<T>{});
}

template <typename T>
CONSTAINER_PURE_CONST constexpr require<std::is_arithmetic<T>, T>
abs(T i) {
	// Works for infty, and -0.: Returns --0. = +0.
	return i>0? i : -i;
}

/**< Does FP-multiplication but covers infinity to keep the expressions constant.
     Some implementations (e.g. Clang) require that floating point arithmetic
     shall not produce infinity, since such operations
     are technically invoking undefined behavior, which is not allowed to appear
     in constant expressions. Used in Parsers.hxx. */
template <typename Float>
CONSTAINER_PURE_CONST constexpr Float safeMul( Float lhs, Float rhs ) {
	const Float infties[] {
		-std::numeric_limits<Float>::infinity(),
		 std::numeric_limits<Float>::infinity()
	};
	const auto infty = infties[1];

	if (abs(lhs) < abs(rhs))
		return safeMul(rhs, lhs);

	if (abs(lhs) == infty)
		return infties[signum(lhs)==signum(rhs)];

	if (abs(rhs) <= 1)
		return lhs*rhs;

	if (abs(rhs) > std::numeric_limits<Float>::max()/abs(lhs))
		return infties[signum(lhs)==signum(rhs)];

	return lhs*rhs;
}

template <typename Float>
CONSTAINER_PURE_CONST constexpr require<std::is_arithmetic<Float>, Float>
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


CONSTAINER_PURE_CONST constexpr unsigned popcount( std::uint64_t v ) {
	#if defined __clang__ || defined __GNUG__
		return __builtin_popcountll(v);
	#else
		// From http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
		/*static*/ const std::uint8_t table[256] =
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

CONSTAINER_PURE_CONST constexpr unsigned count_trailing( std::uint64_t v ) {
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

}


#endif // MATH_HXX_INCLUDED
