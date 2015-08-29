/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "impl/Fundamental.hxx"
#include "Vector.hxx"
#include "ChunkPool.hxx"
#include "Operators.hxx"

namespace Constainer {

template <typename T, typename Pool>
class BasicStableVector
{
	struct Node;

public:

	using pool_type = typename Pool::template rebind<Node>;

	using value_type      = T;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference       = value_type&;
	using const_reference = value_type const&;
	using pointer         = value_type*;
	using const_pointer   = value_type const*;

	static constexpr size_type max_size() {return pool_type::max_size();}

private:
	template <bool _const>
	class _iterator_base;
	using _pointer_container = Vector<Node*, max_size()+1>;
	using _pointer_iter = Node**;

public:

	using       iterator = _iterator_base<false>;
	using const_iterator = _iterator_base<true>;

	using       reverse_iterator = Constainer::reverse_iterator<      iterator>;
	using const_reverse_iterator = Constainer::reverse_iterator<const_iterator>;

private:
	struct Node {
		T value = {};
		using _pointer_iter = Node**;
		_pointer_iter up = nullptr;

		constexpr Node(const_reference value, _pointer_iter up)
			: value(value), up(up) {}
		constexpr Node() = default;

		constexpr Node& operator=(Node const&) = default;
		constexpr Node& operator=(Node && n) {value = std::move(n.value); up = n.up; return *this;}
		constexpr Node(Node const&) = default;
		constexpr Node(Node &&) = default;

		constexpr void setParent(_pointer_iter it) {
			up = it;
			*up = this;
		}
		template <typename... Args>
		constexpr void setParent(_pointer_iter it, Args&&... args) {
			value = value_type(std::forward<Args>(args)...); setParent(it);
		}
		// Avoid superfluous temporary if possible:
		template <typename Arg>
		constexpr auto setParent(_pointer_iter it, Arg&& arg)
			-> require<std::is_same<std::decay_t<Arg>, value_type>> {
			value = std::forward<Arg>(arg); setParent(it);
		}
	};

	template <bool _const>
	class _iterator_base : public RandomAccessIteratable<_iterator_base<_const>,
	                                                     std::conditional_t<_const, const_pointer  , pointer  >,
	                                                     std::conditional_t<_const, const_reference, reference>, std::ptrdiff_t> {
	public:
		using difference_type   = std::ptrdiff_t;
		using pointer           = std::conditional_t<_const, BasicStableVector::const_pointer, BasicStableVector::pointer>;
		using reference         = std::conditional_t<_const, BasicStableVector::const_reference, BasicStableVector::reference>;
		using value_type        = BasicStableVector::value_type;
		using iterator_category = std::random_access_iterator_tag;

	private:
		Node* _node;

		friend BasicStableVector;

		friend constexpr _iterator_base<false> unconstifyIterator( BasicStableVector&, _iterator_base c ) {return c;}

		constexpr explicit _iterator_base(Node* p) : _node(p) {}

		template <bool _const2, std::enable_if_t<(_const2 > _const), int> = 0>
		constexpr _iterator_base(_iterator_base<_const2> i) : _node(i._node) {}

	public:

		template <bool _const2, std::enable_if_t<(_const2 <= _const), long> = 0>
		constexpr _iterator_base(_iterator_base<_const2> i) : _node(i._node) {}

		constexpr _iterator_base(_iterator_base const& i) : _node(i._node) {}

		constexpr _iterator_base() : _iterator_base(nullptr) {}

		constexpr reference operator*() const {return _node->value;}

		constexpr _iterator_base& operator+=(difference_type d) {return *this = _iterator_base(_node->up[ d]);}
		constexpr _iterator_base& operator-=(difference_type d) {return *this = _iterator_base(_node->up[-d]);}

		constexpr _iterator_base& operator++() {return *this += 1;}
		constexpr _iterator_base& operator--() {return *this -= 1;}

		constexpr difference_type operator-(_iterator_base rhs) const {
			return _node->up - rhs._node->up;
		}

		constexpr bool operator==(_iterator_base rhs) const {return _node == rhs._node;}
		constexpr bool operator <(_iterator_base rhs) const {return _node->up < rhs._node->up;}
	};

	constexpr _pointer_iter _piter_of(const_iterator it) {return it._node->up;}

public:

	constexpr const_iterator  begin() const {return iterator(_pointers.front());}
	constexpr const_iterator    end() const {return iterator(_pointers.back());}
	constexpr       iterator  begin()       {return as_const(*this).begin();}
	constexpr       iterator    end()       {return as_const(*this).end();}
	constexpr const_iterator cbegin() const {return begin();}
	constexpr const_iterator   cend() const {return end();}

	constexpr       reverse_iterator  rbegin()       {return reverse_iterator(end());}
	constexpr       reverse_iterator    rend()       {return reverse_iterator(begin());}
	constexpr const_reverse_iterator  rbegin() const {return rbegin();}
	constexpr const_reverse_iterator    rend() const {return rend();}
	constexpr const_reverse_iterator crbegin() const {return rbegin();}
	constexpr const_reverse_iterator   crend() const {return rend();}

private:

	pool_type _pool {};
	_pointer_container _pointers {&_endNode};
	Node _endNode{{}, _pointers.begin()};

	constexpr void _append(BasicStableVector const& s) {
		insert(end(), s.begin(), s.end());
	}
	constexpr void _append(BasicStableVector && s) {
		insert(end(), Constainer::make_move_iterator(s.begin()),
		              Constainer::make_move_iterator(s.end()));
		s.clear();
	}

public:

	constexpr void clear() {_pool.free(); _pointers.clear();}

	constexpr size_type size() const {return _pointers.size()-1;}
	constexpr bool     empty() const {return size() == 0;}

	constexpr       reference front()       {return _pointers.front()->value;}
	constexpr       reference  back()       {return _pointers. back()->value;}
	constexpr const_reference front() const {return _pointers.front()->value;}
	constexpr const_reference  back() const {return _pointers. back()->value;}

	constexpr       reference operator[](size_type s)       {return _pointers[s]->value;}
	constexpr const_reference operator[](size_type s) const {return _pointers[s]->value;}

	constexpr BasicStableVector() = default;
	constexpr explicit BasicStableVector(size_type s)  {
		resize(s);
	}
	constexpr BasicStableVector(size_type s, const_reference v)  {
		insert(begin(), s, v);
	}
	template<typename InputIterator>
	constexpr BasicStableVector(InputIterator first, InputIterator last) {
		insert(begin(), first, last);
	}

	constexpr BasicStableVector(std::initializer_list<value_type> ilist) :
		BasicStableVector(ilist.begin(), ilist.end()) {}

	constexpr BasicStableVector(BasicStableVector const& s) {_append(s);}
	constexpr BasicStableVector(BasicStableVector && s) {_append(std::move(s));}

	constexpr BasicStableVector& operator=(BasicStableVector const& s) {
		clear(); _append(s);
	}
	constexpr BasicStableVector& operator=(BasicStableVector && s) {
		clear(); _append(std::move(s));
	}

private:
	template <typename... Args>
	constexpr void _initialize( _pointer_iter it, Args&&... args) {
		_pool.grab()->setParent(it, std::forward<Args>(args)...);
	}

	// Shifts all elements in [it, _pointers.end()) by n elements to the right, so
	// n empty are made available. The
	constexpr void _shift(_pointer_iter it, size_type n) {
		auto start = _pointers.end();
		_pointers.resize(_pointers.size() + n);
		while (start != it) {
			--start;
			(*start)->setParent(start+n);
		}
	}

	template <typename InputIt>
	constexpr void _insert_n(const_iterator it, size_type d, InputIt first) {
		auto pointer_iter = it._node->up;
		_shift(pointer_iter, d);
		while (d--)
			_initialize(pointer_iter++, *first++);
	}

	template <typename ForwardIt>
	constexpr void _insert(const_iterator it, ForwardIt first, ForwardIt last, std::forward_iterator_tag) {
		_insert_n(it, Constainer::distance(first, last), first);
	}

	template <typename ForwardIt>
	constexpr void _insert(const_iterator it, ForwardIt first, ForwardIt last, std::input_iterator_tag) {
		auto pointer_iter = it._node->up;
		while (first != last) {
			_shift(it, 1);
			_initialize(pointer_iter++, *first++);
		}
	}

	template <typename... Args>
	constexpr iterator _insert_repeat(const_iterator it, size_type n, Args const&... args) {
		auto pointer_iter = _piter_of(it);
		_shift(pointer_iter, n);
		while (n--)
			_initialize(pointer_iter++, args...);
		return it;
	}

public:

	template <typename... Args>
	constexpr iterator emplace(const_iterator it, Args&&... args) {
		auto pointer_iter = _piter_of(it);
		_shift(pointer_iter, 1);
		_initialize(pointer_iter, std::forward<Args>(args)...);
		return it;
	}

	constexpr iterator insert(const_iterator it, const_reference v) {return emplace(it, v);}
	constexpr iterator insert(const_iterator it, value_type   && v) {return emplace(it, std::move(v));}

	constexpr iterator insert(const_iterator it, size_type n, const_reference v) {
		return _insert_repeat(it, n, v);
	}
	template <typename InputIterator>
	constexpr require<isInputIterator<InputIterator>, iterator>
	insert(const_iterator it, InputIterator first, InputIterator last) {
		_insert(it, first, last, typename std::iterator_traits<InputIterator>::iterator_category{});
		return it;
	}
	/**< Inserts [first, first+d). Useful if InputIterator is an input iterator only. */
	template <typename InputIterator>
	constexpr require<isInputIterator<InputIterator>, iterator>
	insert(const_iterator it, size_type d, InputIterator first) {
		_insert_n(it, d, first);
		return it;
	}

	constexpr iterator insert(const_iterator it, std::initializer_list<value_type> ilist) {
		_insert(it, ilist.begin(), ilist.end(), std::random_access_iterator_tag{});
		return it;
	}

	constexpr void push_back( const_reference x ) {insert(end(), x);}
	constexpr void push_back( value_type&& x ) {insert(end(), std::move(x));}

	template <typename... Args>
	constexpr void emplace_back(Args&&... args) {
		emplace(end(), std::forward<Args>(args)...);
	}

	constexpr iterator erase(const_iterator first, const_iterator last) {
		auto pfirst=_piter_of(first), plast=_piter_of(last);
		for (auto i = pfirst; i != plast; ++i)
			_pool.free(*i++);
		_pointers.erase(pfirst, plast);
		for (; pfirst != _pointers.end(); ++pfirst)
			(*pfirst)->up = pfirst;
		return first;
	}
	constexpr iterator erase(const_iterator i) {
		auto pi = _piter_of(i);
		_pool.free(*pi);
		_pointers.erase(pi);
		while (pi != _pointers.end())
			--(*pi++)->up;
		return i;
	}

	constexpr void resize(size_type s) {
		if (s < size())
			erase(end()-(size()-s), end());
		else
			_insert_repeat(end(), s-size());
	}

};

template <typename T, std::size_t MaxN=defaultContainerSize>
using StableVector = BasicStableVector<T, ChunkPool<T, MaxN>>;

template <typename... A1, typename... A2>
constexpr bool operator==(BasicStableVector<A1...> const& lhs, BasicStableVector<A2...> const& rhs) {
	return lhs.size() == rhs.size() && Constainer::equal(lhs.begin(), lhs.end(), rhs.begin());
}
template <typename... A1, typename... A2>
constexpr bool operator <(BasicStableVector<A1...> const& lhs, BasicStableVector<A2...> const& rhs) {
	return Constainer::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename... A1, typename... A2>
constexpr bool operator!=(BasicStableVector<A1...> const& lhs, BasicStableVector<A2...> const& rhs) {
	return !(lhs == rhs);
}
template <typename... A1, typename... A2>
constexpr bool operator>=(BasicStableVector<A1...> const& lhs, BasicStableVector<A2...> const& rhs) {
	return !(lhs < rhs);
}
template <typename... A1, typename... A2>
constexpr bool operator >(BasicStableVector<A1...> const& lhs, BasicStableVector<A2...> const& rhs) {
	return rhs < lhs;
}
template <typename... A1, typename... A2>
constexpr bool operator<=(BasicStableVector<A1...> const& lhs, BasicStableVector<A2...> const& rhs) {
	return !(lhs > rhs);
}

}
