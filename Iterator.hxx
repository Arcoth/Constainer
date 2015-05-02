/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef ITERATOR_HXX_INCLUDED
#define ITERATOR_HXX_INCLUDED

#include <iterator>

namespace Constainer {

template <typename InputIt>
constexpr auto distance( InputIt first, InputIt last ) {
	typename std::iterator_traits<InputIt>::difference_type n = 0;
	for (; first != last; ++first)
		++n;
	return n;
}

template <typename InputIt>
constexpr void advance( InputIt& it, typename std::iterator_traits<InputIt>::difference_type n ) {
	if (n < 0)
		do --it;
		while (++n);
	else
		while (n-- > 0)
			++it;
}

template <typename InputIt>
constexpr auto next( InputIt it, typename std::iterator_traits<InputIt>::difference_type n=1 ) {
	advance(it, n);
	return it;
}

template <typename InputIt>
constexpr auto prev( InputIt it, typename std::iterator_traits<InputIt>::difference_type n=1 ) {
	return next(it, -n);
}

template<typename Iterator>
class reverse_iterator :
	public std::iterator<typename std::iterator_traits<Iterator>::iterator_category,
	                     typename std::iterator_traits<Iterator>::value_type,
	                     typename std::iterator_traits<Iterator>::difference_type,
	                     typename std::iterator_traits<Iterator>::pointer,
	                     typename std::iterator_traits<Iterator>::reference>
{
	Iterator current;

	using traits_type = std::iterator_traits<Iterator>;

public:

	using iterator_type = Iterator;
	using difference_type = typename traits_type::difference_type;
	using pointer         = typename traits_type::pointer;
	using reference       = typename traits_type::reference;

	constexpr reverse_iterator() : current() { }

	constexpr explicit reverse_iterator(iterator_type i) : current(i) {}

	constexpr reverse_iterator(const reverse_iterator&) = default;

	template<typename It>
	constexpr  reverse_iterator(reverse_iterator<It> const& other)
	: current(other.base()) { }

	constexpr iterator_type base() const { return current; }

	constexpr reference operator*() const {
		return *prev(current);
	}

	constexpr pointer operator->() const { return &operator*(); }

	constexpr reverse_iterator& operator++() {
		--current;
		return *this;
	}

	constexpr reverse_iterator operator++(int) {
		reverse_iterator t = *this;
		--current;
		return t;
	}

	constexpr reverse_iterator& operator--() {
		++current;
		return *this;
	}

	constexpr reverse_iterator operator--(int) {
		reverse_iterator t = *this;
		++current;
		return t;
	}

	constexpr reverse_iterator operator+(difference_type n) const
	{ return reverse_iterator(current - n); }

	constexpr reverse_iterator& operator+=(difference_type n) {
		current -= n;
		return *this;
	}

	constexpr reverse_iterator operator-(difference_type __n) const
	{ return reverse_iterator(current + __n); }

	constexpr reverse_iterator& operator-=(difference_type __n) {
		current += __n;
		return *this;
	}

	constexpr reference operator[](difference_type __n) const
	{ return *(*this + __n); }
};

template<typename I>
constexpr bool operator==(const reverse_iterator<I>& lhs, const reverse_iterator<I>& rhs)
{ return lhs.base() == rhs.base(); }

template<typename I>
constexpr bool operator <(const reverse_iterator<I>& lhs, const reverse_iterator<I>& rhs)
{ return rhs.base() < lhs.base(); }

template<typename I>
constexpr bool operator!=(const reverse_iterator<I>& lhs, const reverse_iterator<I>& rhs)
{ return !(lhs == rhs); }

template<typename I>
constexpr bool operator >(const reverse_iterator<I>& lhs, const reverse_iterator<I>& rhs)
{ return rhs < lhs; }

template<typename I>
constexpr bool operator<=(const reverse_iterator<I>& lhs, const reverse_iterator<I>& rhs)
{ return !(rhs < lhs); }

template<typename I>
constexpr bool operator>=(const reverse_iterator<I>& lhs, const reverse_iterator<I>& rhs)
{ return !(lhs < rhs); }

template<typename I>
constexpr auto operator- (const reverse_iterator<I>& lhs, const reverse_iterator<I>& rhs)
{ return rhs.base() - lhs.base(); }

template<typename I>
constexpr reverse_iterator<I>
operator+(typename reverse_iterator<I>::difference_type n, const reverse_iterator<I>& lhs)
{ return reverse_iterator<I>(lhs.base() - n); }

template<typename I1, typename I2>
constexpr bool operator==(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
{ return lhs.base() == rhs.base(); }

template<typename I1, typename I2>
constexpr bool operator <(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
{ return rhs.base() < lhs.base(); }

template<typename I1, typename I2>
constexpr bool operator!=(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
{ return !(lhs == rhs); }

template<typename I1, typename I2>
constexpr bool operator >(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
{ return rhs < lhs; }

template<typename I1, typename I2>
constexpr bool operator<=(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
{ return !(rhs < lhs); }

template<typename I1, typename I2>
constexpr bool operator>=(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
{ return !(lhs < rhs); }

template<typename I1, typename I2>
constexpr auto operator -(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
	-> decltype(rhs.base() - lhs.base())
{ return rhs.base() - lhs.base(); }

template <typename I>
constexpr reverse_iterator<I> make_reverse_iterator( I i )
{ return reverse_iterator<I>(i); }

}

#include "RangeAccess.hxx"

#endif // ITERATOR_HXX_INCLUDED
