/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "FlatTree.hxx"
#include "Algorithms.hxx"

namespace Constainer { namespace detail {

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Container>
class FlatUniqueTree
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
	using _base::equal_range;

public:

	// Convenience in this template definition
	using typename _base::key_type;
	using typename _base::value_type;
	using typename _base::key_compare;
	using typename _base::value_compare;
	using typename _base::iterator;
	using typename _base::const_iterator;
	using typename _base::const_reference;

	constexpr       _iter_pair equal_range(key_type const& key)       {return lower_bound_range(key);}
	constexpr _const_iter_pair equal_range(key_type const& key) const {return lower_bound_range(key);}
	template <typename T>
	constexpr has_is_transparent<key_compare,      _iter_pair> equal_range(T const& t)       {return lower_bound_range(t);}
	template <typename T>
	constexpr has_is_transparent<key_compare, _const_iter_pair> equal_range(T const& t) const {return lower_bound_range(t);}

	constexpr FlatUniqueTree() : _base{} {}
	constexpr FlatUniqueTree(FlatUniqueTree const&) = default;
	constexpr FlatUniqueTree(FlatUniqueTree &&) = default;
	constexpr FlatUniqueTree& operator=(FlatUniqueTree const&) = default;
	constexpr FlatUniqueTree& operator=(FlatUniqueTree &&) = default;

	explicit FlatUniqueTree( value_compare const& comp ) : _base(comp) {}

	template <typename InputIt>
	constexpr FlatUniqueTree(InputIt first, InputIt last, value_compare const& comp) :
		_base(std::true_type{}, first, last, comp) {}
	template <typename InputIt>
	constexpr FlatUniqueTree(InputIt first, InputIt last) :
		FlatUniqueTree(first, last, value_compare{}) {}
	template <typename InputIt>
	constexpr FlatUniqueTree(ordered_unique_range_t, InputIt first, InputIt last, value_compare const& comp) :
		_base(ordered_range, first, last, comp) {}
	template <typename InputIt>
	constexpr FlatUniqueTree(ordered_unique_range_t, InputIt first, InputIt last) :
		FlatUniqueTree(ordered_unique_range, first, last, value_compare{}) {}

	constexpr FlatUniqueTree(std::initializer_list<value_type> ilist) :
		FlatUniqueTree(ilist.begin(), ilist.end()) {}
	constexpr FlatUniqueTree(ordered_unique_range_t, std::initializer_list<value_type> ilist) :
		FlatUniqueTree(ordered_unique_range, ilist.begin(), ilist.end()) {}
	constexpr FlatUniqueTree(std::initializer_list<value_type> ilist, value_compare const& vcmp) :
		FlatUniqueTree(ilist.begin(), ilist.end(), vcmp) {}
	constexpr FlatUniqueTree(ordered_unique_range_t, std::initializer_list<value_type> ilist, value_compare const& vcmp) :
		FlatUniqueTree(ordered_unique_range, ilist.begin(), ilist.end(), vcmp) {}

	constexpr std::pair<iterator, bool> insert(const_reference val) {
		return _base::insert_unique(val);
	}
	constexpr std::pair<iterator, bool> insert(value_type&& val) {
		return _base::insert_unique(std::move(val));
	}
	constexpr iterator insert(const_iterator hint, const_reference val) {
		return _base::insert_unique(hint, val);
	}
	constexpr iterator insert(const_iterator hint, value_type&& val) {
		return _base::insert_unique(hint, std::move(val));
	}

	template <typename InputIt>
	constexpr void insert(InputIt first, InputIt last) {
		_base::insert_unique(first, last);
	}
	constexpr void insert(std::initializer_list<value_type> ilist) {
		insert(ilist.begin(), ilist.end());
	}
	template <typename InputIt>
	constexpr void insert(ordered_unique_range_t, InputIt first, InputIt last) {
		_base::insert_unique(ordered_unique_range, first, last);
	}
	constexpr void insert(ordered_unique_range_t, std::initializer_list<value_type> ilist) {
		insert(ordered_unique_range, ilist.begin(), ilist.end());
	}

	template <typename... Args>
	constexpr std::pair<iterator, bool> emplace(Args&&... args) {
		return this->emplace_unique(std::forward<Args>(args)...);
	}
	template <typename... Args>
	constexpr iterator emplace_hint(const_iterator hint, Args&&... args) {
		return this->emplace_hint_unique(hint, std::forward<Args>(args)...);
	}
};

}}
