/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Copy.hxx"

namespace Constainer {

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr STD::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                 InputIt2 first2, Comp comp) {
	for (; first1 != last1; ++first1, ++first2)
		if (!comp(*first1, *first2))
			break;
	return {first1, first2};
}

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr STD::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
	return Constainer::mismatch(first1, last1, first2, STD::equal_to<>{});}

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr STD::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                 InputIt2 first2, InputIt2 last2, Comp comp) {
	while (first1 != last1 && first2 != last2) {
		if (!comp(*first1, *first2))
			break;
		++first1; ++first2;
	}

	return {first1, first2};
}

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr STD::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                 InputIt2 first2, InputIt2 last2) {
	return Constainer::mismatch(first1, last1, first2, last2, STD::equal_to<>{});}

template <typename InputIt1, typename InputIt2, typename Compare>
constexpr bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                                       InputIt2 first2, InputIt2 last2,
                                       Compare comp)
{
	while (first1 != last1 && first2 != last2) {
		if (comp(*first1, *first2)) return true;
		if (comp(*first2, *first1)) return false;
		++first1; ++first2;
	}
	return (first1 == last1) && (first2 != last2);
}

template <typename InputIt1, typename InputIt2>
constexpr bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                                       InputIt2 first2, InputIt2 last2) {
	return Constainer::lexicographical_compare(first1, last1, first2, last2, STD::less<>{});}

template <typename Input1, typename Input2, typename Comparator>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2, Comparator comp ) {
	return Constainer::mismatch(first1, last1, first2, comp).first == last1;}
template <typename Input1, typename Input2>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2 ) {
	return Constainer::equal(first1, last1, first2, STD::equal_to<>{});}
template <typename Input1, typename Input2, typename Comparator>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2, Input2 last2, Comparator comp ) {
	auto p = Constainer::mismatch(first1, last1, first2, last2, comp);
	return p.first == last1 && p.second == last2;
}
template <typename Input1, typename Input2>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2, Input2 last2 ) {
	return Constainer::equal(first1, last1, first2, last2, STD::equal_to<>{});}

template <typename ForwardIt1, typename ForwardIt2, typename BinaryPredicate>
constexpr ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                            ForwardIt2 s_first, ForwardIt2 s_last,
                            BinaryPredicate pred) {
	for (;;++first) {
		auto it   = first;
		auto s_it = s_first;
		for (;;) {
			if (s_it == s_last)
				return first;
			if (it == last)
				return last;
			if (!pred(*it, *s_it))
				break;
			++it; ++s_it;
		}
	}
}
template <typename ForwardIt1, typename ForwardIt2, typename BinaryPredicate>
constexpr ForwardIt1 search(ForwardIt1 f, ForwardIt1 l,
                            ForwardIt2 sf, ForwardIt2 sl) {
	return search(f, l, sf, sl, STD::equal_to<>{});}



template <typename ForwardIt1, typename ForwardIt2, typename BinaryPred>
constexpr ForwardIt1 find_end(ForwardIt1 first, ForwardIt1 last,
                              ForwardIt2 s_first, ForwardIt2 s_last,
                              BinaryPred pred)
{
	if (s_first == s_last)
		return last;
	auto result = last;
	for(;;) {
		auto new_result = Constainer::search(first, last, s_first, s_last, pred);
		if (new_result == last)
			return result;
		else {
			result = new_result;
			first = result;
			++first;
		}
	}
	return result;
}
template <typename ForwardIt1, typename ForwardIt2>
constexpr ForwardIt1 find_end(ForwardIt1 first, ForwardIt1 last,
                              ForwardIt2 s_first, ForwardIt2 s_last) {
	return Constainer::find_end(first, last, s_first, s_last, STD::equal_to<>{});}



template <typename ForwardIt, typename ForwardIt2>
constexpr auto swap_ranges(ForwardIt first, ForwardIt last, ForwardIt2 first2) {
	while (first != last)
		swap(*first++, *first2++);

	return first;
}
template <typename T, STD::size_t N>
constexpr void swap(T (&a)[N], T (&b)[N]) {
	swap_ranges(a, a+N, b);
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename Compare>
constexpr OutputIt merge(InputIt1 first1, InputIt1 last1,
                         InputIt2 first2, InputIt2 last2,
                         OutputIt out, Compare comp) {
	for (; first1 != last1; ++out) {
		if (first2 == last2)
			return Constainer::copy(first1, last1, out);

		if (comp(*first2, *first1))
			*out = *first2++;
		else
			*out = *first1++;
	}
	return Constainer::copy(first2, last2, out);
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename Compare>
constexpr OutputIt merge(InputIt1 first1, InputIt1 last1,
                         InputIt2 first2, InputIt2 last2,
                         OutputIt out) {
	Constainer::merge(first1, last1, first2, last2, out, STD::less<>{});}



}
