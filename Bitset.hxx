/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Algorithms.hxx"
#include "Math.hxx"
#include "String.hxx"

namespace Constainer {

template <STD::size_t N=defaultContainerSize>
class Bitset {
	// TODO: Support zero-sized bitsets?

	using size_type = STD::size_t;

	static constexpr auto _bits_per_chunk = 64;
	Array<STD::uint64_t, (N+_bits_per_chunk-1)/_bits_per_chunk> _storage;

	constexpr decltype(auto) _chunkof(size_type i)       {return _storage[i/_bits_per_chunk];}
	constexpr decltype(auto) _chunkof(size_type i) const {return _storage[i/_bits_per_chunk];}

	CONSTAINER_PURE_CONST static constexpr STD::uint64_t _mask_chunkof(size_type i) {
		return 1ull << i%_bits_per_chunk;
	}
	static constexpr STD::uint64_t _full_mask = -1;

	constexpr void _flip (size_type pos) {
		_chunkof(pos) ^= _mask_chunkof(pos);
	}
	constexpr void _reset(size_type pos) {
		_chunkof(pos) &= ~_mask_chunkof(pos);
	}
	constexpr void _set  (size_type pos, bool v) {
		_chunkof(pos) ^= (-v ^ _chunkof(pos)) & _mask_chunkof(pos);
	}
	constexpr bool _test (size_type pos) {
		return _chunkof(pos) & _mask_chunkof(pos);
	}

	constexpr void _nullify_trailbits() {
		_storage.back() &= _mask_chunkof(size()) - 1;
	}


public:

	class reference {
		Bitset* parent;
		size_type index;
		friend Bitset;
		constexpr reference(Bitset* p, size_type i) : parent(p), index(i) {}

	public:

		constexpr reference& operator=(reference const&) = default;
		constexpr reference& operator=(bool b) {
			parent->set(index, b);
			return *this;
		}

		constexpr operator bool() const {return parent->_test(index);}
		constexpr bool operator ~() const {return !operator bool();}

		constexpr reference& flip() {
			parent->_flip(index);
			return *this;
		}
	};

	constexpr reference operator[](size_type pos) {
		assert(pos < size());
		return {this, pos};
	}
	constexpr bool operator[](size_type pos) const {
		assert(pos < size());
		return test(pos);
	}

	constexpr size_type size() const {return N;}

	constexpr bool none() const {
		return Constainer::find(begin(_storage), end(_storage), 0, STD::not_equal_to<>()) == end(_storage);
	}
	constexpr bool any() const {return !none();}
	constexpr bool all() const {
		return Constainer::find(begin(_storage), end(_storage)-1, _full_mask, STD::not_equal_to<>()) == end(_storage)-1
		    && _storage.back() + 1 == _mask_chunkof(size());
	}

	constexpr Bitset() : _storage{} {}

	constexpr bool test(size_type i) const {
		assert(i < size());
		return _chunkof(i) & _mask_chunkof(i);
	}

	constexpr size_type count() const {
		size_type ret = 0;
		for (auto c : _storage)
			ret += popcount(c);
		return ret;
	}

	constexpr Bitset& set() {
		_storage.fill( _mask_chunkof(_bits_per_chunk-1)-1 );
		_nullify_trailbits();
		return *this;
	}
	constexpr Bitset& set(size_type pos, bool value = true) {
		assert(pos < size());
		_set(pos, value);
		return *this;
	}
	constexpr Bitset& set(size_type pos1, size_type pos2, bool value = true) {
	    // TODO: This can be made a good lot more efficient, and you know how.
		while (pos1 != pos2+1)
            set(pos1++, value);
		return *this;
	}

	constexpr Bitset& flip() {
		for (auto& c : _storage)
			c = ~c;
		_nullify_trailbits();
		return *this;
	}
	constexpr Bitset& flip(size_type pos) {
		assert(pos < size());
		_flip(pos);
		return *this;
	}

	constexpr Bitset& reset() {
		Constainer::fill(Constainer::begin(_storage), Constainer::end(_storage), 0);
		return *this;
	}
	constexpr Bitset& reset(size_type pos) {
		assert(pos < size());
		_reset(pos);
		return *this;
	}

	constexpr size_type leading(bool value) const {
		STD::size_t count = 0;
		for (auto& c : _storage) {
			auto v = value? ~c : c;
			if (v != 0)
				return STD::min<size_type>(size(), count++*_bits_per_chunk + count_trailing(v));
		}

		return size();
	}

	template <typename CharT = char, STD::size_t MaxN = N,
	          typename CharTraits=Constainer::CharTraits<CharT>>
	constexpr BasicString<CharT, MaxN, CharTraits> to_string(CharT zero='0', CharT one='1') const
	{
		BasicString<CharT, MaxN, CharTraits> str; str.resize(size(), zero);
		// TODO: More efficient?
		for (size_type i = 0; i != size(); ++i)
			if (test(i))
				CharTraits::assign(str[i], one);
		return str;
	}

private:

	template <typename Op>
	friend constexpr Bitset _apply( Bitset const& lhs, Bitset const& rhs, Op op ) {
		auto rval = lhs;
		auto ptr = rval._storage.data();
		for (auto i : rhs._storage) {
			*ptr = op(*ptr, i);
			++ptr;
		}
		return rval;
	}

public:

	friend constexpr Bitset operator|( Bitset const& lhs, Bitset const& rhs ) {
		return _apply(lhs, rhs, STD::bit_or<>());
	}
	friend constexpr Bitset operator&( Bitset const& lhs, Bitset const & rhs ) {
		return _apply(lhs, rhs, STD::bit_and<>());
	}
	friend constexpr Bitset operator^( Bitset const& lhs, Bitset const& rhs ) {
		return _apply(lhs, rhs, STD::bit_xor<>());
	}
};



}
