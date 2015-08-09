/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#ifndef FUNDAMENTAL_HXX_INCLUDED
#define FUNDAMENTAL_HXX_INCLUDED

#include <type_traits>
#include <utility>

#include <climits>

#if __cplusplus < 201402L
	#error This library is intended for use with a C++14 compliant implementation.
#endif

#if CHAR_BIT != 8
	#error This library depends on char representing an octet. Your runtime environment is bad, and you should feel bad.
#endif

#ifndef CONSTAINER_PURE
	#if defined __clang__ || defined __GNUG__
		#define CONSTAINER_PURE [[gnu::pure]]
	#elif defined __INTEL_COMPILER
		#define CONSTAINER_PURE __attribute__((pure))
	#else
		#define CONSTAINER_PURE
	#endif
#endif // defined CONSTAINER_PURE

#ifndef CONSTAINER_PURE_CONST
	#if defined __clang__ || defined __GNUG__
		#define CONSTAINER_PURE_CONST [[gnu::const]]
	#elif defined __INTEL_COMPILER
		#define CONSTAINER_PURE_CONST __attribute__((const))
	#else
		#define CONSTAINER_PURE_CONST
	#endif
#endif // defined CONSTAINER_PURE_CONST

namespace Constainer {

template <typename T> struct identity {using type=T;};

/**< More literal versions of enable_if */
template <typename T, typename R=void>
using requires     = std::enable_if_t<T{}, R>;

template <typename T, typename R=void>
using requires_not = std::enable_if_t<!T{}, R>;

/**< Depends on the resolution of LWG #2296 */
template <typename T>
constexpr auto addressof( T& t ) {
#ifdef __clang__
	return __builtin_addressof(t);
#else
	return &t;
#endif
}

template <typename T>
constexpr decltype(auto) as_const(T const& obj) {return obj;}

/**< These templates are, despite being well-formed per se(!),
     useful for getting a type or value "printed" at compile-time.  */
template <typename T>
void forceError() {
	forceError(T(), 0);
}

template <typename T>
void forceError(T);

}

/**< assocWithNS determines whether Constainer is an associated namespace of T.
     The definition checks whether ADL sees the second overload that is only visible in the instantiation context. */
namespace Constainer {

template <typename T>
std::false_type _assocTester(T, ...);

namespace detail {
	template <typename T>
	using assocWithNS = decltype(_assocTester(std::declval<T>()));
}

template <typename T>
std::true_type _assocTester(T, int);

}

#endif // FUNDAMENTAL_HXX_INCLUDED
