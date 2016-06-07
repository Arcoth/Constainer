/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

	Part of this work was inspired and derived from algorithms as shown in boost::container::flat_tree<>.
	Those are copyrighted by Ion Gaztanaga.  */

#pragma once

#include "Algorithms.hxx"
#include "Operators.hxx"
#include "Vector.hxx"

#include <type_traits>

namespace Constainer {

struct ordered_range_t                          {} constexpr ordered_range        {};
struct ordered_unique_range_t : ordered_range_t {} constexpr ordered_unique_range {};

namespace detail {
	template <typename Compare, typename Value, typename KeyOfValue, typename=void>
	class ValueComparator : protected Compare {
	public:
		constexpr ValueComparator() : Compare{} {}
		constexpr ValueComparator(Compare const& comp) : Compare(comp) {}

		constexpr bool operator()(Value const& lhs, Value const& rhs) const {
			return Compare::operator()(KeyOfValue()(lhs), KeyOfValue()(rhs));
		}
	};
	template <typename Compare, typename Value, typename KeyOfValue>
	class ValueComparator<Compare, Value, KeyOfValue, STD::enable_if_t<STD::is_final<Compare>{} or not STD::is_class<Compare>{}>>  {;
		Compare _comp;

	protected:
		constexpr operator Compare const&() const {return _comp;}

	public:
		constexpr ValueComparator() : _comp{} {}
		constexpr ValueComparator(Compare const& comp) : _comp(comp) {}

		constexpr bool operator()(Value const& lhs, Value const& rhs) const {
			return _comp(KeyOfValue()(lhs), KeyOfValue()(rhs));
		}
	};

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Container>
class FlatTree : private ValueComparator<Compare, Value, KeyOfValue>
{
	static constexpr bool _constIterator = STD::is_same<Key, Value>::value;

public:
	using key_type = Key;
	using key_compare = Compare;
	using value_type = Value;
	using value_compare = ValueComparator<key_compare, value_type, KeyOfValue>;

	using container_type = Container;

	static_assert( STD::is_same<typename container_type::value_type, value_type>{} );

	using pointer         = typename container_type::pointer;
	using const_pointer   = typename container_type::const_pointer;
	using reference       = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	using size_type       = typename container_type::size_type;
	using difference_type = typename container_type::difference_type;

private:
	template <bool _const>
	class _iterator_base : RandomAccessIteratable<_iterator_base<_const>,
	                                              STD::conditional_t<_const, const_pointer  , pointer  >,
	                                              STD::conditional_t<_const, const_reference, reference>, STD::ptrdiff_t>
	{
		using _iter_type = typename container_type::const_iterator;
		using _iter_traits = STD::iterator_traits<STD::conditional_t<_const or _constIterator, typename container_type::const_iterator,
		                                                                                       typename container_type::      iterator>>;
		_iter_type _iter{};

		friend FlatTree;
		constexpr _iterator_base(_iter_type it) : _iter(it) {}

		template <bool _const2, STD::enable_if_t<(_const2 >  _const), int> = 0>
		constexpr _iterator_base(_iterator_base<_const2> i) : _iter(i._iter) {}

	public:

		using value_type        = typename _iter_traits::value_type;
		using reference         = typename _iter_traits::reference;
		using pointer           = typename _iter_traits::pointer;
		using difference_type   = typename _iter_traits::difference_type;
		using iterator_category = STD::random_access_iterator_tag;

		template <bool _const2, STD::enable_if_t<(_const2 <= _const), long> = 0>
		constexpr _iterator_base(_iterator_base<_const2> i) : _iter(i._iter) {}

		constexpr _iterator_base() = default;
		constexpr _iterator_base(_iterator_base const&) = default;

		constexpr reference operator*() const {return const_cast<reference>(*_iter);}

		constexpr _iterator_base& operator+=(difference_type d) {_iter += d; return *this;}
		constexpr _iterator_base& operator-=(difference_type d) {_iter -= d; return *this;}

		constexpr _iterator_base& operator++() {++_iter; return *this;}
		constexpr _iterator_base& operator--() {--_iter; return *this;}

		constexpr difference_type operator-(_iterator_base rhs) const {return _iter - rhs._iter;}

		constexpr bool operator==(_iterator_base rhs) const {return _iter == rhs._iter;}
		constexpr bool operator <(_iterator_base rhs) const {return _iter  < rhs._iter;}
	};

public:
	using iterator               = typename container_type::iterator;
	using const_iterator         = typename container_type::const_iterator;
	using reverse_iterator       = Constainer::reverse_iterator<iterator>;
	using const_reverse_iterator = Constainer::reverse_iterator<const_iterator>;

private:
	container_type _container = {};

	template <typename T>
	constexpr iterator _inject( const_iterator it, T&& t ) {
		return _container.insert(it, STD::forward<T>(t));
	}

protected:
	constexpr value_compare const& _val_comp() const {return *this;}
	constexpr   key_compare const& _key_comp() const {return *this;}

	using _const_iter_pair = STD::pair<const_iterator, const_iterator>;
	using _iter_pair = STD::pair<iterator, iterator>;

public:

	constexpr FlatTree() : value_compare{} {}
	constexpr FlatTree(FlatTree const&) = default;
	constexpr FlatTree(FlatTree &&) = default;
	constexpr FlatTree& operator=(FlatTree const&) = default;
	constexpr FlatTree& operator=(FlatTree &&) = default;

	explicit FlatTree( value_compare const& comp ) : value_compare(comp) {}

	template <bool Unique, typename InputIt>
	constexpr FlatTree( STD::bool_constant<Unique>, InputIt first, InputIt last,
	                    value_compare const& comp)
		: value_compare(comp)
	{
		// More efficient assuming [first, last) is sorted to some extent
		if(Unique)
			while (first != last)
				insert_unique(this->cend(), *first++);
		else
			while (first != last)
				insert_equal (this->cend(), *first++);
	}

	template <typename InputIt>
	constexpr FlatTree( ordered_range_t, InputIt first, InputIt last,
	                    value_compare const& comp)
		: value_compare(comp), _container(first, last) {}

	constexpr   key_compare   key_comp() const {return *this;}
	constexpr value_compare value_comp() const {return *this;}

	constexpr         bool empty() const {return _container.empty();}
	constexpr size_type     size() const {return _container.size();}
	static constexpr size_type max_size() {return container_type::max_size();}

	constexpr       iterator  begin()       {return _container.begin();}
	constexpr       iterator    end()       {return _container.end();}
	constexpr const_iterator  begin() const {return _container.begin();}
	constexpr const_iterator    end() const {return _container.end();}
	constexpr const_iterator cbegin() const {return begin();}
	constexpr const_iterator   cend() const {return end();}

	constexpr       reverse_iterator  rbegin()       {return reverse_iterator(end());}
	constexpr       reverse_iterator    rend()       {return reverse_iterator(begin());}
	constexpr const_reverse_iterator  rbegin() const {return rbegin();}
	constexpr const_reverse_iterator    rend() const {return rend();}
	constexpr const_reverse_iterator crbegin() const {return rbegin();}
	constexpr const_reverse_iterator   crend() const {return rend();}

	constexpr       iterator lower_bound(key_type const& key) {
		return _lower_bound(begin(), end(), key);
	}
	constexpr const_iterator lower_bound(key_type const& key) const {
		return _lower_bound(begin(), end(), key);
	}
	template <typename T>
	constexpr has_is_transparent<key_compare,       _iter_pair> lower_bound(T const& t) {
		return _lower_bound(begin(), end(), t);
	}
	template <typename T>
	constexpr has_is_transparent<key_compare, _const_iter_pair> lower_bound(T const& t) const {
		return _lower_bound(begin(), end(), t);
	}

	constexpr       iterator upper_bound(key_type const& key) {
		return _upper_bound(begin(), end(), key);
	}
	constexpr const_iterator upper_bound(key_type const& key) const {
		return _upper_bound(begin(), end(), key);
	}
	template <typename T>
	constexpr has_is_transparent<key_compare,       _iter_pair> upper_bound(T const& t) {
		return _upper_bound(begin(), end(), t);
	}
	template <typename T>
	constexpr has_is_transparent<key_compare, _const_iter_pair> upper_bound(T const& t) const {
		return _upper_bound(begin(), end(), t);
	}

	constexpr       iterator find(key_type const& key) {
		auto i = lower_bound(key), last = end();
		return i == last || _key_comp()(key, *i)? last : i;
	}
	constexpr const_iterator find(key_type const& key) const {
		return const_cast<FlatTree*>(this)->find(key);
	}
	template <typename T>
	constexpr has_is_transparent<key_compare,       iterator> find(T const& key) {
		auto i = lower_bound(key), last = end();
		return i == last || _key_comp()(key, *i)? last : i;
	}
	template <typename T>
	constexpr has_is_transparent<key_compare, const_iterator> find(T const& key) const {
		return const_cast<FlatTree*>(this)->find<>(key);
	}

	template <typename T>
	constexpr       _iter_pair lower_bound_range(T const& t) {
		return lower_bound_range(begin(), end(), t);
	}
	template <typename T>
	constexpr _const_iter_pair lower_bound_range(T const& t) const {
		return _lower_bound_range(begin(), end(), t);
	}

	constexpr       _iter_pair equal_range(key_type const& key) {
		return _equal_range(begin(), end(), key);
	}
	constexpr _const_iter_pair equal_range(key_type const& key) const {
		return _equal_range(begin(), end(), key);
	}
	template <typename T>
	constexpr has_is_transparent<key_compare,       _iter_pair> equal_range(T const& t) {
		return _equal_range(begin(), end(), t);
	}
	template <typename T>
	constexpr has_is_transparent<key_compare, _const_iter_pair> equal_range(T const& t) const {
		return _equal_range(begin(), end(), t);
	}

	constexpr size_type count(key_type const& key) const {
		auto range = equal_range(key);
		return range.second - range.first;
	}
	template <typename T>
	constexpr has_is_transparent<key_compare, size_type> count(T const& t) const {
		auto range = equal_range(t);
		return range.second - range.first;
	}

	constexpr iterator erase(const_iterator position) {
		return _container.erase(position);
	}
	constexpr iterator erase(const_iterator first, const_iterator last) {
		return _container.erase(first, last);
	}
	constexpr size_type erase(key_type const& key) {
		auto range = equal_range(key);
		size_type ret = range.second - range.first;
		if (ret != 0)
			erase(range.first, range.second);

		return ret;
	}

	constexpr void clear() {_container.clear();}

	constexpr iterator insert_equal(const_reference val) {
		auto i = upper_bound(KeyOfValue()(val));
		return _container.insert(i, val);
	}
	constexpr iterator insert_equal(value_type&& val) {
		auto i = upper_bound(KeyOfValue()(val));
		return _container.insert(i, STD::move(val));
	}

	constexpr iterator insert_equal(const_iterator hint, const_reference val) {
		hint = _equal_find_insertion_spot(hint, val);
		return _inject(hint, val);
	}
	constexpr iterator insert_equal(const_iterator hint, value_type&& mval) {
		hint = _equal_find_insertion_spot(hint, mval);
		return _inject(hint, STD::move(mval));
	}

	constexpr STD::pair<iterator, bool> insert_unique(const_reference val) {
		auto ret = _unique_find_insertion_spot(val);
		if (ret.second)
			return {_inject(ret.first, val), true};
		return {unconstifyIterator(_container, ret.first), false};
	}
	constexpr STD::pair<iterator, bool> insert_unique(value_type&& val) {
		auto ret = _unique_find_insertion_spot(val);
		if (ret.second)
			return {_inject(ret.first, STD::move(val)), true};
		return {unconstifyIterator(_container, ret.first), false};
	}

	constexpr iterator insert_unique(const_iterator hint, const_reference val) {
		auto ret = _unique_find_insertion_spot(hint, val);
		if (ret.second)
			return _inject(ret.first, val);
		return unconstifyIterator(_container, ret.first);
	}
	constexpr iterator insert_unique(const_iterator hint, value_type&& val) {
		auto ret = _unique_find_insertion_spot(hint, val);
		if (ret.second)
			return _inject(ret.first, STD::move(val));
		return unconstifyIterator(_container, ret.first);
	}
private:
	template <typename InputIt>
	constexpr void _insert_unique(ordered_unique_range_t, InputIt first, InputIt last, STD::input_iterator_tag) {
		auto pos = end();
		for (; first != last; ++first) {
			pos = insert_unique(pos, *first).first;
			++pos;
		}
	}

	template <typename BiDirIt>
	constexpr void _insert_unique(ordered_unique_range_t, BiDirIt first, BiDirIt last, STD::bidirectional_iterator_tag) {
		_insert_ordered_range(STD::true_type{}, first, last);
	}

public:

	template <typename InputIt>
	constexpr void insert_unique(InputIt first, InputIt last) {
		while (first != last)
			insert_unique(*first++);
	}

	template <typename InputIt>
	constexpr void insert_unique(ordered_unique_range_t, InputIt first, InputIt last) {
		_insert_unique(ordered_unique_range, first, last, iteratorCategory<InputIt>{});
	}

	template <typename InputIt>
	constexpr void insert_equal(InputIt first, InputIt last) {
		while (first != last)
			insert_equal(*first++);
	}

private:
	template <typename InputIt>
	constexpr void _insert_equal(ordered_range_t, InputIt first, InputIt last, STD::input_iterator_tag) {
		auto pos = end();
		while (first != last) {
			// Potential performance boost if several elements can be inserted adjacently
			pos = insert_equal(pos, *first++);
			++pos;
		}
	}

	template <typename BiDirIt>
	constexpr void _insert_equal(ordered_range_t, BiDirIt first, BiDirIt last, STD::bidirectional_iterator_tag) {
		insert_ordered_range(false, first, last);
	}

public:

	template <typename InputIt>
	constexpr void insert_equal(ordered_range_t, InputIt first, InputIt last) {
		_insert_equal(ordered_range, first, last, iteratorCategory<InputIt>{});
	}

	template <typename... Args>
	constexpr STD::pair<iterator, bool> emplace_unique(Args&&... args) {
		value_type val(STD::forward<Args>(args)...);
		return insert_unique(STD::move(val));
	}
	template <typename... Args>
	constexpr iterator emplace_hint_unique(const_iterator hint, Args&&... args) {
		value_type val(STD::forward<Args>(args)...);
		return insert_unique(hint, STD::move(val));
	}
	template <typename... Args>
	constexpr iterator emplace_equal(Args&&... args) {
		value_type val(STD::forward<Args>(args)...);
		return insert_equal(STD::move(val));
	}
	template <typename... Args>
	constexpr iterator emplace_hint_equal(const_iterator hint, Args&&... args) {
		value_type val(STD::forward<Args>(args)...);
		return insert_equal(hint, STD::move(val));
	}

	constexpr       iterator nth(size_type i)       {return begin() + i;}
	constexpr const_iterator nth(size_type i) const {return cbegin() + i;}
	constexpr size_type index_of(iterator i)             {return i-begin();}
	constexpr size_type index_of(const_iterator i) const {return i-cbegin();}

protected:

	template <typename T>
	constexpr const_iterator _lower_bound(const_iterator first, const_iterator last, T const& key) const {
		auto& cmp = _key_comp();
		size_type count = last-first;
		while (count > 0) {
			auto intervall_size = count / 2;
			auto it = first + intervall_size;
			if (cmp(KeyOfValue()(*it), key)) {
				first = ++it;
				count -= intervall_size + 1;
			}
			else
				count = intervall_size;
		}
		return first;
	}
	template <typename T>
	constexpr const_iterator _upper_bound(const_iterator first, const_iterator last, T const& key) const {
		auto& cmp = _key_comp();
		size_type count = last-first;
		while (count > 0) {
			auto intervall_size = count / 2;
			auto it = first + intervall_size;
			if (not cmp(key, KeyOfValue()(*it))) {
				first = ++it;
				count -= intervall_size + 1;
			}
			else
				count = intervall_size;
		}
		return first;
	}
	template <typename T>
	constexpr iterator _lower_bound(const_iterator first, const_iterator last, T const& key) {
		return unconstifyIterator(_container, as_const(*this)._lower_bound(first, last, key));
	}
	template <typename T>
	constexpr iterator _upper_bound(const_iterator first, const_iterator last, T const& key) {
		return unconstifyIterator(_container, as_const(*this)._upper_bound(first, last, key));
	}

	constexpr STD::pair<const_iterator, bool> _unique_find_insertion_spot(const_iterator first, const_iterator last, const_reference val) const {
		auto const& cmp = _val_comp();
		auto it = _lower_bound(first, last, KeyOfValue()(val));
		return {it, it == last || cmp(val, *it)};
	}

	constexpr STD::pair<const_iterator, bool> _unique_find_insertion_spot(const_reference val) const {
		return _unique_find_insertion_spot(begin(), end(), val);
	}

	constexpr STD::pair<const_iterator, bool> _unique_find_insertion_spot(const_iterator hint, const_reference val) const {
		// N1780:
		// To insert val at hint:
		// if hint == end || val <= *hint
		//    if hint == begin || val >= *(hint-1)
		//       insert val before hint
		//    else
		//       insert val before upper_bound(val)
		// else if hint+1 == end || val <= *(hint+1)
		//    insert val after hint
		// else
		//    insert val before lower_bound(val)
		auto const& cmp = _val_comp();
		auto last = end();
		if (hint == last || cmp(val, *hint)) {
			STD::pair<const_iterator, bool> rval = {}; rval.second = true;
			auto first = begin();
			rval.first = hint;
			if (hint == first)
				return rval;

			auto prev = Constainer::prev(hint);
			if (cmp(*prev, val))
				return rval;

			else if(!cmp(val, *prev))
				return {prev, false};

			else
				return _unique_find_insertion_spot(first, prev, val);

		}
		else
			return _unique_find_insertion_spot(hint, last, val);
	}

	constexpr const_iterator _equal_find_insertion_spot(const_iterator pos, const_reference val) const {
		// N1780
		//   To insert val at pos:
		//   if pos == end || val <= *pos
		//      if pos == begin || val >= *(pos-1)
		//         insert val before pos
		//      else
		//         insert val before upper_bound(val)
		//   else
		//      insert val before lower_bound(val)
		value_compare const& val_cmp = *this;

		if (pos == cend() || !val_cmp(*pos, val)) {
			if (pos == cbegin() || !val_cmp(val, pos[-1]))
				return pos;
			else return _upper_bound(cbegin(), pos, KeyOfValue()(val));
		}
		else return _lower_bound(pos, cend(), KeyOfValue()(val));
	}

	/**< Derived from boost::container::flat_tree<>::priv_equal_range. */
	template <typename T>
	constexpr _const_iter_pair _equal_range(const_iterator first, const_iterator last, T const& key) const {
		auto const& key_comp = _key_comp();
		KeyOfValue key_extract;
		size_type len = last - first;

		while (len != 0) {
			auto step = len / 2;
			auto middle = first + step;

			if (key_comp(key_extract(*middle), key)){
				first = ++middle;
				len -= step + 1;
			}
			else if (key_comp(key, key_extract(*middle)))
				len = step;
			else
				// Here, key == *middle. Perform bound searches in both directions.
				return {_lower_bound(first   , middle   , key),
				        _upper_bound(middle+1, first+len, key)};
		}
		return {first, first};
	}
	template <typename T>
	constexpr _iter_pair _equal_range(const_iterator first, const_iterator last, T const& key) {
		auto r = as_const(*this)._equal_range(first, last, key);
		return {unconstifyIterator(_container, r.first),
		        unconstifyIterator(_container, r.second)};
	}

	template <typename T>
	constexpr _const_iter_pair _lower_bound_range(const_iterator first, const_iterator last, T const& key) const {
		// Since there can be at most one occurence of the key, we solely need to check whether the lower bound is not at the end
		// and actually is the key.
		auto pos = _lower_bound(first, last, key);
		if(pos != last && not _key_comp()(key, KeyOfValue()(*pos)))
			return {pos, ++pos};
		return {pos, pos};
	}

	/**< The following algorithm was taken from boost::container::flat_tree<>::priv_insert_ordered_range. */
	template <bool Unique, typename BiDirIt>
	constexpr void _insert_ordered_range(STD::bool_constant<Unique>, BiDirIt first, BiDirIt last)
	{
		size_type len = Constainer::distance(first, last), BurstSize=len;
		//Auxiliary data for insertion positions.
		Constainer::Vector<size_type, max_size()> positions(len);
		auto b = begin(), pos = b, ce = end();
		auto const& val_cmp = _val_comp();
		//Loop in burst sizes
		bool back_insert = false;
		while (len != 0 && not back_insert) {
			auto burst = STD::min(len, BurstSize);
			size_type unique_burst = 0, checked = 0;
			for(; checked != burst; ++checked) {
				//Get the insertion position for each key
				auto const& val = *first;
				pos = _lower_bound(pos, ce, KeyOfValue()(val));
				//Check if already present
				if (pos != ce) {
					++first;
					--len;
					if (Unique && !val_cmp(val, *pos))
						positions[checked] = size_type(-1);
					else {
						++unique_burst;
						positions[checked] = pos - b;
					}
				}
				else { //this element and the remaining should be back inserted
					back_insert = true;
					break;
				}
			}
			if (unique_burst != 0) {
				AssertExcept<STD::bad_alloc>(max_size() - size() >= unique_burst);
				insert_ordered_at(_container, unique_burst, first, positions.begin() + checked);
				pos += unique_burst;
			}
		}
		//The remaining range should be back inserted
		if (len != 0) {
			if(Unique)
				do {
					auto next = Constainer::next(first);
					auto const& val = *first;
					if (next == last || val_cmp(val, *next))
						_container.push_back(val);

					first = next;
				} while (--len != 0);
			else
				_container.insert(end(), len, first);
		}
	}
};


template <typename... Args1, typename... Args2>
constexpr bool operator==(FlatTree<Args1...> const& lhs, FlatTree<Args2...> const& rhs) {
	return lhs.size() == rhs.size() && Constainer::equal(lhs.begin(), lhs.end(), rhs.begin());
}
template <typename... Args1, typename... Args2>
constexpr bool operator <(FlatTree<Args1...> const& lhs, FlatTree<Args2...> const& rhs) {
	return Constainer::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename... Args1, typename... Args2>
constexpr bool operator!=(FlatTree<Args1...> const& lhs, FlatTree<Args2...> const& rhs) {
	return !(lhs == rhs);
}
template <typename... Args1, typename... Args2>
constexpr bool operator>=(FlatTree<Args1...> const& lhs, FlatTree<Args2...> const& rhs) {
	return !(lhs < rhs);
}
template <typename... Args1, typename... Args2>
constexpr bool operator >(FlatTree<Args1...> const& lhs, FlatTree<Args2...> const& rhs) {
	return rhs < lhs;
}
template <typename... Args1, typename... Args2>
constexpr bool operator<=(FlatTree<Args1...> const& lhs, FlatTree<Args2...> const& rhs) {
	return !(lhs > rhs);
}

} // end namespace Constainer::detail

} // end namespace Constainer
