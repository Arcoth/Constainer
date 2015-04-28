/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef ALGORITHMS_HXX_INCLUDED
#define ALGORITHMS_HXX_INCLUDED

#include <utility>

namespace Constainer {

template <typename T>
constexpr void swap(T& a, T& b) {
	T tmp = std::move(a);
	a = std::move(b);
	b = std::move(tmp);
}

template <typename InputIterator, typename OutputIterator>
constexpr void copy(InputIterator first, InputIterator last, OutputIterator out) {
	while (first != last)
		*out++ = *first++;
}

template <typename InputIterator, typename T>
constexpr void fill(InputIterator first, InputIterator last, T const& value) {
	while (first != last)
		*first++ = value;
}

template <typename InputIterator, typename SizeType, typename T>
constexpr void fill_n(InputIterator first, SizeType count, T const& value) {
	for (SizeType i = 0; i++ < count;)
		*first++ = value;
}

template <typename InputIterator, typename InputIterator2>
constexpr void swap_ranges(InputIterator first, InputIterator last,
                           InputIterator2 first2) {
	while (first != last)
		swap(*first++, *first2++);
}

template <typename T, std::size_t N>
constexpr void swap(T (&a)[N], T (&b)[N]) {
	swap_ranges(a, a+N, b);
}

}

#endif
