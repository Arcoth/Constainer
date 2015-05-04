/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef ALLOCATOR_HXX_INCLUDED
#define ALLOCATOR_HXX_INCLUDED

#include "Algorithms.hxx"
#include <utility> // forward

namespace Constainer {

template <typename T>
struct DefaultConstructor {

	using value_type = T;
	using pointer = T*;
	using const_pointer = T const*;
	using reference = T&;
	using const_reference = T const&;
	using size_type = std::size_t;

	/**< The default construct move-assigns a temporary constructed from the given arguments */
	template <typename... Args>
	constexpr void construct( pointer p, Args&&... args ) {
		*p = value_type(std::forward<Args>(args)...);
	}

	/**< If the argument is of type T, and if the assignment expression is valid, elide the temporary.
	     It may be that T only has a move assignment operator and the argument is an lvalue; SFINAE
	     is necessary to ensure that the above overload is selected in such cases */
	template <typename Arg>
	constexpr auto construct( pointer p, Arg&& arg )
		-> std::enable_if_t<std::is_same<std::remove_reference_t<Arg>, value_type>{},
		                    decltype(void(*p = std::forward<Arg>(arg)))>
	{
		*p = std::forward<Arg>(arg);
	}

	/**< By default, the quasi-destruction is vacuous. This function should never call destructors as we'd
	     have to call placement-new to ressurect the object, which is impossible in constant expressions. */
	constexpr void destroy( pointer ) {}

	constexpr auto select_on_container_copy_construction() const {return *this;}
};

// Trivial comparison
template <typename T>
constexpr bool operator==( DefaultConstructor<T> const& lhs, DefaultConstructor<T> const& rhs ) {return true;}
template <typename T>
constexpr bool operator!=( DefaultConstructor<T> const& lhs, DefaultConstructor<T> const& rhs ) {return false;}

template <typename Constructor>
struct ConstructorTraits {
	template <class T>
	static constexpr void destroy( Constructor& a, T* p ) {a.destroy(p);}

	template <class T, typename... Args>
	static constexpr void construct( Constructor& a, T* p, Args&&... args )
	{a.construct(p, std::forward<Args>(args)...);}

private:

	template <typename U>
	static constexpr auto _select_on_container_copy_construction(U const& a, int)
		-> decltype(Constructor(a.select_on_container_copy_construction()))
	{return a.select_on_container_copy_construction();}
	static constexpr Constructor _select_on_container_copy_construction(Constructor const& a, ...)
	{return a;}

public:
	static constexpr Constructor select_on_container_copy_construction(Constructor const& a)
	{return _select_on_container_copy_construction(a, 0);}

	// TODO: Extract these from Constructor, if present
	static constexpr std::false_type propagate_on_container_copy_assignment = {};
	static constexpr std::false_type propagate_on_container_move_assignment = {};
	static constexpr std::false_type propagate_on_container_swap = {};
};

/// CONSTRUCTIVE_ITERATOR:

/* This iterator saves a reference to its Constructor. This is to allow for stateful constructors. */
template <class Iterator, typename Constructor>
class ConstructiveIterator
	: public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	Iterator _it;
	Constructor* _constructor;


public:
	constexpr ConstructiveIterator(Iterator i, Constructor& c)
		: _it(i), _constructor(&c) {}

	constexpr ConstructiveIterator(ConstructiveIterator const&) = default;
	constexpr ConstructiveIterator& operator=(ConstructiveIterator const&) = default;

	template <typename Arg>
	constexpr ConstructiveIterator& operator=(Arg&& arg) {
		ConstructorTraits<Constructor>::construct(*_constructor, &*_it, std::forward<Arg>(arg));
		return *this;
	}

	constexpr ConstructiveIterator& operator*() { return *this; }

	constexpr ConstructiveIterator& operator++() {
		++_it;
		return *this;
	}

	constexpr ConstructiveIterator operator++(int) {
		auto tmp = *this;
		++_it;
		return tmp;
	}
};

/// Algorithms using ConstructiveIterator:

template <typename Iter, typename Constructor>
constexpr auto make_constructive_iterator( Iter i, Constructor& c ) {
	return ConstructiveIterator<Iter, Constructor>(i, c);
}

template <typename OutputIt, typename T, typename Constructor>
constexpr void uninitialized_fill( OutputIt first, OutputIt last, T const& v, Constructor& c ) {
	while (first != last)
		ConstructorTraits<Constructor>::construct(c, &*first++, v);
}

template <typename OutputIt, typename SizeType, typename T, typename Constructor>
constexpr void uninitialized_fill_n( OutputIt out, SizeType count, T const& v, Constructor& c ) {
	(fill_n)(make_constructive_iterator(out, c), count, v);
}

template <typename InputIt, typename OutputIt, typename Constructor>
constexpr auto uninitialized_copy( InputIt first, InputIt last, OutputIt out, Constructor& c ) {
	return (copy)(first, last, make_constructive_iterator(out, c));
}

template<class InputIt, class OutputIt, typename Constructor>
constexpr auto uninitialized_move(InputIt first, InputIt last, OutputIt out, Constructor& c) {
	return (move)(first, last, make_constructive_iterator(out, c));
}

template <typename InputIt, typename OutputIt, typename SizeType, typename Constructor>
constexpr auto uninitialized_move_n( InputIt first, SizeType count, OutputIt out, Constructor& c ) {
	return (move_n)( first, count, make_constructive_iterator(out, c) );
}

template <typename InputIt, typename OutputIt, typename SizeType, typename Constructor>
constexpr auto uninitialized_copy_n( InputIt first, SizeType count, OutputIt out, Constructor& c ) {
	return (copy_n)(first, count, make_constructive_iterator(out, c));
}

template <typename BiDir, typename BiDir2, typename Constructor>
constexpr auto uninitialized_copy_backward(BiDir  first, BiDir last, BiDir2 last2, Constructor& c) {
	while (last != first)
		ConstructorTraits<Constructor>::construct(c, *--last2, *--last);

	return last2;
}

template <typename BiDir, typename BiDir2, typename Constructor>
constexpr auto uninitialized_move_backward(BiDir first, BiDir last, BiDir2 last2, Constructor& c) {
	return (uninitialized_copy_backward)(make_move_iterator(first), make_move_iterator(last), last2);
}

template <typename InputIt, typename Constructor>
constexpr void destroy( InputIt first, InputIt last, Constructor& c ) {
	while (first != last)
		ConstructorTraits<Constructor>::destroy(c, &*first++);
}

template <typename InputIt, typename SizeType, typename Constructor>
constexpr void destroy_n( InputIt first, SizeType count, Constructor& c ) {
	for (; count > 0; --count)
		ConstructorTraits<Constructor>::destroy(c, &*first++);
}


}

#endif // ALLOCATOR_HXX_INCLUDED
