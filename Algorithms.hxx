/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef ALGORITHMS_HXX_INCLUDED
#define ALGORITHMS_HXX_INCLUDED

#include "Iterator.hxx"

#include <functional>
#include <utility>

namespace Constainer {

namespace detail {
	template <typename T>
	constexpr void swap(T& a, T& b, ...) {
		T tmp = std::move(a);
		a = std::move(b);
		b = std::move(tmp);
	}

	template <typename T>
	constexpr auto swap(T& a, T& b, int) -> decltype(a.swap(b)) {
		a.swap(b);
	}
}

template <typename T>
constexpr void swap(T& a, T& b) {detail::swap(a,b,0);}

template <typename InputIterator, typename T, typename Comp>
constexpr auto find(InputIterator first, InputIterator last, T const& val, Comp comp) {
	while (first != last && !comp(*first, val))
		++first;
	return first;
}

template <typename InputIterator, typename T>
constexpr auto find(InputIterator first, InputIterator last, T const& val) {
	return Constainer::find(first, last, val, std::equal_to<>());
}

template <typename InputIt, typename OutputIt>
constexpr auto copy(InputIt first, InputIt last, OutputIt out) {
	while (first != last)
		*out++ = *first++;

	return out;
}

template <typename InputIt, typename SizeType, typename OutputIt>
constexpr auto copy_n(InputIt first, SizeType count, OutputIt out) {
	while (count-- != 0)
		*out++ = *first++;

	return out;
}

template <typename BiDir, typename BiDir2>
constexpr auto copy_backward(BiDir  first, BiDir last, BiDir2 last2) {
	while (last != first)
		*--last2 = *--last;

	return last2;
}

template <typename BiDir, typename BiDir2>
constexpr auto move_backward(BiDir first, BiDir last, BiDir2 last2) {
	return Constainer::copy_backward(make_move_iterator(first), make_move_iterator(last), last2);
}

template<class InputIt, class OutputIt>
constexpr auto move(InputIt first, InputIt last, OutputIt out) {
	return Constainer::copy(make_move_iterator(first), make_move_iterator(last), out);
}

template <typename InputIt, typename SizeType, typename OutputIt>
constexpr auto move_n(InputIt first, SizeType count, OutputIt out) {
	return Constainer::copy_n(make_move_iterator(first), count, out);
}

template <typename OutputIt, typename T>
constexpr auto fill(OutputIt first, OutputIt last, T const& value) {
	while (first != last)
		*first++ = value;

	return first;
}

template <typename OutputIt, typename SizeType, typename T>
constexpr auto fill_n(OutputIt first, SizeType count, T const& value) {
	for (SizeType i = 0; i++ < count;)
		*first++ = value;

	return first;
}

template <typename ForwardIt, typename ForwardIt2>
constexpr auto swap_ranges(ForwardIt first, ForwardIt last, ForwardIt2 first2) {
	while (first != last)
		swap(*first++, *first2++);

	return first;
}

template <typename T, std::size_t N>
constexpr void swap(T (&a)[N], T (&b)[N]) {
	swap_ranges(a, a+N, b);
}

template <typename ForwardIterator, typename T>
constexpr void iota(ForwardIterator first, ForwardIterator last, T value) {
	while(first != last) {
		*first++ = value;
		++value;
	}
}

template <typename ForwardIterator, typename SizeType, typename T>
constexpr void iota_n(ForwardIterator first, SizeType n, T value) {
	while (n--) {
		*first++ = value;
		++value;
	}
}

template <typename InputIt, typename T, typename BinaryOp>
constexpr T accumulate(InputIt first, InputIt last, T init, BinaryOp op)
{
	for (; first != last; ++first)
		init = op(init, *first);

	return init;
}

template <typename InputIt, typename T>
constexpr T accumulate(InputIt first, InputIt last, T init) {
	Constainer::accumulate(first, last, init, std::plus<>());
}

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr std::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                 InputIt2 first2, Comp comp) {
	for (; first1 != last1; ++first1, ++first2)
		if (!comp(*first1, *first2))
			break;
	return {first1, first2};
}

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr std::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
	return Constainer::mismatch(first1, last1, first2, std::equal_to<>());
}

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr std::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                 InputIt2 first2, InputIt2 last2, Comp comp) {
	while (first1 != last1 && first2 != last2) {
		if (!comp(*first1, *first2))
			break;
		++first1; ++first2;
	}

	return {first1, first2};
}

template <typename InputIt1, typename InputIt2, typename Comp>
constexpr std::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                 InputIt2 first2, InputIt2 last2) {
	return Constainer::mismatch(first1, last1, first2, last2, std::equal_to<>());
}

template <typename InputIt1, typename InputIt2, typename Compare>
constexpr bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                                       InputIt2 first2, InputIt2 last2,
                                       Compare comp) {
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
	return Constainer::lexicographical_compare(first1, last1, first2, last2, std::less<>());
}

template <typename Input1, typename Input2, typename Comp>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2, Comp comp ) {
	return Constainer::mismatch(first1, last1, first2, comp).first == last1;
}
template <typename Input1, typename Input2>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2 ) {
	return Constainer::equal(first1, last1, first2, std::equal_to<>());
}
template <typename Input1, typename Input2, typename Comp>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2, Input2 last2, Comp comp ) {
	auto p = Constainer::mismatch(first1, last1, first2, last2, comp);
	return p.first == last1 && p.second == last2;
}
template <typename Input1, typename Input2>
constexpr bool equal( Input1 first1, Input1 last1, Input2 first2, Input2 last2 ) {
	return Constainer::equal(first1, last1, first2, last2, std::equal_to<>());
}

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
	return search(f, l, sf, sl, std::equal_to<>());
}

template <typename InputIt, typename ForwardIt, typename BinaryPredicate>
constexpr InputIt find_first_of(InputIt first, InputIt last,
                                ForwardIt s_first, ForwardIt s_last,
                                BinaryPredicate pred) {
	for (; first != last; ++first)
		for (auto it = s_first; it != s_last; ++it)
			if (pred(*first, *it))
				return first;

	return last;
}
template <typename InputIt, typename ForwardIt, typename BinaryPredicate>
constexpr InputIt find_first_of(InputIt f, InputIt l,
                                ForwardIt sf, ForwardIt sl) {
	return find_first_of(f, l, sf, sl, std::equal_to<>());
}


template <typename InputIt, typename ForwardIt, typename BinaryPredicate>
constexpr InputIt find_first_not_of(InputIt first, InputIt last,
                                    ForwardIt s_first, ForwardIt s_last,
                                    BinaryPredicate pred) {
	for (; first != last; ++first) {
		auto it = s_first;
		for (; it != s_last; ++it)
			if (pred(*first, *it))
				break;
		if (it == s_last)
			return first;
	}
	return last;
}

template <typename InputIt, typename ForwardIt, typename BinaryPredicate>
constexpr InputIt find_first_not_of(InputIt f, InputIt l,
                                    ForwardIt sf, ForwardIt sl) {
	return find_first_of(f, l, sf, sl, std::equal_to<>());
}
}

#endif
