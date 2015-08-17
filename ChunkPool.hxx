/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Array.hxx"
#include "Bitset.hxx"

namespace Constainer {

template <typename T, std::size_t N>
class ChunkPool : private Array<T, N> {

	using _base = Array<T, N>;

public:

	template <typename U>
	using rebind = ChunkPool<U, N>;

	using typename _base::size_type;
	using typename _base::value_type;
	using typename _base::pointer;
	using typename _base::const_pointer;
	using typename _base::reference;
	using typename _base::const_reference;

	constexpr std::size_t max_size() const {return _base::size();}

private:
	Bitset<N> _used;

public:

	constexpr ChunkPool() : _base{}, _used{} {}

	constexpr size_type used() const {
		return _used.count();
	}

	constexpr bool available() const {
		return !_used.all();
	}

	constexpr pointer grab() {
		auto pos = _used.leading(1);
		AssertExcept<std::bad_alloc>(pos != this->size());
		_used.set(pos);
		auto& ref = _base::operator[](pos);
		ref = value_type();
		return &ref;
	}

	constexpr void free() {
		_used.reset();
	}
	constexpr void free( const_pointer p ) {
		auto i = p-_base::data();
		Assert( _used.test(i) );
		_used.reset(i);
	}
};

}
