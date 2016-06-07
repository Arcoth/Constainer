/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

namespace Constainer {

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
	Constainer::accumulate(first, last, init, STD::plus<>{});
}

template <typename InputIt, typename OutputIt, typename BinaryOp>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt out,
                                       BinaryOp op) {
	if (first == last)
		return out;

	auto* prev = &*first;
	*out = *prev;
	while (++first != last) {
		auto&& val = *first;
		*++out = op(val, *prev);
		prev = &val;
	}
	return ++out;
}
template <typename InputIt, typename OutputIt, typename BinaryOp>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt out) {
	return Constainer::adjacent_difference(first, last, out, STD::minus<>{});}

template <typename InputIt, typename OutputIt, typename BinaryOperation>
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt out, BinaryOperation op) {
	if (first == last)
		return out;

	auto sum = *first;
	*out = sum;

	while (++first != last) {
		sum = op(sum, *first);
		*++out = sum;
	}
	return ++out;
}
template <typename InputIt, typename OutputIt>
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt out) {
	return Constainer::partial_sum(first, last, out, STD::plus<>{});}

template <typename InputIt1, typename InputIt2, typename T,
          typename BinaryOp1, typename BinaryOp2>
constexpr T inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, T value,
                          BinaryOp1 op1, BinaryOp2 op2) {
	while (first1 != last1)
		value = op1(value, op2(*first1++, *first2++));

	return value;
}
template <typename InputIt1, typename InputIt2, typename T>
constexpr T inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, T value) {
	return Constainer::inner_product(first1, last1, first2, value, STD::plus<>{}, STD::multiplies<>{});}

}
