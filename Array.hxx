/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Iterator.hxx"

#include "impl/Algorithms/BinarySeqOps.hxx"
#include "impl/Algorithms/Transformations.hxx"

#include <cstddef> // size_t
#include <algorithm> // STD::max

namespace Constainer {

template <typename T, STD::size_t Size = defaultContainerSize>
struct Array {

	using size_type = STD::size_t;
	CONSTAINER_PURE_CONST static auto constexpr size() {return Size;}

	using difference_type = STD::ptrdiff_t;

	using value_type = T;

	using         pointer = value_type      *;
	using   const_pointer = value_type const*;

	using       reference = value_type      &;
	using const_reference = value_type const&;

	using       iterator =       pointer;
	using const_iterator = const_pointer;

	using const_reverse_iterator = Constainer::reverse_iterator<const_iterator>;
	using       reverse_iterator = Constainer::reverse_iterator<      iterator>;

	T _storage[STD::max(size(), {1})];

	constexpr pointer       data()       {return _storage;}
	constexpr const_pointer data() const {return _storage;}

	constexpr       reference front()       {return *begin();}
	constexpr const_reference front() const {return *begin();}
	constexpr       reference back ()       {return end()[-1];}
	constexpr const_reference back () const {return end()[-1];}

	constexpr iterator        begin()       {return _storage         ;}
	constexpr const_iterator  begin() const {return _storage         ;}
	constexpr iterator        end  ()       {return _storage + size();}
	constexpr const_iterator  end  () const {return _storage + size();}
	constexpr const_iterator cbegin() const {return begin();}
	constexpr const_iterator cend  () const {return   end();}

	constexpr reverse_iterator        rbegin()       {return       reverse_iterator(  end());}
	constexpr const_reverse_iterator  rbegin() const {return const_reverse_iterator(  end());}
	constexpr reverse_iterator        rend  ()       {return       reverse_iterator(begin());}
	constexpr const_reverse_iterator  rend  () const {return const_reverse_iterator(begin());}
	constexpr const_reverse_iterator crbegin() const {return rbegin();}
	constexpr const_reverse_iterator crend  () const {return rend();}


	constexpr reference       operator[](size_type index)       {return _storage[index];}
	constexpr const_reference operator[](size_type index) const {return _storage[index];}

	constexpr void fill(T const& value) {
		Constainer::fill( begin(), end(), value );
	}

	constexpr void swap(Array& other) {
		Constainer::swap( _storage, other._storage );
	}
};

template <typename T, STD::size_t N>
constexpr void swap( Array<T, N>& lhs, Array<T, N>& rhs ) {
	lhs.swap(rhs);
}

template <typename T1, STD::size_t N1, typename T2, STD::size_t N2>
constexpr bool operator<( Array<T1, N1> const& lhs, Array<T2, N2> const& rhs ){
	return N1 <= N2 && (N1 < N2 || lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}
template <typename T1, STD::size_t N1, typename T2, STD::size_t N2>
constexpr bool operator>( Array<T1, N1> const& lhs, Array<T2, N2> const& rhs ){
	return rhs < lhs;
}
template <typename T1, STD::size_t N1, typename T2, STD::size_t N2>
constexpr bool operator==( Array<T1, N1> const& lhs, Array<T2, N2> const& rhs ){
	return equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename T1, STD::size_t N1, typename T2, STD::size_t N2>
constexpr bool operator!=( Array<T1, N1> const& lhs, Array<T2, N2> const& rhs ){
	return !(lhs == rhs);
}
template <typename T1, STD::size_t N1, typename T2, STD::size_t N2>
constexpr bool operator<=( Array<T1, N1> const& lhs, Array<T2, N2> const& rhs ){
	return !(lhs > rhs);
}
template <typename T1, STD::size_t N1, typename T2, STD::size_t N2>
constexpr bool operator>=( Array<T1, N1> const& lhs, Array<T2, N2> const& rhs ){
	return !(lhs < rhs);
}

}
