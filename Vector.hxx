/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef VECTOR_HXX_INCLUDED
#define VECTOR_HXX_INCLUDED

#include "Array.hxx"
#include "Assert.hxx"

#include <algorithm>

namespace Constainer {

// inb4 you can drop the private y'know8
template <typename T, std::size_t MaxN>
class Vector : private Array<T, MaxN> {

	using _base = Array<T, MaxN>;

public:
	using typename _base::size_type;
	using typename _base::value_type;
	using typename _base::iterator;
	using typename _base::const_iterator;
	using typename _base::reverse_iterator;
	using typename _base::const_reverse_iterator;
	using typename _base::reference;
	using typename _base::const_reference;

private:
	size_type _size = 0;

	constexpr void _verifySize() const {
		AssertExcept<std::bad_alloc>( size() <= MaxN );
	}
	constexpr void _sizeIncable(size_type s = 1) const {
		AssertExcept<std::bad_alloc>( size() <= MaxN-s );
	}
	constexpr void _verifiedSizeInc() {
		++_size; _verifySize();
	}

public:

	using _base::rend;
	using _base::crend;
	using _base::begin;
	using _base::cbegin;
	using _base::front;
	static constexpr auto max_size() {return _base::size();}

	constexpr auto size() const {return _size;}
	constexpr bool empty() const {return size() == 0;}

	constexpr Vector() :_base{},  _size(0) {}

	constexpr Vector( size_type s ) : _base{}, _size(s) {_verifySize();}

	constexpr Vector( size_type s, value_type const& v ) : Vector(s) {
		fill_n( this->begin(), size(), v );
	}

	template <typename InputIterator,
	          typename=requires<isInputIterator<InputIterator>>>
	constexpr Vector( InputIterator first, InputIterator last )
		: Vector(distance(first, last)) {
		copy(first, last, begin());
	}

private:
	template <std::size_t OtherN>
	constexpr Vector(Vector<value_type, OtherN> const& other, int)
		: Vector(other.begin(), other.end()) {}

	template <std::size_t OtherN>
	constexpr Vector(Vector<value_type, OtherN>     && other, int)
		: Vector(make_move_iterator(other.begin()), make_move_iterator(other.end())) {}

public:
	template <std::size_t N>
	using ThisResized = Vector<value_type, N>;

	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN> const& other) : Vector(other, 0) {}
	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN>     && other) : Vector(std::move(other), 0) {}

	constexpr Vector(Vector const& other) : Vector(other, 0) {}
	constexpr Vector(Vector&& other)      : Vector(std::move(other), 0) {}

	constexpr Vector(std::initializer_list<value_type> ilist)
		: Vector(std::begin(ilist), std::end(ilist)) {}

public:
	template <std::size_t OtherN>
	constexpr Vector& operator=( ThisResized<OtherN> const& other ) {
		AssertExcept<std::bad_alloc>( other.size() <= max_size() );
		assign(other.begin(), other.end());
		return *this;
	}

	template <std::size_t OtherN>
	constexpr Vector& operator=( ThisResized<OtherN>&& other ) {
		AssertExcept<std::bad_alloc>( other.size() <= max_size() );
		assign(make_move_iterator(other.begin()), make_move_iterator(other.end()));
		other.clear();
		return *this;
	}

	constexpr Vector& operator=( Vector const& other )
	{ return operator=<>(other); }

	constexpr Vector& operator=( Vector&& other )
	{ return operator=<>(std::move(other)); }

	constexpr Vector& operator=( std::initializer_list<value_type> ilist )
	{ assign(begin(ilist), end(ilist)); return *this; }

	constexpr const_reverse_iterator crbegin() const {return rbegin();}
	constexpr const_iterator            cend() const {return    end();}
	constexpr reverse_iterator        rbegin()       {return       reverse_iterator(end());}
	constexpr const_reverse_iterator  rbegin() const {return const_reverse_iterator(end());}
	constexpr iterator                   end()       {return this->begin() + size();}
	constexpr const_iterator             end() const {return this->begin() + size();}

	constexpr       reference back ()       {return end()[-1];}
	constexpr const_reference back () const {return end()[-1];}

	constexpr reference       operator[](size_type s) {
		Assert(s < size(), "Invalid index!"); return _base::operator[](s);
	}
	constexpr const_reference operator[](size_type s) const {
		Assert(s < size(), "Invalid index!"); return _base::operator[](s);
	}

	template <typename... Args>
	constexpr void emplace_back( Args&&... args ) {
		push_back( value_type(std::forward<Args>(args)...) );
	}

private:
	template <typename U>
	constexpr void _push_back( U&& u ) {
		_sizeIncable();
		*this->end() = std::forward<U>(u);
		++_size;
	}

public:

	constexpr void push_back( value_type const& v ) {
		_push_back(v);
	}
	constexpr void push_back( value_type&& v ) {
		_push_back(std::move(v));
	}

	constexpr void erase( const_iterator first, const_iterator last ) {
		move( const_cast<iterator>(last),    this->end(),
		      const_cast<iterator>(first));
		_size -= last-first;
	}
	constexpr void erase( const_iterator it ) {
		erase(it, it+1);
	}

	constexpr void clear() {_size = 0;}

private:
	template <typename InputIt>
	constexpr void _insert( iterator pos, InputIt first, InputIt last, std::input_iterator_tag ) {
		while (first != last)
			insert(pos++, *first++);
	}

	template <typename ForwardIt>
	constexpr void _insert( iterator pos, ForwardIt first, ForwardIt last, std::forward_iterator_tag ) {
		auto size_increase = distance(first, last);
		_sizeIncable(size_increase);
		move_backward( const_cast<iterator>(pos), end(), end()+size_increase );
		copy( first, last, const_cast<iterator>(pos) );
		_size += size_increase;
	}

	template <typename U>
	constexpr void _insert( const_iterator pos, U&& u ) {
		_sizeIncable();
		auto it = const_cast<iterator>(pos);
		move_backward( it, end(), end()+1 );
		*it = std::forward<U>(u);
		++_size;
	}

public:
	template <typename InputIt>
	constexpr requires<isInputIterator<InputIt>>
	insert( const_iterator pos, InputIt first, InputIt last ) {
		_insert(const_cast<iterator>(pos), first, last,
		        typename std::iterator_traits<InputIt>::iterator_category());
	}

	template <typename... Args>
	constexpr void emplace( const_iterator i, Args&&... args ) {
		// TODO: Avoid unnecessary copy (?)
		insert(i, value_type(std::forward<Args>(args)...));
	}

	constexpr void insert( const_iterator pos, value_type const& v ) {_insert(pos, v);}
	constexpr void insert( const_iterator pos, value_type     && v ) {_insert(pos, std::move(v));}

	constexpr void insert( const_iterator pos, size_type c, value_type const& v ) {
		_sizeIncable(c);
		move_backward( const_cast<iterator>(pos), end(), end()+c );
		fill_n( const_cast<iterator>(pos), c, v);
		_size += c;
	}

	constexpr void insert( const_iterator i, std::initializer_list<value_type> ilist ) {
		insert(i, ilist.begin(), ilist.end());
	}

	template <class InputIt>
	constexpr requires<isInputIterator<InputIt>>
	assign(InputIt first, InputIt last) {
		clear();
		while (first != last)
			emplace_back(*first++);
	}

	constexpr void pop_back() {Assert(size() >= 1, "Can't pop"); erase(end()-1);}

	template <typename U, std::size_t OtherMax>
	constexpr void swap( Vector<U, OtherMax>& other ) {
		Assert( other.size() < max_size() && size() < OtherMax, "Swap fails" );

		auto min = std::min(other.size(), size());
		swap_ranges(other.begin(), other.begin() + min, this->begin());

		if (other.size()> size()) move(other.begin() + min, other.end(), this->begin()+min);
		else                      move(this->begin() + min, this->end(), other.begin()+min);

		Constainer::swap(_size, other._size);
	}
};

template <typename T, std::size_t Size> constexpr auto begin(Vector<T, Size> const& a) {return a.begin();}
template <typename T, std::size_t Size> constexpr auto   end(Vector<T, Size> const& a) {return a.  end();}
template <typename T, std::size_t Size> constexpr auto begin(Vector<T, Size>      & a) {return a.begin();}
template <typename T, std::size_t Size> constexpr auto   end(Vector<T, Size>      & a) {return a.  end();}

template <typename T1, std::size_t Size1, typename T2, std::size_t Size2>
constexpr bool operator==(Vector<T1, Size1> const& lhs, Vector<T2, Size2> const& rhs) {
	return lhs.size() == rhs.size() && Constainer::equal(lhs.begin(), lhs.end(), rhs.begin());
}
template <typename T1, std::size_t Size1, typename T2, std::size_t Size2>
constexpr bool operator!=(Vector<T1, Size1> const& lhs, Vector<T2, Size2> const& rhs) {
	return !(lhs == rhs);
}
template <typename T1, std::size_t Size1, typename T2, std::size_t Size2>
constexpr bool operator<(Vector<T1, Size1> const& lhs, Vector<T2, Size2> const& rhs) {
	return Constainer::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename T1, std::size_t Size1, typename T2, std::size_t Size2>
constexpr bool operator>=(Vector<T1, Size1> const& lhs, Vector<T2, Size2> const& rhs) {
	return !(lhs < rhs);
}
template <typename T1, std::size_t Size1, typename T2, std::size_t Size2>
constexpr bool operator>(Vector<T1, Size1> const& lhs, Vector<T2, Size2> const& rhs) {
	return rhs < lhs;
}
template <typename T1, std::size_t Size1, typename T2, std::size_t Size2>
constexpr bool operator<=(Vector<T1, Size1> const& lhs, Vector<T2, Size2> const& rhs) {
	return !(lhs > rhs);
}
}

#endif // VECTOR_HXX_INCLUDED
