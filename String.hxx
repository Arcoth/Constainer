/* Copyright (c) Columbo (columbo@gmx-topmail.de).
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) */

#ifndef STRING_HXX_INCLUDED
#define STRING_HXX_INCLUDED

#include "Vector.hxx"

namespace Constainer {

/**< This policy class template specifies how strings/characters are compared and copied.
     Its interface corresponds to the one required by BasicString. */
template <typename Char>
struct CharTraits {
private:
	using _char_traits = std::char_traits<Char>;

public:
	using char_type = Char;
	using int_type  = typename _char_traits::int_type;
	using pos_type  = typename _char_traits::pos_type;
	using off_type  = typename _char_traits::off_type;

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

	static constexpr int_type eof() { return _char_traits::eof(); }
	static constexpr int_type not_eof(int_type c) { return _char_traits::not_eof(c); }

	static constexpr void assign( char_type& a, const char_type& b ) {a=b;}

	/**< Called after erase has performed on elements in the container. */
	static constexpr void destroy(char_type* p, std::size_t n) {
		if (n != 0)
			p[0] = char_type();
	}
};

template <typename Char, std::size_t MaxN, typename Traits=CharTraits<Char>>
class BasicString : protected BasicVector<Char, MaxN, Traits> {

	using _base = BasicVector<Char, MaxN, Traits>;

protected:

	using _base::_remcv;
	using _base::_address;

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
	//using _base::insert;
	using _base::erase;
	using _base::begin;
	using _base::end;
	using _base::rbegin;
	using _base::rend;
	using _base::cbegin;
	using _base::cend;
	using _base::crbegin;
	using _base::crend;
	using _base::size;
	using _base::assign;
	using _base::back;
	using _base::front;
	using _base::clear;
	using _base::empty;
	using _base::pop_back;
	using _base::push_back;
	using _base::operator[];

	constexpr auto length() const {return size();}

	constexpr auto data() const {return _base::data();}
	constexpr auto c_str() const {return data();}

	template <size_type OtherN>
	constexpr BasicString(ThisResized<OtherN> const& str, size_type pos, size_type count=npos) :
		_base(str.begin()+pos, str.begin()+pos+count) {}

	constexpr BasicString(const_pointer str, size_type s) {
		this->_verifiedSizeInc(s);
		traits_type::copy(_base::data(), str, s);
	}
	constexpr BasicString(const_pointer str) {
		assign(str);
	}

	constexpr BasicString& assign( const_pointer str, size_type count ) {
		this->clear();
		insert(0, str, count);
		return *this;
	}
	constexpr BasicString& assign( const_pointer str ) {
		return assign(str, traits_type::length(str));
	}
	template <std::size_t N>
	constexpr BasicString& assign( ThisResized<N> const& other,
	                               size_type pos, size_type count = npos ) {
		return assign(other.data()+pos, std::min(count, size()-pos));
	}

	constexpr BasicString& insert( const_iterator it, Char ch ) {
		_base::insert(it, ch);
		return *this;
	}
	constexpr BasicString& insert( size_type index, const_pointer str, size_type len ) {
		this->_createInsertionSpace(this->begin()+index, len);
		traits_type::copy(this->_data() + index, str, len);
		return *this;
	}
	constexpr BasicString& insert( size_type index, const_pointer str ) {
		return insert(index, str, traits_type::length(str));
	}
	constexpr BasicString& insert( size_type index, size_type count, value_type ch ) {
		_base::insert(begin()+index, count, ch);
		return *this;
	}

	template <std::size_t OtherN>
	constexpr BasicString& insert(size_type index, ThisResized<OtherN> const& str,
                                    size_type str_index, size_type count = npos) {
		AssertExcept<std::out_of_range>(str_index <= str.length());
		return insert(index, str.data()+str_index, std::min(count, str.size()-str_index));
	}
	template <std::size_t OtherN>
	constexpr BasicString& insert(size_type index, ThisResized<OtherN> const& str) {
		return insert(index, str, 0);
	}

	template <typename InputIt>
	constexpr requires<isInputIterator<InputIt>, BasicString&>
	insert( const_iterator pos, InputIt first, InputIt last ) {
		_base::insert(pos, first, last);
		return *this;
	}

	constexpr BasicString& insert( const_iterator i, std::initializer_list<value_type> ilist ) {
		_base::insert(i, ilist);
		return *this;
	}

	constexpr BasicString& append( size_type count, value_type ch ) {
		insert( size(), count, ch );
		return *this;
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

	constexpr BasicString& operator+=( Char ch )                {return append(1, ch);}
	template <std::size_t OtherN>
	constexpr BasicString& operator+=( ThisResized<OtherN> const& str ) {return append(str);}
	constexpr BasicString& operator+=( const_pointer str )      {return append(str);}
	constexpr BasicString& operator+=( std::initializer_list<value_type> ilist )
	{return append(ilist);}

	/**< --------- @COMPARE ---------*/

	constexpr int compare( size_type pos, size_type count1,
	                       const_pointer s, size_type count2 ) const {
		size_type len = std::min(count1, count2);
		auto comp = traits_type::compare(data()+pos, s, len);

		if (comp != 0)
			return comp;

		if (count1 != count2)
			return count1 < count2? -1 : 1;

		return 0;
	}

	constexpr int compare( size_type pos1, size_type count1,
	                       const_pointer s ) const {
		return compare(pos1, count1, s, traits_type::length(s));
	}
	constexpr int compare( const_pointer s ) const {
		return compare(0, this->size(), s);
	}

	template <std::size_t OtherN>
	constexpr int compare( size_type pos1, size_type count1,
	                       ThisResized<OtherN> const& other,
	                       size_type pos2, size_type count2=npos ) const {
		count2 = std::min(other.size()-pos1, count2);
		return compare(pos1, count1, other.data()+pos2, count2);
	}
	template <std::size_t OtherN>
	constexpr int compare( size_type pos1, size_type count1,
	                       ThisResized<OtherN> const& str ) const {
		return compare(pos1, count1, str, 0);
	}
	template <std::size_t OtherN>
	constexpr int compare( ThisResized<OtherN> const& str ) const {
		return compare(0, this->size(), str);
	}

	constexpr BasicString& erase( size_type index = 0, size_type count = npos ) {
		_base::erase( begin()+index, begin()+index + std::min(count, size() - index) );
		return *this;
	}

	/**< --------- @REPLACE ---------*/
	constexpr BasicString& replace( size_type pos, size_type count,
                                      const_pointer str, size_type count2 ) {
		if (count < count2)
			insert(pos+count, str+count, count2-count);
		else if (count2 < count)
			erase(pos+count2, count-count2);
		traits_type::copy(_base::data()+pos, str, std::min(count, count2));
		return *this;
	}
	constexpr BasicString& replace( size_type pos, size_type count,
                                      const_pointer str ) {
		return replace(pos, count, str, traits_type::length(str));
	}
	constexpr BasicString& replace( const_iterator first, const_iterator last,
	                                const_pointer p, size_type l ) {
		return replace(first-begin(), last-first, p, l);
	}
	constexpr BasicString& replace( const_iterator first, const_iterator last,
	                                const_pointer p ) {
		return replace(first, last, p, traits_type::length(p));
	}

	template <std::size_t OtherN>
	constexpr BasicString& replace(size_type pos, size_type count,
	                               ThisResized<OtherN> const& str,
	                               size_type pos2, size_type count2 = npos) {
		AssertExcept<std::out_of_range>(pos <= length() && pos2 <= str.length());
		return replace(pos, count, str.data()+pos2, std::min(count2, str.size()-pos2));
	}

	template <std::size_t OtherN>
	constexpr BasicString& replace(size_type pos, size_type count,
                                     ThisResized<OtherN> const& str) {
		return replace(pos, count, str, 0);
	}

	template <std::size_t OtherN>
	constexpr BasicString& replace(const_iterator first, const_iterator last,
                                     ThisResized<OtherN> const& str) {
		return replace(first, last, str.data(), str.size());
	}


private:

	template <typename ForwardIt>
	constexpr void
	replace(const_iterator first, const_iterator last, ForwardIt first2, ForwardIt last2,
	        std::forward_iterator_tag) {
		auto count = last-first;
		auto count2 = Constainer::distance(first2, last2);

		if (count < count2)
			insert(last, last2-(count2-count), last2);
		else if (count2 < count)
			erase(first+count2, last);

		Constainer::copy(first2, first2+std::min(count, count2),
		                 _remcv(first));
	}

	template <typename InputIt>
	constexpr void
	replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2,
	        std::input_iterator_tag) {
		erase(first, last);
		insert(first, first2, last2);
	}

public:

	template <typename InputIt>
	constexpr std::enable_if_t<isInputIterator<InputIt>{}, BasicString&>
	replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2) {
		auto copy = *this;
		// To ensure the strong exception guarantee.
		copy.replace(copy.begin()+(first-begin()),
		             copy.begin()+(last-begin()),
		             first2, last2, typename std::iterator_traits<InputIt>::iterator_category{});
		return *this = copy;
	}

	constexpr BasicString& replace( const_iterator first, const_iterator last,
	                                std::initializer_list<value_type> ilist ) {
		return replace(first, last, std::begin(ilist), std::end(ilist));
	}

	constexpr BasicString& replace( const_iterator first, const_iterator last,
	                                size_type count2, value_type ch ) {
		auto count = last-first;
		if (count < count2)
			this->_createInsertionSpace(last, count2-count);
		else if (count2 < count)
			erase(first+count2, last);
		traits_type::assign(this->_address(first), count2, ch);
		return *this;
	}
};

#include <ostream>

/**< The following three functions have been taken from libstdc++ and adjusted syntactically. Creds to the developers of that implementation. */

namespace detail {
	template <typename CharT, typename Traits>
	void writeTo(std::basic_ostream<CharT, Traits>& os,
	             CharT const* s, std::streamsize n) {
		if (os.rdbuf()->sputn(s, n) != n)
			os.setstate(std::ios_base::badbit);
	}

	template <typename CharT, typename Traits>
	void fill(std::basic_ostream<CharT, Traits>& os, std::streamsize n) {
		auto c = os.fill();
		while (n-- > 0)
			if (Traits::eq_int_type(os.rdbuf()->sputc(c), Traits::eof())) {
				os.setstate(std::ios_base::badbit);
				break;
			}
	}
}

template <typename Char, std::size_t N, typename Traits, typename OTraits>
std::basic_ostream<Char, OTraits>& operator<<( std::basic_ostream<Char, OTraits>& os,
                                               BasicString<Char, N, Traits> const& str )
{
	if (typename std::basic_ostream<Char, OTraits>::sentry sentry{os})
		try {
			auto n = str.size();
			auto w = os.width();
			if (n < w) {
				bool left_aligned = (os.flags() & std::ios_base::adjustfield) == std::ios_base::left;
				if (!left_aligned)
					detail::fill(os, w - n);
				if (os.good()) {
					detail::writeTo(os, str.data(), n);
					if (left_aligned && os.good())
						detail::fill(os, w - n);
				}
			}
			else
				detail::writeTo(os, str.data(), n);

			os.width(0);
		}
		catch(...) {
			os.setstate(std::ios_base::badbit);
		}

	return os;
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
	return result.insert(0, lhs);
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

constexpr String operator"" _s( char const* str, std::size_t len ) {
	return {str, len};
}

/**< A load of relational operators follows. */

template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
constexpr bool operator==( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return lhs.compare(rhs) == 0;
}
template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
constexpr bool operator!=( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return !(lhs == rhs);
}
template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
constexpr bool operator <( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return lhs.compare(rhs) < 0;
}
template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
constexpr bool operator<=( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return !(lhs > rhs);
}
template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
constexpr bool operator >( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return rhs < lhs;
}
template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
constexpr bool operator>=( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return rhs <= lhs;
}

/**< char-ptr <=> BasicString relations */

template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator==( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return rhs.compare(lhs) == 0;
}
template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator==( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return rhs == lhs;
}

template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator!=( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs == rhs);
}
template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator!=( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return rhs != lhs;
}

template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator <( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return rhs.compare(lhs) > 0;
}
template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator <( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return rhs > lhs;
}

template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator<=( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs > rhs);
}
template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator<=( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return !(lhs > rhs);
}

template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator >( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs <= rhs);
}
template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator >( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return !(lhs <= rhs);
}

template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator>=( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs < rhs);
}
template <typename CharT, std::size_t N, typename Traits>
constexpr bool operator>=( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return !(lhs < rhs);
}

}

#endif // STRING_HXX_INCLUDED
