#pragma once

namespace Constainer {

template <typename Extractor, typename Iterator>
struct transform_iterator
{
	using iterator_type = Iterator;
	using extractor_type = Extractor;

	using reference = STD::result_of_t<extractor_type const&(typename STD::iterator_traits<iterator_type>::reference)>;
	using value_type = STD::decay_t<reference>;
	using pointer = value_type*;
	using difference_type = typename STD::iterator_traits<Iterator>::difference_type;
	using iterator_category = typename STD::iterator_traits<Iterator>::iterator_category;

private:

	STD::tuple<Extractor, iterator_type> _data;
	constexpr decltype(auto) _it()       {return STD::get<iterator_type>(_data);}
	constexpr decltype(auto) _ex() const {return STD::get<extractor_type>(_data);}

public:

	constexpr auto extractor() const {return _ex();}
	constexpr auto base() const {return STD::get<iterator_type>(_data);}

	constexpr explicit transform_iterator(iterator_type it, extractor_type const& ex = {}) : _data(ex, it) {}

	template <typename Q, typename Iter2>
	constexpr transform_iterator(transform_iterator<Q, Iter2> const& other)
		: _data(other._data) {}

	constexpr reference operator*() const {return _ex()(*base());}

	constexpr pointer operator->() const {return &operator*();}

	constexpr transform_iterator& operator++() {
		++_it();
		return *this;
	}

	constexpr transform_iterator operator++(int) {
		auto t = *this;
		++_it();
		return t;
	}

	constexpr transform_iterator& operator--() {
		--_it();
		return *this;
	}

	constexpr transform_iterator operator--(int) {
		transform_iterator t = *this;
		--_it();
		return t;
	}

	constexpr transform_iterator operator+(difference_type n) const
	{ return transform_iterator(base() + n); }

	constexpr transform_iterator& operator+=(difference_type n) {
		_it() += n;
		return *this;
	}

	constexpr transform_iterator operator-(difference_type n) const
	{ return transform_iterator(base() - n); }

	constexpr transform_iterator& operator-=(difference_type n) {
		_it() -= n;
		return *this;
	}

	constexpr reference operator[](difference_type n) const
	{ return *(*this + n); }
};

template <typename P, typename I>
constexpr bool operator==(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return lhs.base() == rhs.base(); }

template <typename P, typename I>
constexpr bool operator <(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return rhs.base() < lhs.base(); }

template<typename P, typename I>
constexpr bool operator!=(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return !(lhs == rhs); }

template <typename P, typename I>
constexpr bool operator >(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return rhs < lhs; }

template <typename P, typename I>
constexpr bool operator<=(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return !(rhs < lhs); }

template <typename P, typename I>
constexpr bool operator>=(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return !(lhs < rhs); }

template <typename P, typename I>
constexpr auto operator-(transform_iterator<P,I> const& lhs, transform_iterator<P,I> const& rhs)
{ return rhs.base() - lhs.base(); }

template <typename P, typename I>
constexpr auto operator+(typename transform_iterator<P,I>::difference_type n, transform_iterator<P,I> const& lhs)
{ return transform_iterator<P,I>(lhs.base() - n); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator==(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return lhs.base() == rhs.base(); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator <(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return rhs.base() < lhs.base(); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator!=(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return !(lhs == rhs); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator >(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return rhs < lhs; }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator<=(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return !(rhs < lhs); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr bool operator>=(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return !(lhs < rhs); }

template <typename P1, typename I1, typename P2, typename I2>
constexpr decltype(auto) operator-(transform_iterator<P1,I1> const& lhs, transform_iterator<P2,I2> const& rhs)
{ return rhs.base() - lhs.base(); }

template <typename E, typename I>
constexpr auto make_transform_iterator(I i, E const& e) {
	return transform_iterator<E,I>(i,e);
}

}
