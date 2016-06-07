/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "impl/IteratorTraits.hxx"

#include <iterator>
#include <tuple>

namespace Constainer {

namespace detail {
	template <typename RandomIt>
	constexpr auto distance( RandomIt first, RandomIt last, STD::random_access_iterator_tag )
	{ return last-first; }

	template <typename InputIt>
	constexpr auto distance( InputIt first, InputIt last, STD::input_iterator_tag )
	{
		typename STD::iterator_traits<InputIt>::difference_type n = 0;
		for (; first != last; ++first)
			++n;
		return n;
	}
}

template <typename InputIt>
constexpr auto distance( InputIt first, InputIt last )
{ return detail::distance(first, last, typename STD::iterator_traits<InputIt>::iterator_category{}); }

template <typename InputIt>
constexpr void advance( InputIt& it, typename STD::iterator_traits<InputIt>::difference_type n ) {
	if (n < 0)
		do --it;
		while (++n);
	else
		while (n-- > 0)
			++it;
}

template <typename InputIt>
constexpr auto next( InputIt it, typename STD::iterator_traits<InputIt>::difference_type n=1 ) {
	advance(it, n);
	return it;
}

template <typename InputIt>
constexpr auto prev( InputIt it, typename STD::iterator_traits<InputIt>::difference_type n=1 ) {
	return next(it, -n);
}

namespace detail {
/**< Primary overload. This should be specialized for containers to improve performance. */
template <typename Container, typename ConstIterator>
constexpr auto unconstifyIterator( Container& c, ConstIterator cit ) {
	return c.begin() + (cit - c.cbegin());
}
template <typename Container, typename T>
constexpr auto unconstifyIterator( Container&, T const* ptr ) {
	return const_cast<T*>(ptr);
}
}

}

#include "impl/Iterator/IndirectIterator.hxx"
#include "impl/Iterator/InsertIterators.hxx"
#include "impl/Iterator/MoveIterator.hxx"
#include "impl/Iterator/ReverseIterator.hxx"
#include "impl/Iterator/TransformIterator.hxx"
