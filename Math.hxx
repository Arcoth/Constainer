/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#ifndef MATH_HXX_INCLUDED
#define MATH_HXX_INCLUDED

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
constexpr T signum(T x) {
	return detail::signum(x, std::is_signed<T>());
}

template <typename T>
constexpr requires<std::is_arithmetic<T>, T>
abs(T i) {
	return i>0? i : -i;
}

/**< Does FP-multiplication but covers infinity to keep the expressions constant.
     Some implementations (e.g. Clang) require that floating point arithmetic
     shall not produce infinity, since such operations
     are technically invoking undefined behavior, which is not allowed to appear
     in constant expressions. Used in Parsers.hxx. */
template <typename Float>
constexpr Float safeMul( Float lhs, Float rhs ) {
	const Float infties[] {
		-std::numeric_limits<Float>::infinity(),
		 std::numeric_limits<Float>::infinity()
	};
	const auto infty = infties[1];

	if (lhs < rhs)
		return safeMul(rhs, lhs);

	if (abs(rhs) <= 1)
		return lhs*rhs;

	if (abs(lhs) > std::numeric_limits<Float>::max()/abs(rhs))
		return infties[signum(lhs)==signum(rhs)];

	return lhs*rhs;
}

template <typename Float>
constexpr requires<std::is_arithmetic<Float>, Float>
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

}


#endif // MATH_HXX_INCLUDED
