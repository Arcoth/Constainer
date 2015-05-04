/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef VECTOR_HXX_INCLUDED
#define VECTOR_HXX_INCLUDED

#include "Constructor.hxx"
#include "Algorithms.hxx"
#include "Array.hxx"
#include "Assert.hxx"

#include <algorithm>

namespace Constainer {

template <typename T, std::size_t MaxN, typename Constructor = DefaultConstructor<T>>
class Vector : private Array<T, MaxN> {

	using _base = Array<T, MaxN>;

public:

	using constructor_type = Constructor;
	using constructor_traits = ConstructorTraits<constructor_type>;

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

	constructor_type _constructor;

	constexpr void _verifySize() const {
		AssertExcept<std::bad_alloc>( size() <= MaxN );
	}
	constexpr void _sizeIncable(size_type s = 1) const {
		AssertExcept<std::bad_alloc>( size() <= MaxN-s );
	}
	constexpr void _verifiedSizeInc(size_type s = 1) {
		_sizeIncable(s); _size += s;
	}

public:

	constexpr constructor_type get_constructor() const {return _constructor;}

	using _base::rend;
	using _base::crend;
	using _base::begin;
	using _base::cbegin;
	using _base::front;
	static constexpr auto max_size() {return _base::size();}

	constexpr auto size() const {return _size;}
	constexpr bool empty() const {return size() == 0;}

	constexpr Vector() :_base{}, _size(0), _constructor() {}
	explicit constexpr Vector(constructor_type const& c) :_base{}, _size(0), _constructor(c) {}

	constexpr Vector( size_type s, constructor_type const& c = constructor_type() )
		: _base{}, _size(s), _constructor(c) {_verifySize();}

	constexpr Vector( size_type s, value_type const& v, constructor_type const& c = constructor_type() )
		: Vector(s, c) {
		uninitialized_fill_n(this->begin(), size(), v, _constructor);
	}

	template <typename InputIterator,
	          typename=requires<isInputIterator<InputIterator>>>
	constexpr Vector( InputIterator first, InputIterator last, constructor_type const& c = constructor_type() )
		: Vector(c) {
		while (first != last)
			emplace_back(*first++);
	}

private:
	template <std::size_t N>
	using ThisResized = Vector<value_type, N, constructor_type>;

	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN> const& other, constructor_type const& c, ...)
		: Vector(other.begin(), other.end(), c) {}

	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN>     && other, constructor_type const& c, ...)
		: Vector(make_move_iterator(other.begin()), make_move_iterator(other.end()), c) {}

	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN> const& other, ...) : Vector(other, other._constructor, 0) {}

	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN>     && other, ...)
		: _base{}, _size(0), _constructor(std::move(other._constructor))
	{
		for (size_type s = 0; s != other.size(); ++s) {
			emplace_back(std::move(other[s]));
			constructor_traits::destroy(_constructor, &other[s]);
		}
		other._size = 0;
	}

public:

	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN> const& other) : Vector(other, constructor_traits::select_on_container_copy_construction(other.get_constructor()), 0) {}
	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN>     && other) : Vector(std::move(other), 0) {}
	constexpr Vector(Vector              const& other) : Vector(other, constructor_traits::select_on_container_copy_construction(other.get_constructor()), 0) {}
	constexpr Vector(Vector                  && other) : Vector(std::move(other), 0) {}

	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN> const& other, constructor_type const& c) : Vector(other, c, 0) {}
	template <std::size_t OtherN>
	constexpr Vector(ThisResized<OtherN>     && other, constructor_type const& c) : Vector(std::move(other), c, 0) {}
	constexpr Vector(Vector              const& other, constructor_type const& c) : Vector(other, c, 0) {}
	constexpr Vector(Vector                  && other, constructor_type const& c) : Vector(std::move(other), c, 0) {}

	constexpr Vector(std::initializer_list<value_type> ilist, constructor_type const& c = constructor_type())
		: Vector(std::begin(ilist), std::end(ilist), c) {}

private:
	template <typename U>
	constexpr void _condAssignCtors(U&& other, std::true_type) {
		_constructor = std::forward<U>(other);
	}
	template <typename U>
	constexpr void _condAssignCtors(U&&, std::false_type) {}

public:
	template <std::size_t OtherN>
	constexpr Vector& operator=( ThisResized<OtherN> const& other ) {
		AssertExcept<std::bad_alloc>( other.size() <= max_size() );
		clear();
		_condAssignCtors(other._constructor, constructor_traits::propagate_on_container_copy_assignment);
		insert(begin(), other.begin(), other.end());
		return *this;
	}

	template <std::size_t OtherN>
	constexpr Vector& operator=( ThisResized<OtherN>&& other ) {
		AssertExcept<std::bad_alloc>( other.size() <= max_size() );
		clear();
		_condAssignCtors(std::move(other._constructor), constructor_traits::propagate_on_container_move_assignment);
		insert(begin(), make_move_iterator(other.begin()), make_move_iterator(other.end()));
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

	template <class InputIt>
	requires<isInputIterator<InputIt>> assign (InputIt first, InputIt last) {
		// TODO: Simplify
		clear();
		insert(begin(), first, last);
	}

private:
	template <typename U>
	constexpr void _push_back( U&& u ) {
		_sizeIncable();
		constructor_traits::construct(_constructor, &*end(), std::forward<U>(u));
		++_size;
	}

public:

	constexpr void push_back( value_type const& v ) {
		_push_back(v);
	}
	constexpr void push_back( value_type&& v ) {
		_push_back(std::move(v));
	}

	template <typename... Args>
	constexpr void emplace_back( Args&&... args ) {
		push_back( value_type(std::forward<Args>(args)...) );
	}

	constexpr void erase( const_iterator first, const_iterator last ) {
		move( const_cast<iterator>(last), end(), const_cast<iterator>(first));
		auto original_end = end();
		_size -= last-first;
		destroy(end(), original_end, _constructor);
	}
	constexpr void erase( const_iterator it ) {
		erase(it, it+1);
	}

	constexpr void clear() {erase(begin(), end());}

	constexpr void resize(size_type s, const_reference v) {
		if (s < size()) {
			destroy(begin() + s, end());
		}
		else {
			_sizeIncable(s-size());
			insert(end(), s-size(), v);
		}
		_size = s;
	}

	constexpr void resize(size_type s) {
		resize(s, value_type());
	}

	template <typename ForwardIt>
	constexpr requires<isForwardIterator<ForwardIt>>
	insert( const_iterator pos, ForwardIt first, ForwardIt last ) {
		// TODO: Get this to work without getting size_increase, i.e. make this algorithm
		//       work with input iterators that aren't forward iterators.
		static_assert( sizeof (ForwardIt) < 2, "This function is unfortunately bugged." );
		auto size_increase = distance(first, last);
		_sizeIncable(size_increase);
		uninitialized_move( end()-size_increase, end(), end(), _constructor );
		move_backward( const_cast<iterator>(pos), end()-size_increase, end() );
		copy( first, last, const_cast<iterator>(pos) );
		_size += size_increase;
	}

	template <typename... Args>
	constexpr void emplace( const_iterator i, Args&&... args ) {
		// TODO: Avoid unnecessary copy
		insert(i, value_type(std::forward<Args>(args)...));
	}

	constexpr void insert( const_iterator i, value_type const& v ) {
		insert(i, &v, &v+1);
	}

	constexpr void insert( const_iterator pos, size_type c, value_type const& v ) {
		_sizeIncable(c);
		uninitialized_move( end()-c, end(), end(), _constructor );
		move_backward( const_cast<iterator>(pos), end()-c, end() );
		fill_n( const_cast<iterator>(pos), c, v );
		_size += c;
	}

	constexpr void insert( const_iterator i, std::initializer_list<value_type> ilist ) {
		insert(i, ilist.begin(), ilist.end());
	}

	constexpr void pop_back() {Assert(size() >= 1, "Can't pop"); erase(end()-1);}

private:
	constexpr void _swap_ctors( constructor_type& other, std::true_type) {
		swap(_constructor, other);
	}
	constexpr void _swap_ctors( constructor_type& other, std::false_type) {
		Assert(other == _constructor);
	}

public:

	template <std::size_t OtherMax>
	constexpr void swap( ThisResized<OtherMax>& other ) {
		AssertExcept<std::out_of_range>( other.size() < max_size() && size() < OtherMax, "Swap fails" );

		auto min = std::min(other.size(), size());
		swap_ranges(other.begin(), other.begin() + min, begin());

		auto& _1 = other.size() >  size()? other : *this;
		auto& _2 = other.size() <= size()? other : *this;
		uninitialized_move(_1.begin() + min, _1.end(), _2.begin()+min, _2._constructor);
		destroy(_1.begin() + min, _1.end(), _1._constructor);

		_swap_ctors(other._constructor, constructor_traits::propagate_on_container_swap);
		Constainer::swap(_size, other._size);
	}

};

template <typename T, std::size_t Size, typename Ctor> constexpr auto begin(Vector<T, Size, Ctor> const& a) {return a.begin();}
template <typename T, std::size_t Size, typename Ctor> constexpr auto   end(Vector<T, Size, Ctor> const& a) {return a.  end();}
template <typename T, std::size_t Size, typename Ctor> constexpr auto begin(Vector<T, Size, Ctor>      & a) {return a.begin();}
template <typename T, std::size_t Size, typename Ctor> constexpr auto   end(Vector<T, Size, Ctor>      & a) {return a.  end();}

// TODO: Implement relational operators

}

#endif // VECTOR_HXX_INCLUDED
