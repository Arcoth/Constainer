/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Assert.hxx"
#include "Iterator.hxx"
#include "Vector.hxx"

#include <experimental/algorithm>
#include <functional>
#include <utility>

namespace Constainer {

using STD::max;
using STD::min;

template <typename F>
struct NegateFunctor {
	F f;

	constexpr NegateFunctor(F const& f) : f(f) {}

	template <typename... Args>
	constexpr bool operator()(Args&&... args) const {
		return !f(STD::forward<Args>(args)...);
	}
};
template <typename F>
constexpr auto negateFunctor(F const& f) {
	return NegateFunctor<F>(f);
}

struct IdentityFunctor {
	template <typename T>
	constexpr decltype(auto) operator()(T&& t) const {return STD::forward<T>(t);}
};

}

#include "impl/Algorithms/Copy.hxx"
#include "impl/Algorithms/BinarySeqOps.hxx"
#include "impl/Algorithms/NonModifying.hxx"
#include "impl/Algorithms/Numerical.hxx"
#include "impl/Algorithms/Partitioning.hxx"
#include "impl/Algorithms/Transformations.hxx"
