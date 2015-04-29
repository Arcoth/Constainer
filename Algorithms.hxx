/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef ALGORITHMS_HXX_INCLUDED
#define ALGORITHMS_HXX_INCLUDED

#include <functional>
#include <utility>

namespace Constainer {

template <typename T>
constexpr void swap(T& a, T& b) {
	T tmp = std::move(a);
	a = std::move(b);
	b = std::move(tmp);
}

template <typename InputIterator, typename T, typename Comp>
constexpr auto find(InputIterator first, InputIterator last, T const& val, Comp comp) {
	while (first != last && !comp(*first, val))
		++first;
	return first;
}

template <typename InputIterator, typename T>
constexpr auto find(InputIterator first, InputIterator last, T const& val) {
	return find(first, last, val, std::equal_to<>());
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt out) {
	while (first != last)
		*out++ = *first++;

	return out;
}

template <typename BiDir, typename BiDir2>
constexpr BiDir2 copy_backward(BiDir  first, BiDir last,
                               BiDir2 last2) {
	while (last != first)
		*--last2 = *--last;

	return last2;
}

template <typename BiDir, typename BiDir2>
constexpr BiDir2 move_backward(BiDir  first, BiDir last,
                               BiDir2 last2) {
	while (last != first)
		*--last2 = std::move(*--last);

	return last2;
}

template<class InputIt, class OutputIt>
constexpr OutputIt move(InputIt first, InputIt last, OutputIt out) {
	while (first != last)
		*out++ = std::move(*first++);

	return out;
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
