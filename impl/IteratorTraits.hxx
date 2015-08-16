/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#ifndef TRAITS_HXX_INCLUDED
#define TRAITS_HXX_INCLUDED

#include "Fundamental.hxx"

#include <iterator>
#include <type_traits>

namespace Constainer {

	/*! The following traits depend on the resolution of LWG issue #2408 which is not included in C++14 but already resolved in
	    libc++ and libstdc++ */

	namespace detail {
		template <typename, typename, typename=void>
		struct hasIterCategoryConvTo : std::false_type {};
		template <typename I, typename C>
		struct hasIterCategoryConvTo<I, C,
			require<std::is_convertible<
				typename std::iterator_traits<I>::iterator_category, C
			>>>
			: std::true_type {};
	}

	template <typename I>
	using         isInputIterator = detail::hasIterCategoryConvTo<I, std::        input_iterator_tag>;
	template <typename I>
	using       isForwardIterator = detail::hasIterCategoryConvTo<I, std::      forward_iterator_tag>;
	template <typename I>
	using        isOutputIterator = detail::hasIterCategoryConvTo<I, std::       output_iterator_tag>;
	template <typename I>
	using  isRandomAccessIterator = detail::hasIterCategoryConvTo<I, std::random_access_iterator_tag>;
	template <typename I>
	using isBidirectionalIterator = detail::hasIterCategoryConvTo<I, std::bidirectional_iterator_tag>;

	template <typename I, typename T>
	using hasValueType = std::is_same<typename std::iterator_traits<I>::value_type, T>;

	/*template <typename I>
	using isContiguousIterator = detail::hasIterCategoryConvTo<I, std::contiguous_iterator_tag>;*/
}

#endif // TRAITS_HXX_INCLUDED
