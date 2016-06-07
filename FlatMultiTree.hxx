/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "FlatTree.hxx"
#include "Algorithms.hxx"

namespace Constainer { namespace detail {

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Container>
class FlatMultiTree
	: public      FlatTree<Key, Value, KeyOfValue, Compare, Container> {
	using _base = FlatTree<Key, Value, KeyOfValue, Compare, Container>;

	using typename _base::_iter_pair;
	using typename _base::_const_iter_pair;

	using _base::insert_unique;
	using _base::insert_equal;
	using _base::lower_bound_range;
	using _base::emplace_equal;
	using _base::emplace_hint_equal;
	using _base::emplace_unique;
	using _base::emplace_hint_unique;

public:

	// Convenience in this template definition
	using typename _base::key_type;
	using typename _base::value_type;
	using typename _base::key_compare;
	using typename _base::value_compare;
	using typename _base::iterator;
	using typename _base::const_iterator;
	using typename _base::const_reference;

	constexpr FlatMultiTree() : _base{} {}
	constexpr FlatMultiTree(FlatMultiTree const&) = default;
	constexpr FlatMultiTree(FlatMultiTree &&) = default;
	constexpr FlatMultiTree& operator=(FlatMultiTree const&) = default;
	constexpr FlatMultiTree& operator=(FlatMultiTree &&) = default;

	constexpr explicit FlatMultiTree( value_compare const& comp ) : _base(comp) {}

	template <typename InputIt>
	constexpr FlatMultiTree(InputIt first, InputIt last, value_compare const& comp) :
		_base(STD::false_type{}, first, last, comp) {}
	template <typename InputIt>
	constexpr FlatMultiTree(InputIt first, InputIt last) :
		FlatMultiTree(first, last, value_compare{}) {}
	template <typename InputIt>
	constexpr FlatMultiTree(ordered_range_t, InputIt first, InputIt last, value_compare const& comp) :
		_base(ordered_range, first, last, comp) {}
	template <typename InputIt>
	constexpr FlatMultiTree(ordered_range_t, InputIt first, InputIt last) :
		FlatMultiTree(ordered_range, first, last, value_compare{}) {}

	constexpr FlatMultiTree(STD::initializer_list<value_type> ilist) :
		FlatMultiTree(ilist.begin(), ilist.end()) {}
	constexpr FlatMultiTree(ordered_range_t, STD::initializer_list<value_type> ilist) :
		FlatMultiTree(ordered_range, ilist.begin(), ilist.end()) {}
	constexpr FlatMultiTree(STD::initializer_list<value_type> ilist, value_compare const& vcmp) :
		FlatMultiTree(ilist.begin(), ilist.end(), vcmp) {}
	constexpr FlatMultiTree(ordered_range_t, STD::initializer_list<value_type> ilist, value_compare const& vcmp) :
		FlatMultiTree(ordered_range, ilist.begin(), ilist.end(), vcmp) {}

	constexpr STD::pair<iterator, bool> insert(const_reference val) {
		return _base::insert_equal(val);
	}
	constexpr STD::pair<iterator, bool> insert(value_type&& val) {
		return _base::insert_equal(STD::move(val));
	}
	constexpr iterator insert(const_iterator hint, const_reference val) {
		return _base::insert_equal(hint, val);
	}
	constexpr iterator insert(const_iterator hint, value_type&& val) {
		return _base::insert_equal(hint, STD::move(val));
	}

	template <typename InputIt>
	constexpr void insert(InputIt first, InputIt last) {
		_base::insert_equal(first, last);
	}
	constexpr void insert(STD::initializer_list<value_type> ilist) {
		insert(ilist.begin(), ilist.end());
	}
	template <typename InputIt>
	constexpr void insert(ordered_range_t, InputIt first, InputIt last) {
		_base::insert_equal(ordered_range, first, last);
	}
	constexpr void insert(ordered_range_t, STD::initializer_list<value_type> ilist) {
		insert(ordered_range, ilist.begin(), ilist.end());
	}

	template <typename... Args>
	constexpr STD::pair<iterator, bool> emplace(Args&&... args) {
		return this->emplace_equal(STD::forward<Args>(args)...);
	}
	template <typename... Args>
	constexpr iterator emplace_hint(const_iterator hint, Args&&... args) {
		return this->emplace_hint_equal(hint, STD::forward<Args>(args)...);
	}
};

}}
