/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "Vector.hxx"

namespace Constainer {

template <typename T, typename Container = Vector<T>>
struct Stack
{
	using container_type = Container;
	using value_type =  typename container_type::value_type;
	using size_type = typename container_type::size_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;

	Container c;

	constexpr Stack(container_type const& c) : c(c) {}
	constexpr Stack(container_type && c = {}) : c(std::move(c)) {}

	constexpr Stack(Stack const& s) = default;
	constexpr Stack(Stack && s) = default;
	constexpr Stack& operator=(Stack const& s) = default;
	constexpr Stack& operator=(Stack && s) = default;

	constexpr reference top() {return c.back();}
	constexpr const_reference top() const {return c.back();}

	constexpr bool empty() const {return c.empty();}
	constexpr size_type size() const {return c.size();}

	template <typename... Args>
	constexpr void emplace(Args&&... args) {
		c.emplace_back(std::forward<Args>(args)...);
	}
	constexpr void push(const_reference r) {c.push_back(r);}
	constexpr void push(value_type&&    r) {c.push_back(std::move(r));}

	constexpr void pop() {c.pop_back();}

	constexpr void swap(Stack& other) {swap(c, other.c);}
};

}
