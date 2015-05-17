/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef STRING_HXX_INCLUDED
#define STRING_HXX_INCLUDED

#include "Vector.hxx"

namespace Constainer {

template <typename Char>
struct CharTraits {
	using char_type = Char;
	using int_type  = unsigned long;
	using pos_type  = std::streampos;
	using off_type  = std::streamoff;

	static constexpr void assign( char_type& a, const char_type& b ) {a=b;}

	static constexpr bool eq( char_type a, char_type b ) {return a==b;}
	static constexpr bool lt( char_type a, char_type b ) {return a <b;}

	static constexpr bool eq_int_type(int_type const& lhs, int_type const& rhs) { return lhs == rhs; }

	static constexpr int compare(char_type const* s1, char_type const* s2, std::size_t n) {
		for (std::size_t i = 0; i < n; ++i)
			     if (lt(s1[i], s2[i]))
				return -1;
			else if (lt(s2[i], s1[i]))
				return 1;

		return 0;
	}

	static constexpr std::size_t length(char_type const* p) {
		std::size_t i = 0;
		while (!eq(*p++, char_type()))
			++i;

		return i;
	}

	static constexpr char_type const* find(char_type const* s, std::size_t n, char_type const& a) {
		for (; n--; ++s)
			if (eq(*s, a))
				return s;
		return nullptr;
	}

	static constexpr char_type* copy(char_type* s1, char_type const* s2, std::size_t n) {
		return Constainer::copy_n(s2, n, s1);
	}

	static constexpr char_type* move(char_type* s1, char_type const* s2, std::size_t n) {
		return Constainer::move_n(s2, n, s1);
	}

	static constexpr char_type* assign(char_type* s, std::size_t n, char_type a) {
		return Constainer::fill_n(s, n, a);
	}

	static constexpr char_type to_char_type( int_type i ) {return std::char_traits<char_type>::to_char_type(i);}
	static constexpr char_type to_int_type( char_type c ) {return std::char_traits<char_type>::to_int_type(c);}

	static constexpr int_type eof() { return static_cast<int_type>(_GLIBCXX_STDIO_EOF); }
	static constexpr int_type not_eof(const int_type& c) { return c == eof()? 0 : c; }
};

template <typename Char, std::size_t MaxN, typename Traits=CharTraits<Char>>
class BasicString : public BasicVector<Char, MaxN+1, Traits> {

	using _base = BasicVector<Char, MaxN+1, Traits>;

public:

	using traits_type = Traits;

	using typename _base::size_type;
	using typename _base::value_type;
	using typename _base::iterator;
	using typename _base::const_iterator;
	using typename _base::reverse_iterator;
	using typename _base::const_reverse_iterator;
	using typename _base::reference;
	using typename _base::const_reference;
	using typename _base::pointer;
	using typename _base::const_pointer;

	static constexpr size_type npos = -1;

private:
	template <size_type OtherN>
	using ThisResized = BasicString<Char, OtherN>;

	constexpr bool _is_null(const_pointer str) {return traits_type::eq(*str, value_type()); }

public:

	using _base::_base;

	constexpr auto data() const {return _base::data();}
	constexpr auto c_str() const {return data();}

	template <size_type OtherN>
	constexpr BasicString(ThisResized<OtherN> const& str, size_type pos, size_type count=npos) :
		_base(str.begin()+pos, str.begin()+pos+count) {}

	constexpr BasicString(const_pointer str, size_type s) {
		this->_verifiedSinceInc(s);
		traits_type::copy(&*this->begin(), str, s);
	}

	constexpr BasicString(const_pointer str) {
		assign(str);
	}

	constexpr BasicString& assign( const_pointer str, size_type count ) {
		_base::assign(str, count);
	}
	constexpr BasicString& assign( const_pointer str ) {
		this->clear();
		insert(0, str);
		return *this;
	}

	constexpr BasicString& insert( const_iterator it, Char ch ) {
		_base::insert(it, ch);
		return *this;
	}
	constexpr BasicString& insert( size_type index, const_pointer str, size_type len ) {
		this->_verifiedSizeInc(len);
		Assert( index <= this->size() );
		// TODO: Implement move_backward using traits::assign
		move_backward( this->begin()+index, this->end(), this->end()+len );
		traits_type::copy(this->_data() + index, str, len);
		return *this;
	}
	constexpr BasicString& insert( size_type index, const_pointer str ) {
		return insert(index, str, traits_type::length(str));
	}

	constexpr BasicString& append( size_type count, value_type ch ) {
		return this->insert( this->end(), count, ch );
	}
	constexpr BasicString& append( const_pointer str, size_type len ) {
		return this->insert( this->size(), str, len );
	}

	template <std::size_t OtherN>
	constexpr BasicString& append( ThisResized<OtherN> const& str,
	                               size_type pos, size_type len=npos ) {
		AssertExcept<std::out_of_range>(pos < str.size(), "Invalid position!");
		len = std::min(len, str.size()-pos-1);
		return this->insert( this->size(), str._data()+pos, len );
	}
	template <std::size_t OtherN>
	constexpr BasicString& append( ThisResized<OtherN> const& str ) {
		return append( str, 0, npos );
	}
	constexpr BasicString& append( const_pointer str ) {
		return append( str, traits_type::length(str) );
	}
	template <typename InputIt>
	constexpr requires<isInputIterator<InputIt>, BasicString&>
	append( InputIt first, InputIt last ) {
		return this->insert(this->end(), first, last);
	}
	constexpr BasicString& append( std::initializer_list<value_type> ilist ) {
		return append(std::begin(ilist), std::end(ilist));
	}

	constexpr BasicString& operator+=( Char ch )                {return append(ch, 1);}
	template <std::size_t OtherN>
	constexpr BasicString& operator+=( ThisResized<OtherN> const& str ) {return append(str);}
	constexpr BasicString& operator+=( const_pointer str )      {return append(str);}
	constexpr BasicString& operator+=( std::initializer_list<value_type> ilist )
	{return append(ilist);}

	constexpr int compare( ThisResized<OtherN> const& other ) const {
		// TODO: Implement
		return 0;
	}

};

#include <ostream>
template <typename Char, std::size_t N, typename Traits, typename OTraits>
std::basic_ostream<Char, OTraits>& operator<<( std::basic_ostream<Char, OTraits>& os,
                                               BasicString<Char, N, Traits> const& str ) {
	return os << str.data();
}

template <typename Char, std::size_t N1, std::size_t N2, typename Traits>
constexpr auto operator+(BasicString<Char, N1, Traits> const& lhs,
                         BasicString<Char, N2, Traits> const& rhs) {
	BasicString<Char, std::max(N1, N2), Traits> result = lhs;
	return result += rhs;
}

template <typename Char, std::size_t N, typename Traits>
constexpr auto operator+(Char const* lhs, BasicString<Char, N, Traits> const& rhs) {
	BasicString<Char, N, Traits> result = rhs;
	return rhs.insert(0, lhs);
}
template <typename Char, std::size_t N, typename Traits>
constexpr auto operator+(BasicString<Char, N, Traits> const& lhs, Char const* rhs) {
	auto result = lhs;
	return result += rhs;
}
template <typename Char, std::size_t N, typename Traits>
constexpr auto operator+(BasicString<Char, N, Traits> const& lhs, Char rhs) {
	auto result = lhs;
	return result += rhs;
}
template <typename Char, std::size_t N, typename Traits>
constexpr auto operator+(Char lhs, BasicString<Char, N, Traits> const& rhs) {
	auto result = rhs;
	return result.insert(0, lhs);
}

//! Note: The move overloads are not provided as they aren't beneficial for this implementation

using String = BasicString<char, 256>;

}

#endif // STRING_HXX_INCLUDED
