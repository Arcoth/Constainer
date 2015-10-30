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
	constexpr auto distance( RandomIt first, RandomIt last, std::random_access_iterator_tag )
	{ return last-first; }

	template <typename InputIt>
	constexpr auto distance( InputIt first, InputIt last, std::input_iterator_tag )
	{
		typename std::iterator_traits<InputIt>::difference_type n = 0;
		for (; first != last; ++first)
			++n;
		return n;
	}
}

template <typename InputIt>
constexpr auto distance( InputIt first, InputIt last )
{ return detail::distance(first, last, typename std::iterator_traits<InputIt>::iterator_category{}); }

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

template<typename Iterator>
class reverse_iterator
{
	Iterator _current;

	using _traits_type = std::iterator_traits<Iterator>;

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

/// MOVE ITERATOR:

template <typename Iterator>
class move_iterator
{
private:
	Iterator _current;

	using _traits_type    = std::iterator_traits<Iterator>;
	using _underlying_ref = typename _traits_type::reference;

public:
	using iterator_type = Iterator;
	using iterator_category = typename _traits_type::iterator_category;
	using value_type        = typename _traits_type::value_type;
	using difference_type   = typename _traits_type::difference_type;
	using pointer           = iterator_type; //! LWG #680

	//! LWG #2106
	using reference = std::conditional_t<std::is_reference<_underlying_ref>{},
	                                     std::remove_reference_t<_underlying_ref>&&,
	                                     _underlying_ref>;

	constexpr iterator_type base() const { return _current; }

	constexpr move_iterator() : _current() {}

	constexpr explicit move_iterator(iterator_type i) : _current(i) {}

	template<typename Iter>
	constexpr move_iterator(move_iterator<Iter> const& i) : _current(i.base()) {}

	constexpr reference operator*() const {
		//! LWG #2106
		return static_cast<reference>(*_current);
	}

	constexpr pointer operator->() const { return _current; }

	constexpr move_iterator  operator+ (difference_type n) const {return move_iterator(_current + n);}
	constexpr move_iterator  operator- (difference_type n) const {return move_iterator(_current - n);}

	constexpr move_iterator& operator++() {++_current; return *this;}
	constexpr move_iterator& operator--() {--_current; return *this;}

	constexpr move_iterator operator++(int) {move_iterator __tmp = *this; ++*this; return __tmp;}
	constexpr move_iterator operator--(int) {move_iterator __tmp = *this; --*this; return __tmp;}

	constexpr move_iterator& operator+=(difference_type n) {_current += n; return *this;}
	constexpr move_iterator& operator-=(difference_type n) {_current -= n; return *this;}

	constexpr reference operator[](difference_type n) const { return std::move(_current[n]); }
};

template <typename I1, typename I2>
constexpr bool operator==( move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return lhs.base() == rhs.base(); }

template <typename I1, typename I2>
constexpr bool operator!=( move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return !(lhs == rhs); }

template <typename I1, typename I2>
constexpr bool operator<(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return lhs.base() < rhs.base(); }

template <typename I1, typename I2>
constexpr bool operator<=(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return !(rhs < lhs); }

template <typename I1, typename I2>
constexpr bool operator>(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return rhs < lhs; }

template <typename I1, typename I2>
constexpr bool operator>=(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return !(lhs < rhs); }

//! Related: LWG #685
template <typename I1, typename I2>
constexpr decltype(auto) operator-(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return lhs.base() - rhs.base(); }

template <typename I>
constexpr move_iterator<I>
operator+(typename move_iterator<I>::difference_type n, move_iterator<I> const& lhs)
{ return lhs + n; }

template <typename I>
constexpr auto make_move_iterator(I i) {return move_iterator<I>(i);}

/// TRANSFORM ITERATOR

template <typename Extractor, typename Iterator>
struct transform_iterator
{
	using iterator_type = Iterator;
	using extractor_type = Extractor;

	using reference = std::result_of_t<
	                    extractor_type const&(typename std::iterator_traits<iterator_type>::reference)
	                  >;
	using value_type = std::decay_t<reference>;
	using pointer = value_type*;
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

private:

	std::tuple<Extractor, iterator_type> _data;
	constexpr decltype(auto) _it()       {return std::get<iterator_type>(_data);}
	constexpr decltype(auto) _ex() const {return std::get<extractor_type>(_data);}

public:

	constexpr auto extractor() const {return _ex();}
	constexpr auto base() const {return std::get<iterator_type>(_data);}

	constexpr explicit transform_iterator(iterator_type it, extractor_type const& ex = {}) : _data(ex, it) {}

	template <typename Q, typename Iter2>
	constexpr transform_iterator(transform_iterator<Q, Iter2> const& other)
		: _data(other._data) {}

	constexpr reference operator*() const {return _ex()(*base());}

	constexpr pointer operator->() const {return &operator*();}

	constexpr transform_iterator& operator++() {
		++_it();
		return *this;
	}

	constexpr transform_iterator operator++(int) {
		auto t = *this;
		++_it();
		return t;
	}

	constexpr transform_iterator& operator--() {
		--_it();
		return *this;
	}

	constexpr transform_iterator operator--(int) {
		transform_iterator t = *this;
		--_it();
		return t;
	}

	constexpr transform_iterator operator+(difference_type n) const
	{ return transform_iterator(base() + n); }

	constexpr transform_iterator& operator+=(difference_type n) {
		_it() += n;
		return *this;
	}

	constexpr transform_iterator operator-(difference_type n) const
	{ return transform_iterator(base() - n); }

	constexpr transform_iterator& operator-=(difference_type n) {
		_it() -= n;
		return *this;
	}

	constexpr reference operator[](difference_type n) const
	{ return *(*this + n); }
};

template <typename P, typename I>
constexpr bool operator==(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return lhs.base() == rhs.base(); }

template <typename P, typename I>
constexpr bool operator <(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return rhs.base() < lhs.base(); }

template<typename P, typename I>
constexpr bool operator!=(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return !(lhs == rhs); }

template <typename P, typename I>
constexpr bool operator >(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return rhs < lhs; }

template <typename P, typename I>
constexpr bool operator<=(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return !(rhs < lhs); }

template <typename P, typename I>
constexpr bool operator>=(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return !(lhs < rhs); }

template <typename P, typename I>
constexpr auto operator-(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return rhs.base() - lhs.base(); }

template <typename P, typename I>
constexpr auto operator+(typename transform_iterator<P,I>::difference_type n, transform_iterator<P,I> const& lhs)
{ return transform_iterator<P,I>(lhs.base() - n); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator==(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return lhs.base() == rhs.base(); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator <(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return rhs.base() < lhs.base(); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator!=(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return !(lhs == rhs); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator >(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return rhs < lhs; }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator<=(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return !(rhs < lhs); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator>=(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return !(lhs < rhs); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr decltype(auto) operator-(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return rhs.base() - lhs.base(); }

template <typename E, typename I>
constexpr auto make_transform_iterator(I i, E const& e) {
	return transform_iterator<E,I>(i,e);
}


}
