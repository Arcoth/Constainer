/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

namespace Constainer {

template<typename Iterator>
class reverse_iterator
{
	Iterator _current;

	using _traits_type = STD::iterator_traits<Iterator>;

public:

	using iterator_type     = Iterator;

	using difference_type   = typename _traits_type::difference_type;
	using pointer           = typename _traits_type::pointer;
	using reference         = typename _traits_type::reference;
	using value_type        = typename _traits_type::value_type;
	using iterator_category = typename _traits_type::iterator_category;

	constexpr reverse_iterator() : _current() { }

	constexpr explicit reverse_iterator(iterator_type i) : _current(i) {}

	constexpr reverse_iterator(const reverse_iterator&) = default;

	template<typename It>
	constexpr  reverse_iterator(reverse_iterator<It> const& other)
	: _current(other.base()) { }

	constexpr iterator_type base() const { return _current; }

	constexpr reference operator*() const {
		return *prev(_current);
	}

	constexpr pointer operator->() const { return &operator*(); }

	constexpr reverse_iterator& operator++() {
		--_current;
		return *this;
	}

	constexpr reverse_iterator operator++(int) {
		reverse_iterator t = *this;
		--_current;
		return t;
	}

	constexpr reverse_iterator& operator--() {
		++_current;
		return *this;
	}

	constexpr reverse_iterator operator--(int) {
		reverse_iterator t = *this;
		++_current;
		return t;
	}

	constexpr reverse_iterator operator+(difference_type n) const
	{ return reverse_iterator(_current - n); }

	constexpr reverse_iterator& operator+=(difference_type n) {
		_current -= n;
		return *this;
	}

	constexpr reverse_iterator operator-(difference_type n) const
	{ return reverse_iterator(_current + n); }

	constexpr reverse_iterator& operator-=(difference_type n) {
		_current += n;
		return *this;
	}

	constexpr reference operator[](difference_type n) const
	{ return *(*this + n); }
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
constexpr auto
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

//! Related: LWG #685
template<typename I1, typename I2>
constexpr decltype(auto) operator-(const reverse_iterator<I1>& lhs, const reverse_iterator<I2>& rhs)
{ return rhs.base() - lhs.base(); }

template <typename I>
constexpr auto make_reverse_iterator( I i ) {return reverse_iterator<I>(i);}

}
