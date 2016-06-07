/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "TransformIterator.hxx"

namespace Constainer {

struct indirect_iterator_extractor {
	template <typename Iter>
	constexpr decltype(auto) operator()(Iter i) {
		return *i;
	}
};

template <typename Iter>
using indirect_iterator = transform_iterator<indirect_iterator_extractor, Iter>;

}
