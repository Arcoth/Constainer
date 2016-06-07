/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

namespace Constainer {

template <typename InputIt, typename UnaryPred>
constexpr bool is_partitioned(InputIt first, InputIt last, UnaryPred pred)
{
	first = Constainer::find_if_not(first, last, pred);
	return Constainer::find_if(first, last, pred) == last;
}

template <typename BidirIt, typename UnaryPred>
constexpr BidirIt partition(BidirIt first, BidirIt last, UnaryPred pred)
{
	first = Constainer::find_if_not(first, last, pred);
	if (first == last)
		return first;

	for (auto i = Constainer::next(first); i != last; ++i)
		if (pred(*i)){
			Constainer::iter_swap(i, first);
			++first;
		}

	return first;
}

template <typename InputIt, typename OutputIt1,
          typename OutputIt2, typename UnaryPred>
constexpr STD::pair<OutputIt1, OutputIt2>
	partition_copy(InputIt first, InputIt last,
	               OutputIt1 out_true, OutputIt2 out_false,
	               UnaryPred pred)
{
	while (first != last)
		if (pred(*first)) *out_true++  = *first++;
		else              *out_false++ = *first++;

	return {out_true, out_false};
}

template <typename ForwardIt, typename UnaryPred>
constexpr ForwardIt partition_point(ForwardIt first, ForwardIt last, UnaryPred pred) {
	return Constainer::find_first_not_of(first, last, pred);
}

namespace detail {
	template <typename ForwardIt, typename UnaryPred>
	constexpr void stable_partition_small(ForwardIt first, ForwardIt last, UnaryPred pred) {
		Constainer::Vector<typename STD::iterator_traits<ForwardIt>::value_type,
		                   internalWorkingMemory> vec{};
		for (auto it = first; it != last; ++it)
			if (pred(*it))
				vec.push_back(STD::move(*it));
		for (auto it = first; it != last; ++it)
			if (!pred(*it))
				vec.push_back(STD::move(*it));
		Constainer::move(begin(vec), end(vec), first);
	}
}

template <typename BidirIt, typename UnaryPred>
constexpr BidirIt stable_partition(BidirIt first, BidirIt last, UnaryPred pred)
{
	auto dist = Constainer::distance(first, last);
	// do fine-grained partitioning in O(n) time using some working memory…
	while (dist >= 0) {
		auto next = Constainer::next(first, Constainer::min((STD::size_t)dist, internalWorkingMemory));
		detail::stable_partition_small(first, next, pred);
		first = next;
		dist -= internalWorkingMemory;
	}

	// finish off using repeated rotation
	first = Constainer::find_if_not(first, last, pred);
	auto end = first;
	for (;;) {
		auto new_first = Constainer::find_if(end, last, pred);
		if (new_first == last)
			break;
		auto end = Constainer::find_if_not(new_first, last, pred);
		Constainer::rotate(first, new_first, end);
		first = Constainer::next(first, Constainer::distance(new_first, end));
	}
	return first;
}

}
