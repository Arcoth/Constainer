#ifndef FUNDAMENTAL_HXX_INCLUDED
#define FUNDAMENTAL_HXX_INCLUDED

#include <climits>

#if __cplusplus < 201402L
	#error This library is intended for use with the C++14 programming language and following.
#endif

#if CHAR_BIT != 8
	#error This library depends on char designating an octet. Your runtime environment is bad, and you should feel bad.
#endif

#ifndef CONSTAINER_PURE_CONST
	#if defined __clang__ || defined __GNUG__
		#define CONSTAINER_PURE_CONST [[gnu::const]]
	#elif defined __INTEL_COMPILER
		#define CONSTAINER_PURE_CONST __attribute__((const))
	#else
		#define CONSTAINER_PURE_CONST
	#endif
#endif // defined CONSTAINER_PURE_CONST

#endif // FUNDAMENTAL_HXX_INCLUDED
