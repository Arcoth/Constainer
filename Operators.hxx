/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

namespace Constainer {

	template <typename T>
	struct EqualityComparable {
		constexpr friend bool operator!=(T const& lhs, T const& rhs) {return !(lhs == rhs);}
	};
	template <typename T>
	struct LessThanComparable : EqualityComparable<T> {
		constexpr friend bool operator>(T const& lhs, T const& rhs)  {return   rhs < lhs;}
		constexpr friend bool operator<=(T const& lhs, T const& rhs) {return !(rhs < lhs);}
		constexpr friend bool operator>=(T const& lhs, T const& rhs) {return !(lhs < rhs);}
	};
	template <typename T>
	struct TotallyOrdered : EqualityComparable<T>, LessThanComparable<T> {};

	template <typename T>
	struct Decrementable {
		friend constexpr T operator--(T& x, int) {
			T nrv(x);
			--x;
			return nrv;
		}
	};
	template <typename T>
	struct Incrementable {
		friend constexpr T operator++(T& x, int) {
			T nrv(x);
			++x;
			return nrv;
		}
	};
	template <typename T, typename P>
	struct Dereferencable {
		constexpr P operator->() const {
			return Constainer::addressof(*static_cast<const T&>(*this));
		}
	};
	template <typename T, typename I, typename R>
	struct Indexable {
		constexpr R operator[](I n) const {
			return *(static_cast<const T&>(*this) + n);
		}
	};
	template <typename T>
	struct Addable {
		constexpr friend T operator+(T const& lhs, T const& rhs) {
			T nrv(lhs); nrv += rhs;
			return nrv;
		}
	};
	template <typename T, typename U>
	struct Addable2 {
		constexpr friend T operator+(T const& lhs, U const& rhs) {
			T nrv(lhs); nrv += rhs;
			return nrv;
		}
		constexpr friend T operator+(U const& lhs, T const& rhs) {
			return rhs + lhs;
		}
	};
	template <typename T, typename U>
	struct Subtractable2 {
		constexpr friend T operator-(T const& lhs, U const& rhs) {
			T nrv(lhs); nrv -= rhs;
			return nrv;
		}
	};
	template <typename T>
	struct Subtractable : Subtractable2<T, T> {};
	template <typename T>
	struct Additive : Addable<T>, Subtractable<T> {};
	template <typename T, typename D>
	struct Additive2 : Addable2<T, D>, Subtractable2<T, D> {};

	template <typename I, typename P>
	struct InputIteratable : EqualityComparable<I>, Incrementable<I>, Dereferencable<I, P> {};
	template <typename I>
	struct OutputIteratable : Incrementable<I> {};
	template <typename I, typename P>
	struct ForwardIteratable : InputIteratable<I, P> {};
	template <typename I, typename P>
	struct BidirectionalIteratable : ForwardIteratable<I, P>, Decrementable<I> {};

	template <typename I, typename P, typename R, typename D>
	struct RandomAccessIteratable : BidirectionalIteratable<I, P>, TotallyOrdered<I>, Additive2<I, D>, Indexable<I, D, R> {};

}
