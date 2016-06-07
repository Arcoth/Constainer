/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "impl/Fundamental.hxx"

#include <stdexcept>

#include <cassert>

namespace Constainer {

// In the abscence of a better name...
/*! If you're getting an error message about throw-expressions not being usable in a constant expression here,
    go back one step in the call stack for this error and check the message */
template <typename Except=STD::logic_error, typename... Args>
constexpr void AssertExcept(bool b, Args&&... args) {
	if (!b)
		throw Except(STD::forward<Args>(args)...);
}

//! LWG 2234 guarantees that assert produces a constant subexpression if the argument is one.

}
