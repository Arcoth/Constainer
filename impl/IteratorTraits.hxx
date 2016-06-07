/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Fundamental.hxx"

#include <iterator>
#include <type_traits>

namespace Constainer {

	/*! The following traits depend on the resolution of LWG issue #2408 which is not included in C++14 but already resolved in
	    libc++ and libstdc++ */

	namespace detail {
		template <typename, typename, typename=void>
		struct hasIterCategoryConvTo : STD::false_type {};
		template <typename I, typename C>
		struct hasIterCategoryConvTo<I, C,
			require<STD::is_convertible<
				typename STD::iterator_traits<I>::iterator_category, C
			>>>
			: STD::true_type {};
	}

	template <typename I>
	using         isInputIterator = detail::hasIterCategoryConvTo<I, STD::        input_iterator_tag>;
	template <typename I>
	using       isForwardIterator = detail::hasIterCategoryConvTo<I, STD::      forward_iterator_tag>;
	template <typename I>
	using        isOutputIterator = detail::hasIterCategoryConvTo<I, STD::       output_iterator_tag>;
	template <typename I>
	using  isRandomAccessIterator = detail::hasIterCategoryConvTo<I, STD::random_access_iterator_tag>;
	template <typename I>
	using isBidirectionalIterator = detail::hasIterCategoryConvTo<I, STD::bidirectional_iterator_tag>;

	template <typename I, typename T>
	using hasValueType = STD::is_same<typename STD::iterator_traits<I>::value_type, T>;

	template <typename I>
	using iteratorCategory = typename STD::iterator_traits<I>::iterator_category;

	/*template <typename I>
	using isContiguousIterator = detail::hasIterCategoryConvTo<I, STD::contiguous_iterator_tag>;*/
}
