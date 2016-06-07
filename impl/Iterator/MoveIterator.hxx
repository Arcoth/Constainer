#pragma once

namespace Constainer {

template <typename Iterator>
class move_iterator
{
private:
	Iterator _current;

	using _traits_type    = STD::iterator_traits<Iterator>;
	using _underlying_ref = typename _traits_type::reference;

public:
	using iterator_type = Iterator;
	using iterator_category = typename _traits_type::iterator_category;
	using value_type        = typename _traits_type::value_type;
	using difference_type   = typename _traits_type::difference_type;
	using pointer           = iterator_type; //! LWG #680

	//! LWG #2106
	using reference = STD::conditional_t<STD::is_reference<_underlying_ref>{},
	                                     STD::remove_reference_t<_underlying_ref>&&,
	                                     _underlying_ref>;

	constexpr iterator_type base() const { return _current; }

	constexpr move_iterator() : _current() {}

	constexpr explicit move_iterator(iterator_type i) : _current(i) {}

	template<typename Iter>
	constexpr move_iterator(move_iterator<Iter> const& i) : _current(i.base()) {}

	constexpr reference operator*() const {
		//! LWG #2106
		return static_cast<reference>(*_current);
	}

	constexpr pointer operator->() const { return _current; }

	constexpr move_iterator  operator+ (difference_type n) const {return move_iterator(_current + n);}
	constexpr move_iterator  operator- (difference_type n) const {return move_iterator(_current - n);}

	constexpr move_iterator& operator++() {++_current; return *this;}
	constexpr move_iterator& operator--() {--_current; return *this;}

	constexpr move_iterator operator++(int) {move_iterator __tmp = *this; ++*this; return __tmp;}
	constexpr move_iterator operator--(int) {move_iterator __tmp = *this; --*this; return __tmp;}

	constexpr move_iterator& operator+=(difference_type n) {_current += n; return *this;}
	constexpr move_iterator& operator-=(difference_type n) {_current -= n; return *this;}

	constexpr reference operator[](difference_type n) const { return STD::move(_current[n]); }
};

template <typename I1, typename I2>
constexpr bool operator==( move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return lhs.base() == rhs.base(); }

template <typename I1, typename I2>
constexpr bool operator!=( move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return !(lhs == rhs); }

template <typename I1, typename I2>
constexpr bool operator<(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return lhs.base() < rhs.base(); }

template <typename I1, typename I2>
constexpr bool operator<=(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return !(rhs < lhs); }

template <typename I1, typename I2>
constexpr bool operator>(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return rhs < lhs; }

template <typename I1, typename I2>
constexpr bool operator>=(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return !(lhs < rhs); }

//! Related: LWG #685
template <typename I1, typename I2>
constexpr decltype(auto) operator-(move_iterator<I1> const& lhs, move_iterator<I2> const& rhs)
{ return lhs.base() - rhs.base(); }

template <typename I>
constexpr move_iterator<I>
operator+(typename move_iterator<I>::difference_type n, move_iterator<I> const& lhs)
{ return lhs + n; }

template <typename I>
constexpr auto make_move_iterator(I i) {return move_iterator<I>(i);}

}
