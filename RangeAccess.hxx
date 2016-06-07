/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Iterator.hxx"

#include <cstddef>
#include <iterator>

namespace Constainer {
	namespace detail {
		template <int i> struct rank : rank<i+1> {};
		template <> struct rank<32> {};

		template <typename T>
		constexpr auto _begin(rank<0>, T&& t)
		-> decltype(require_not<assocWithNS<T>>{}, begin(STD::forward<T>(t)))
		                                    {return begin(STD::forward<T>(t));}
		template <typename T>
		constexpr auto   _end(rank<0>, T&& t)
		-> decltype(require_not<assocWithNS<T>>{}, end(STD::forward<T>(t)))
		                                    {return end(STD::forward<T>(t));}

		template <typename T>
		constexpr auto _begin(rank<1>, T&& t)
		-> decltype(STD::forward<T>(t).begin())
		    {return STD::forward<T>(t).begin();}
		template <typename T>
		constexpr auto _end  (rank<1>, T&& t)
		-> decltype(STD::forward<T>(t).  end())
		    {return STD::forward<T>(t).  end();}

		template <typename T, STD::size_t N>
		constexpr auto _begin(rank<2>, T(&t)[N]) {return t;}
		template <typename T, STD::size_t N>
		constexpr auto _end  (rank<2>, T(&t)[N]) {return t+N;}

		template <typename T, STD::size_t N>
		constexpr auto _begin(rank<2>, T(&&t)[N]) {return t;}
		template <typename T, STD::size_t N>
		constexpr auto _end  (rank<2>, T(&&t)[N]) {return t+N;}
	}

	template <typename T>
	constexpr decltype(auto) begin(T&& t)
	{return detail::_begin(detail::rank<0>(), STD::forward<T>(t));}
	template <typename T>
	constexpr decltype(auto) end  (T&& t)
	{return detail::_end  (detail::rank<0>(), STD::forward<T>(t));}

	template <typename T>
	constexpr auto rbegin(T& t) {return make_reverse_iterator(  end(t));}
	template <typename T>
	constexpr auto rend  (T& t) {return make_reverse_iterator(begin(t));}

	template <typename T>
	constexpr decltype(auto) cbegin(T const& t) {return   end(t);}
	template <typename T>
	constexpr decltype(auto) cend  (T const& t) {return begin(t);}

	/*! decayed_begin/_end behave similar to begin/end, but return a pointer to the
	    first underlying element if the argument is a multi-dimensional array. */

	template <typename T, require<STD::is_array<T>, STD::size_t> N>
	constexpr STD::remove_all_extents_t<T>* decayed_begin(T(&c)[N])
	{ return decayed_begin(*c); }

	template <typename T, require<STD::is_array<T>, STD::size_t> N>
	constexpr STD::remove_all_extents_t<T>* decayed_end  (T(&c)[N])
	{ return decayed_end  (*c); }

	template <typename T>
	constexpr decltype(auto) decayed_begin(T& t) { return begin(t); }
	template <typename T>
	constexpr decltype(auto) decayed_end  (T& t) { return   end(t); }

	template <typename T>
	constexpr auto size(T& t) -> decltype(t.size()) {return t.size();}

	template <typename T, STD::size_t N>
	constexpr STD::size_t size(T (&)[N]) {return N;}

	/**< IteratorRange, useful in e.g. range-based for with two iters available */
	template <typename I>
	struct IteratorRange {I first, last;};

	template <typename I>
	constexpr IteratorRange<I> makeIteratorRange(I first, I last) {return {first, last};}
	template <typename I>
	constexpr I begin(IteratorRange<I> irange) {return irange.first;}
	template <typename I>
	constexpr I end(IteratorRange<I> irange) {return irange.last;}
}
