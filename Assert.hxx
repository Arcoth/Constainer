/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef ASSERT_HXX_INCLUDED
#define ASSERT_HXX_INCLUDED

#include <stdexcept>

#include <cstdlib>

namespace Constainer {

// In the abscence of a better name...
/*! If you're getting an error message about throw-expressions not being usable in a constant expression here,
    go back one step in the call stack for this error and check the message */
template <typename Except=std::logic_error, typename... Args>
constexpr void AssertExcept(bool b, Args&&... args) {
	if (!b) throw Except(std::forward<Args>(args)...);
}

/*! If you're getting an error message about std::abort not being usable in a constant expression here,
    go back one step in the call stack for this error and check the message */
constexpr void Assert(bool b, char const* = nullptr) {
	if (!b) std::abort();
}

}

#endif // ASSERT_HXX_INCLUDED