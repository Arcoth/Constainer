/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef VECTOR_HXX_INCLUDED
#define VECTOR_HXX_INCLUDED

#include "Array.hxx"
#include "Assert.hxx"

#include <algorithm>

namespace Constainer {

template <typename T>
struct DefaultCopyTraits {
	using value_type = T;
	using pointer = T*;
	using const_pointer = T const*;
	using reference = T&;
	using const_reference = T const&;

	static constexpr pointer copy(pointer s1, const_pointer s2, std::size_t n) {
		return Constainer::copy_n(s2, n, s1);
	}

	/**< This function is called to actually 'move', not copy. */
	static constexpr pointer move(pointer s1, const_pointer s2, std::size_t n) {
		return Constainer::move_n(s2, n, s1);
	}

	static constexpr pointer assign(pointer s, std::size_t n, const_reference a) {
		return Constainer::fill_n(s, n, a);
	}

	/**< Called after erase has performed on elements in the container */
	static constexpr void destroy(pointer, std::size_t) {}

	template <typename U>
	static constexpr void assign( reference r, U&& a ) {
		r = std::forward<U>(a);
	}
};

/**< This is the fundamental class template that provides a 'resizable array'.
     Its interface is std::vector-like.
     Note that the size of the underlying array is MaxN+1 because BasicString needs a null-terminator,
     and this is the easiest and cheapest way of providing one. */
template <typename T, std::size_t MaxN, typename CopyTraits>
class BasicVector : private Array<T, MaxN+1> {
	using _base = Array<T, MaxN+1>;

public:
	using traits_type = CopyTraits;

	using typename _base::size_type;
	using typename _base::value_type;
	using typename _base::iterator;
	using typename _base::const_iterator;
	using typename _base::pointer;
	using typename _base::const_pointer;
	using typename _base::reverse_iterator;
	using typename _base::const_reverse_iterator;
	using typename _base::reference;
	using typename _base::const_reference;

private:
	template <size_type OtherN>
	using ThisResized = BasicVector<value_type, OtherN, traits_type>;

protected:

	size_type _size = 0;

	constexpr auto _data() {return this->_storage;}

	constexpr void _verifySize() const {
		AssertExcept<std::length_error>( size() <= MaxN, "Size check failed" );
	}
	constexpr void _verifySizeInc(size_type s = 1) const {
		AssertExcept<std::length_error>( size() <= MaxN-s, "Invalid attempt to increase container size" );
	}
	constexpr void _verifiedSizeInc(size_type c = 1) {
		_size += c; _verifySize();
	}

	constexpr void _createInsertionSpace(const_iterator pos, size_type len) {
		Assert(pos <= end());
		_verifiedSizeInc(len);
		if (pos != end()-len)
			// TODO: Implement move_backward that uses CopyTraits::assign
			Constainer::move_backward(_address(pos), end()-len, end());
	}

public:
	using _base::data;

	using _base::rend;
	using _base::crend;
	using _base::begin;
	using _base::cbegin;
	using _base::front;
	static constexpr auto max_size() {return _base::size();}

	constexpr auto size() const {return _size;}
	constexpr bool empty() const {return size() == 0;}

	constexpr BasicVector() :_base{},  _size(0) {}

	constexpr BasicVector( size_type s ) : _base{}, _size(s) {_verifySize();}

	constexpr BasicVector( size_type s, value_type const& v ) : BasicVector(0) {
		insert(begin(), s, v);
	}

	template <typename InputIt,
	          typename=requires<isInputIterator<InputIt>>>
	constexpr BasicVector( InputIt first, InputIt last ) : BasicVector() {
		assign(first, last);
	}

private:
	template <std::size_t OtherN>
	constexpr BasicVector(ThisResized<OtherN> const& other, int)
		: BasicVector(other.begin(), other.end()) {}

	template <std::size_t OtherN>
	constexpr BasicVector(ThisResized<OtherN>     && other, int)
		: BasicVector(make_move_iterator(other.begin()), make_move_iterator(other.end())) {}

public:

	template <std::size_t OtherN>
	constexpr BasicVector(ThisResized<OtherN> const& other) : BasicVector(other, 0) {}
	template <std::size_t OtherN>
	constexpr BasicVector(ThisResized<OtherN>     && other) : BasicVector(std::move(other), 0) {}

	constexpr BasicVector(BasicVector const& other) : BasicVector(other, 0) {}
	constexpr BasicVector(BasicVector&& other)      : BasicVector(std::move(other), 0) {}

	constexpr BasicVector(std::initializer_list<value_type> ilist)
		: BasicVector(std::begin(ilist), std::end(ilist)) {}

	template <std::size_t OtherN>
	constexpr BasicVector& operator=( ThisResized<OtherN> const& other ) {
		AssertExcept<std::bad_alloc>( other.size() <= max_size() );
		assign(other.begin(), other.end());
		return *this;
	}

	template <std::size_t OtherN>
	constexpr BasicVector& operator=( ThisResized<OtherN>&& other ) {
		AssertExcept<std::bad_alloc>( other.size() <= max_size() );
		assign(make_move_iterator(other.begin()), make_move_iterator(other.end()));
		other.clear();
		return *this;
	}

	constexpr BasicVector& operator=( BasicVector const& other )
	{ return operator=<>(other); }

	constexpr BasicVector& operator=( BasicVector&& other )
	{ return operator=<>(std::move(other)); }

	constexpr BasicVector& operator=( std::initializer_list<value_type> ilist )
	{ assign(ilist); return *this; }

	constexpr const_reverse_iterator crbegin() const {return rbegin();}
	constexpr const_iterator            cend() const {return    end();}
	constexpr reverse_iterator        rbegin()       {return       reverse_iterator(end());}
	constexpr const_reverse_iterator  rbegin() const {return const_reverse_iterator(end());}
	constexpr iterator                   end()       {return this->begin() + size();}
	constexpr const_iterator             end() const {return this->begin() + size();}

	constexpr       reference back()       {return end()[-1];}
	constexpr const_reference back() const {return end()[-1];}

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
		_verifySizeInc();
		traits_type::assign(*this->end(), std::forward<U>(u));
		++_size;
	}

protected:
	constexpr pointer _address(const_iterator i) {
		static_assert( std::is_same<const_iterator, const_pointer>{},
		               "Requires const_iterator = const_pointer" );
		return const_cast<pointer>(i);
	}

	constexpr pointer _remcv(const_iterator i) {
		static_assert( std::is_same<const_iterator, const_pointer>{},
		               "Requires const_iterator = const_pointer" );
		return const_cast<iterator>(i);
	}

public:

	constexpr void push_back( value_type const& v ) {
		_push_back(v);
	}
	constexpr void push_back( value_type&& v ) {
		_push_back(std::move(v));
	}

	constexpr void erase( const_iterator first, const_iterator last ) {
		traits_type::move( _address(first), _address(last), end() - last);
		traits_type::destroy(end() - (last-first), last-first);
		_size -= last-first;
	}
	constexpr void erase( const_iterator it ) {
		erase(it, it+1);
	}

	constexpr void clear() {erase(begin(), end());}

private:
	template <typename InputIt>
	constexpr iterator _insert( iterator pos, InputIt first, InputIt last, std::input_iterator_tag ) {
		while (first != last)
			insert(pos++, *first++);
		return pos;
	}

	template <typename ForwardIt>
	constexpr iterator _insert( iterator pos, ForwardIt first, ForwardIt last, std::forward_iterator_tag ) {
		_createInsertionSpace(pos, distance(first, last));
		copy( first, last, pos );
		return pos;
	}

	template <typename U>
	constexpr iterator _insert( const_iterator pos, U&& u ) {
		auto it = _remcv(pos);
		_createInsertionSpace(it, 1);
		traits_type::assign(*it, std::forward<U>(u));
		return it;
	}

public:
	template <typename... Args>
	constexpr iterator emplace( const_iterator i, Args&&... args ) {
		// TODO: Avoid unnecessary copy (?)
		return insert(i, value_type(std::forward<Args>(args)...));
	}

	template <typename InputIt>
	constexpr requires<isInputIterator<InputIt>, iterator>
	insert( const_iterator pos, InputIt first, InputIt last ) {
		return _insert(_remcv(pos), first, last,
		               typename std::iterator_traits<InputIt>::iterator_category());
	}

	constexpr iterator insert( const_iterator pos, value_type const& v ) {return _insert(pos, v);}
	constexpr iterator insert( const_iterator pos, value_type     && v ) {return _insert(pos, std::move(v));}

	constexpr iterator insert( const_iterator pos, size_type c, const_reference v ) {
		_createInsertionSpace(pos, c);
		traits_type::assign(_remcv(pos), c, v);
		return _remcv(pos);
	}

	constexpr iterator insert( const_iterator i, std::initializer_list<value_type> ilist ) {
		return insert(i, ilist.begin(), ilist.end());
	}

	constexpr void assign( std::initializer_list<value_type> ilist ) {
		clear(); insert(ilist);
	}

	constexpr void assign( size_type count, const_reference value ) {
		clear();
		insert(begin(), count, value);
	}

	template <class InputIt>
	constexpr requires<isInputIterator<InputIt>>
	assign(InputIt first, InputIt last) {
		clear();
		while (first != last)
			emplace_back(*first++);
	}

	constexpr void pop_back() {Assert(not empty(), "Can't pop"); erase(end()-1);}

private:
	template <std::size_t OtherMax, typename OtherTraits>
	constexpr void _swap_trailing_elems( BasicVector<value_type, OtherMax, OtherTraits>& other, size_type len ) {
		// Use the trait of the destination string
		if (other.size()> size()) CopyTraits::move(_address(begin()+len), _address(other.begin() + len), distance(other.begin() + len, other.end()));
		else                     OtherTraits::move(_address(other.begin()+len),_address(begin()+len), distance(begin() + len, end()));
	}
	template <typename U, std::size_t OtherMax, typename OtherTraits>
	constexpr void _swap_trailing_elems( BasicVector<U, OtherMax, OtherTraits>& other, size_type len ) {
		if (other.size()> size()) move(other.begin() + len, other.end(), begin()+len);
		else                      move(begin() + len, end(), other.begin()+len);
	}

public:

	template <typename U, std::size_t OtherMax, typename OtherTraits>
	constexpr void swap( BasicVector<U, OtherMax, OtherTraits>& other ) {
		Assert( other.size() < max_size() && size() < OtherMax, "Swap fails" );

		auto min = std::min(other.size(), size());
		// TODO: Use CopyTraits::assign calls to swap the ranges
		swap_ranges(other.begin(), other.begin() + min, begin());
		_swap_trailing_elems(other, min);
		Constainer::swap(_size, other._size);
	}
};

template <typename T, std::size_t Size, typename Traits>
constexpr auto begin(BasicVector<T, Size, Traits> const& a) {return a.begin();}
template <typename T, std::size_t Size, typename Traits>
constexpr auto   end(BasicVector<T, Size, Traits> const& a) {return a.  end();}
template <typename T, std::size_t Size, typename Traits>
constexpr auto begin(BasicVector<T, Size, Traits>      & a) {return a.begin();}
template <typename T, std::size_t Size, typename Traits>
constexpr auto   end(BasicVector<T, Size, Traits>      & a) {return a.  end();}

// TODO: Implement comparison of BasicVector and Array

template <typename T1, std::size_t Size1, typename Traits1,
          typename T2, std::size_t Size2, typename Traits2>
constexpr bool operator==(BasicVector<T1, Size1, Traits1> const& lhs, BasicVector<T2, Size2, Traits2> const& rhs) {
	return lhs.size() == rhs.size() && Constainer::equal(lhs.begin(), lhs.end(), rhs.begin());
}
template <typename T1, std::size_t Size1, typename Traits1,
          typename T2, std::size_t Size2, typename Traits2>
constexpr bool operator<(BasicVector<T1, Size1, Traits1> const& lhs, BasicVector<T2, Size2, Traits2> const& rhs) {
	return Constainer::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename T1, std::size_t Size1, typename Traits1,
          typename T2, std::size_t Size2, typename Traits2>
constexpr bool operator!=(BasicVector<T1, Size1, Traits1> const& lhs, BasicVector<T2, Size2, Traits2> const& rhs) {
	return !(lhs == rhs);
}
template <typename T1, std::size_t Size1, typename Traits1,
          typename T2, std::size_t Size2, typename Traits2>
constexpr bool operator>=(BasicVector<T1, Size1, Traits1> const& lhs, BasicVector<T2, Size2, Traits2> const& rhs) {
	return !(lhs < rhs);
}
template <typename T1, std::size_t Size1, typename Traits1,
          typename T2, std::size_t Size2, typename Traits2>
constexpr bool operator>(BasicVector<T1, Size1, Traits1> const& lhs, BasicVector<T2, Size2, Traits2> const& rhs) {
	return rhs < lhs;
}
template <typename T1, std::size_t Size1, typename Traits1,
          typename T2, std::size_t Size2, typename Traits2>
constexpr bool operator<=(BasicVector<T1, Size1, Traits1> const& lhs, BasicVector<T2, Size2, Traits2> const& rhs) {
	return !(lhs > rhs);
}

template <typename T, std::size_t N>
using Vector = BasicVector<T, N, DefaultCopyTraits<T>>;

}

#endif // VECTOR_HXX_INCLUDED
