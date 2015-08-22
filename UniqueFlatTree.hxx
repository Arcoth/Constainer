/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "FlatTree.hxx"
#include "Algorithms.hxx"

namespace Constainer { namespace detail {

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Container>
class UniqueFlatTree
	: public      FlatTree<Key, Value, KeyOfValue, Compare, Container> {
	using _base = FlatTree<Key, Value, KeyOfValue, Compare, Container>;

	using typename _base::_iter_pair;
	using typename _base::_const_iter_pair;

	using _base::insert_unique;
	using _base::insert_equal;
	using _base::lower_bound_range;
	// TODO: COMPLETE

public:

	using typename _base::iterator;
	using typename _base::const_iterator;
	using typename _base::reverse_iterator;
	using typename _base::const_reverse_iterator;

	using typename _base::value_type;
	using typename _base::key_type;
	using typename _base::value_compare;
	using typename _base::key_compare;
	using typename _base::pointer;
	using typename _base::reference;
	using typename _base::const_pointer;
	using typename _base::const_reference;
	using typename _base::size_type;

	using _base::key_comp;
	using _base::value_comp;
	using _base::clear;
	using _base::erase;
	using _base::empty;
	using _base::size;
	using _base::max_size;

	using _base::begin;
	using _base::end;
	using _base::cbegin;
	using _base::cend;
	using _base::crbegin;
	using _base::crend;

	using _base::lower_bound;
	using _base::upper_bound;
	using _base::find;
	using _base::count;

	constexpr       _iter_pair equal_range(key_type const& key)       {return lower_bound_range(key);}
	constexpr _const_iter_pair equal_range(key_type const& key) const {return lower_bound_range(key);}
	template <typename T>
	constexpr has_is_transparent<key_compare,      _iter_pair> equal_range(T const& t)       {return lower_bound_range(t);}
	template <typename T>
	constexpr has_is_transparent<key_compare, _const_iter_pair> equal_range(T const& t) const {return lower_bound_range(t);}

	constexpr UniqueFlatTree() : _base{} {}
	constexpr UniqueFlatTree(UniqueFlatTree const&) = default;
	constexpr UniqueFlatTree(UniqueFlatTree &&) = default;
	constexpr UniqueFlatTree& operator=(UniqueFlatTree const&) = default;
	constexpr UniqueFlatTree& operator=(UniqueFlatTree &&) = default;

	explicit UniqueFlatTree( value_compare const& comp ) : _base(comp) {}

	template <typename InputIt>
	constexpr UniqueFlatTree(InputIt first, InputIt last, value_compare const& comp) :
		_base(std::true_type{}, first, last, comp) {}
	template <typename InputIt>
	constexpr UniqueFlatTree(InputIt first, InputIt last) :
		UniqueFlatTree(first, last, value_compare{}) {}
	template <typename InputIt>
	constexpr UniqueFlatTree(ordered_unique_range_t, InputIt first, InputIt last, value_compare const& comp) :
		_base(ordered_range, first, last, comp) {}
	template <typename InputIt>
	constexpr UniqueFlatTree(ordered_unique_range_t, InputIt first, InputIt last) :
		UniqueFlatTree(ordered_unique_range, first, last, value_compare{}) {}

	constexpr UniqueFlatTree(std::initializer_list<value_type> ilist) :
		UniqueFlatTree(ilist.begin(), ilist.end()) {}
	constexpr UniqueFlatTree(ordered_unique_range_t, std::initializer_list<value_type> ilist) :
		UniqueFlatTree(ordered_unique_range, ilist.begin(), ilist.end()) {}
	constexpr UniqueFlatTree(std::initializer_list<value_type> ilist, value_compare const& vcmp) :
		UniqueFlatTree(ilist.begin(), ilist.end(), vcmp) {}
	constexpr UniqueFlatTree(ordered_unique_range_t, std::initializer_list<value_type> ilist, value_compare const& vcmp) :
		UniqueFlatTree(ordered_unique_range, ilist.begin(), ilist.end(), vcmp) {}

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
