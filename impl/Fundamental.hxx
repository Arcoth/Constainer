/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

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

#define CONSTAINER_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#define CONSTAINER_DIAGNOSTIC_POP  _Pragma("GCC diagnostic pop")
#define CONSTAINER_STRINGIZE_(x) #x
#define CONSTAINER_STRINGIZE(x) CONSTAINER_STRINGIZE_(x)
#define CONSTAINER_DIAGNOSTIC_IGNORE(w)  _Pragma(CONSTAINER_STRINGIZE(GCC diagnostic ignored w))

#include <experimental/type_traits>

namespace Constainer {

// STD::experimental, std.
namespace STD {
	using namespace std;
	using namespace experimental;
}

static constexpr STD::size_t defaultContainerSize =
#ifdef CONSTAINER_DEFAULT_CONTAINER_SIZE
	CONSTAINER_DEFAULT_CONTAINER_SIZE
#else
	256
#endif // CONSTAINER_DEFAULT_CONTAINER_SIZE
;

// For algorithms like stable_partition, who will work with an array of this size to improve
// running time.
static constexpr STD::size_t internalWorkingMemory =
#ifdef CONSTAINER_INTERNAL_WORKING_MEMORY
	CONSTAINER_INTERNAL_WORKING_MEMORY
#else
	defaultContainerSize
#endif
;

template <typename T> struct identity {using type=T;};

/**< More literal versions of enable_if */
template <typename T, typename R=void>
using require     = STD::enable_if_t<T{}, R>;

template <typename T, typename R=void>
using require_not = STD::enable_if_t<!T{}, R>;

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

template <typename...>
using void_t = void;

/**< These templates are, despite being well-formed per se(!),
     useful for getting a type or value "printed" at compile-time.  */
template <typename T>
void forceError() {
	forceError(T(), 0);
}

template <typename T>
void forceError(T);

template <typename, typename, typename=void>
struct has_is_transparent {};

template<typename Compare, typename T>
struct has_is_transparent<Compare, T, void_t<typename Compare::is_transparent>>
{ using type = T; };

}

/**< assocWithNS determines whether Constainer is an associated namespace of T.
     The definition checks whether ADL sees the second overload that is only visible in the instantiation context. */
namespace Constainer {

template <typename T>
STD::false_type _assocTester(T, ...);

namespace detail {
	template <typename T>
	using assocWithNS = decltype(_assocTester(STD::declval<T>()));
}

template <typename T>
STD::true_type _assocTester(T, int);

}
