/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef RANGEACCESS_HXX_INCLUDED
#define RANGEACCESS_HXX_INCLUDED

#include <cstddef>
#include <iterator>

#define CONSTAINER_AUTORET(...) -> decltype(__VA_ARGS__) {return __VA_ARGS__;}

namespace Constainer {
	namespace RA_detail {
		namespace detail {
		template <int i> struct rank : rank<i+1> {};
		template <> struct rank<10> {};

		template <typename T>
		constexpr auto _begin(rank<0>, T& t) CONSTAINER_AUTORET(begin(t))
		template <typename T>
		constexpr auto _end  (rank<0>, T& t) CONSTAINER_AUTORET(  end(t))

		template <typename T>
		constexpr auto _begin(rank<1>, T& t) CONSTAINER_AUTORET(t.begin())
		template <typename T>
		constexpr auto _end  (rank<1>, T& t) CONSTAINER_AUTORET(t.  end())

		template <typename T, std::size_t N>
		constexpr auto _begin(rank<2>, T(&t)[N]) CONSTAINER_AUTORET(&*t)
		template <typename T, std::size_t N>
		constexpr auto _end  (rank<2>, T(&t)[N]) CONSTAINER_AUTORET(t+N)
		}

		template <typename T>
		constexpr decltype(auto) begin(T&& t)
		{return detail::_begin(detail::rank<0>(), std::forward<T>(t));}
		template <typename T>
		constexpr decltype(auto) end  (T&& t)
		{return detail::_end  (detail::rank<0>(), std::forward<T>(t));}
	}
	// Prevents a recursive instantiation via ADL of Constainer::-members
	using namespace RA_detail;

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

	template <typename T, std::enable_if_t<std::is_array<T>{}, std::size_t> N>
	constexpr std::remove_all_extents_t<T>* decayed_begin(T(&c)[N])
	{ return decayed_begin(*c); }

	template <typename T, std::enable_if_t<std::is_array<T>{}, std::size_t> N>
	constexpr std::remove_all_extents_t<T>* decayed_end  (T(&c)[N])
	{ return decayed_end  (*c); }

	template <typename T>
	constexpr decltype(auto) decayed_begin(T& t) { return begin(t); }
	template <typename T>
	constexpr decltype(auto) decayed_end  (T& t) { return   end(t); }

	template <typename T>
	constexpr auto size(T& t) -> decltype(t.size()) {return t.size();}

	template <typename T, std::size_t N>
	constexpr std::size_t size(T (&t)[N]) {return N;}
}

#undef CONSTAINER_AUTORET

#endif // RANGEACCESS_HXX_INCLUDED
