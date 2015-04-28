/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef ASSERT_HXX_INCLUDED
#define ASSERT_HXX_INCLUDED

#include <stdexcept>

#include <cstdlib>

namespace Constainer {

// In the abscence of a better name...
template <typename Except=std::logic_error, typename... Args>
constexpr void AssertExcept(bool b, Args&&... args) {
	if (!b) throw Except(std::forward<Args>(args)...);
}

constexpr void Assert(bool b) {
	if (!b) std::abort();
}

}

#endif // ASSERT_HXX_INCLUDED
