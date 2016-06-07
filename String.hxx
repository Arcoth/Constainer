/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */


#pragma once

#include "impl/BasicVector.hxx"

#include <string> // char_traits

namespace Constainer {

/**< This policy class template specifies how strings/characters are compared and copied.
     Its interface corresponds to the one required by BasicString. */
template <typename Char>
struct CharTraits {
private:
	using _char_traits = STD::char_traits<Char>;

public:
	using char_type = Char;
	using int_type  = typename _char_traits::int_type;
	using pos_type  = typename _char_traits::pos_type;
	using off_type  = typename _char_traits::off_type;

	static constexpr bool eq( char_type a, char_type b ) {return a==b;}
	static constexpr bool lt( char_type a, char_type b ) {return a <b;}

	static constexpr bool eq_int_type(int_type const& lhs, int_type const& rhs) { return lhs == rhs; }

	static constexpr int compare(char_type const* s1, char_type const* s2, STD::size_t n) {
		for (STD::size_t i = 0; i < n; ++i)
			     if (lt(s1[i], s2[i]))
				return -1;
			else if (lt(s2[i], s1[i]))
				return 1;

		return 0;
	}

	static constexpr STD::size_t length(char_type const* p) {
		STD::size_t i = 0;
		while (!eq(*p++, char_type()))
			++i;

		return i;
	}

	static constexpr char_type const* find(char_type const* s, STD::size_t n, char_type const& a) {
		for (; n--; ++s)
			if (eq(*s, a))
				return s;
		return nullptr;
	}

	static constexpr char_type* copy(char_type* s1, char_type const* s2, STD::size_t n) {
		return Constainer::copy_n(s2, n, s1);
	}

	static constexpr char_type* move(char_type* s1, char_type const* s2, STD::size_t n) {
		return Constainer::move_n(s2, n, s1);
	}

	static constexpr char_type* assign(char_type* s, STD::size_t n, char_type a) {
		return Constainer::fill_n(s, n, a);
	}

	static constexpr char_type to_char_type( int_type i ) {return STD::char_traits<char_type>::to_char_type(i);}
	static constexpr char_type to_int_type( char_type c ) {return STD::char_traits<char_type>::to_int_type(c);}

	static constexpr int_type eof() { return _char_traits::eof(); }
	static constexpr int_type not_eof(int_type c) { return _char_traits::not_eof(c); }

	static constexpr void assign( char_type& a, const char_type& b ) {a=b;}

	/**< Called after erase has performed on elements in the container. */
	static constexpr void destroy(char_type* p, STD::size_t n) {
		if (n != 0)
			p[0] = char_type();
	}
};

CONSTAINER_PURE_CONST constexpr bool isspace(int i) {
	using _traits = CharTraits<char>;
	if (_traits::eq(i, _traits::eof()))
		return false;
	auto ch = _traits::to_char_type(i);
	return ch == ' '  || ch == '\f' || ch == '\n'
	    || ch == '\t' || ch == '\r' || ch == '\v';
}

CONSTAINER_PURE_CONST constexpr int toupper(int i) {
	using _traits = CharTraits<char>;
	if (_traits::eq(i, _traits::eof()))
		return i;
	auto ch = _traits::to_char_type(i);
	char const lower[] = "abcdefghijklmnopqrstuvwxyz";
	char const upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (auto p = _traits::find(lower, 26, ch))
		return upper[p-lower];
	return i;
}


template <typename Char, STD::size_t MaxN, typename Traits=CharTraits<Char>>
class BasicString : protected detail::BasicVector<Char, MaxN, Traits, 1> {

	using _base = detail::BasicVector<Char, MaxN, Traits, 1>;

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

public:

	using _base::erase;

	using _base::begin;
	using _base::end;
	using _base::rbegin;
	using _base::rend;
	using _base::cbegin;
	using _base::cend;
	using _base::crbegin;
	using _base::crend;

	using _base::assign;
	using _base::back;
	using _base::clear;
	using _base::empty;
	using _base::front;
	using _base::operator[];
	using _base::pop_back;
	using _base::size;

	constexpr size_type max_size() const {return npos-1;}

	constexpr auto length() const {return size();}

	constexpr auto data() const {return _base::data();}
	constexpr auto c_str() const {return data();}

	template <size_type OtherN>
	constexpr BasicString(ThisResized<OtherN> const& str, size_type pos=0, size_type count=npos) :
		BasicString(str.data()+pos, STD::min(str.size()-pos, count)) {}

	constexpr BasicString(const_pointer str, size_type s) {
		append(str, s);
	}
	constexpr BasicString(const_pointer str) {
		append(str);
	}

	constexpr BasicString(BasicString&&) = default;
	constexpr BasicString(BasicString const&) = default;

	constexpr BasicString() = default;
	constexpr BasicString(STD::initializer_list<value_type> ilist) :
		_base(ilist) {}
	template <typename InputIt>
	constexpr BasicString(InputIt first, InputIt last) : _base(first, last) {}

	constexpr BasicString& operator=(BasicString const&) = default;
	constexpr BasicString& operator=(BasicString&&) = default;
	constexpr BasicString& operator=(const_pointer p) {return assign(p);}
	constexpr BasicString& operator=(value_type c) {_base::assign(1, c); return *this;} // Prevent postponed lookup into base class
	constexpr BasicString& operator=(STD::initializer_list<value_type> ilist) {
		return assign(ilist);
	}

	/**< --------- @ASSIGN ---------*/

	constexpr BasicString& assign( const_pointer str, size_type count ) {
		this->clear();
		insert(0, str, count);
		return *this;
	}
	constexpr BasicString& assign( const_pointer str ) {
		return assign(str, traits_type::length(str));
	}
	constexpr BasicString& assign( STD::initializer_list<value_type> ilist ) {
		_base::assign(ilist);
		return *this;
	}
	constexpr BasicString& assign( size_type count, value_type value ) {
		_base::assign(count, value);
		return *this;
	}
	template <class InputIt>
	constexpr auto assign(InputIt first, InputIt last)
		-> require<isInputIterator<InputIt>, BasicString&> {
		_base::assign(first, last);
		return *this;
	}
	template <STD::size_t N>
	constexpr BasicString& assign( ThisResized<N> const& other,
	                               size_type pos, size_type count = npos ) {
		return assign(other.data()+pos, STD::min(count, size()-pos));
	}
	/**< --------- @INSERT ---------*/

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

	template <STD::size_t OtherN>
	constexpr BasicString& insert(size_type index, ThisResized<OtherN> const& str,
                                    size_type str_index, size_type count = npos) {
		AssertExcept<STD::out_of_range>(str_index <= str.length());
		return insert(index, str.data()+str_index, STD::min(count, str.size()-str_index));
	}
	template <STD::size_t OtherN>
	constexpr BasicString& insert(size_type index, ThisResized<OtherN> const& str) {
		return insert(index, str, 0);
	}

	template <typename InputIt>
	constexpr require<isInputIterator<InputIt>, BasicString&>
	insert( const_iterator pos, InputIt first, InputIt last ) {
		_base::insert(pos, first, last);
		return *this;
	}

	constexpr BasicString& insert( const_iterator i, STD::initializer_list<value_type> ilist ) {
		_base::insert(i, ilist);
		return *this;
	}

	/**< --------- @APPEND ---------*/

	constexpr BasicString& append( size_type count, value_type ch ) {
		insert( size(), count, ch );
		return *this;
	}
	constexpr BasicString& append( const_pointer str, size_type len ) {
		return this->insert( this->size(), str, len );
	}

	template <STD::size_t OtherN>
	constexpr BasicString& append( ThisResized<OtherN> const& str,
	                               size_type pos, size_type len=npos ) {
		AssertExcept<STD::out_of_range>(pos <= str.size(), "Invalid position!");
		len = STD::min(len, str.size()-pos);
		return this->insert( this->size(), str.data()+pos, len );
	}
	template <STD::size_t OtherN>
	constexpr BasicString& append( ThisResized<OtherN> const& str ) {
		return append( str, 0 );
	}
	constexpr BasicString& append( const_pointer str ) {
		return append( str, traits_type::length(str) );
	}
	template <typename InputIt>
	constexpr require<isInputIterator<InputIt>, BasicString&>
	append( InputIt first, InputIt last ) {
		return this->insert(this->end(), first, last);
	}
	constexpr BasicString& append( STD::initializer_list<value_type> ilist ) {
		return append(STD::begin(ilist), STD::end(ilist));
	}

	/**< --------- @OPERATOR+= ---------*/

	constexpr BasicString& operator+=( Char ch )                {return append(1, ch);}
	template <STD::size_t OtherN>
	constexpr BasicString& operator+=( ThisResized<OtherN> const& str ) {return append(str);}
	constexpr BasicString& operator+=( const_pointer str )      {return append(str);}
	constexpr BasicString& operator+=( STD::initializer_list<value_type> ilist )
	{return append(ilist);}

	/**< --------- @COMPARE ---------*/

	constexpr int compare( size_type pos, size_type count1,
	                       const_pointer s, size_type count2 ) const {
		size_type len = STD::min(count1, count2);
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

	template <STD::size_t OtherN>
	constexpr int compare( size_type pos1, size_type count1,
	                       ThisResized<OtherN> const& other,
	                       size_type pos2, size_type count2=npos ) const {
		count2 = STD::min(other.size()-pos1, count2);
		return compare(pos1, count1, other.data()+pos2, count2);
	}
	template <STD::size_t OtherN>
	constexpr int compare( size_type pos1, size_type count1,
	                       ThisResized<OtherN> const& str ) const {
		return compare(pos1, count1, str, 0);
	}
	template <STD::size_t OtherN>
	constexpr int compare( ThisResized<OtherN> const& str ) const {
		return compare(0, this->size(), str);
	}

	constexpr BasicString& erase( size_type index = 0, size_type count = npos ) {
		_base::erase( begin()+index, begin()+index + STD::min(count, size() - index) );
		return *this;
	}

	/**< --------- @REPLACE ---------*/

	constexpr BasicString& replace( size_type pos, size_type count,
                                      const_pointer str, size_type count2 ) {
		if (count < count2)
			insert(pos+count, str+count, count2-count);
		else if (count2 < count)
			erase(pos+count2, count-count2);
		traits_type::copy(_base::data()+pos, str, STD::min(count, count2));
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

	template <STD::size_t OtherN>
	constexpr BasicString& replace(size_type pos, size_type count,
	                               ThisResized<OtherN> const& str,
	                               size_type pos2, size_type count2 = npos) {
		AssertExcept<STD::out_of_range>(pos <= length() && pos2 <= str.length());
		return replace(pos, count, str.data()+pos2, STD::min(count2, str.size()-pos2));
	}

	template <STD::size_t OtherN>
	constexpr BasicString& replace(size_type pos, size_type count,
                                     ThisResized<OtherN> const& str) {
		return replace(pos, count, str, 0);
	}

	template <STD::size_t OtherN>
	constexpr BasicString& replace(const_iterator first, const_iterator last,
                                     ThisResized<OtherN> const& str) {
		return replace(first, last, str.data(), str.size());
	}


private:

	template <typename ForwardIt>
	constexpr void
	replace(const_iterator first, const_iterator last, ForwardIt first2, ForwardIt last2,
	        STD::forward_iterator_tag) {
		auto count = last-first;
		auto count2 = Constainer::distance(first2, last2);

		if (count < count2)
			insert(last, last2-(count2-count), last2);
		else if (count2 < count)
			erase(first+count2, last);

		Constainer::copy(first2, first2+STD::min(count, count2),
		                 _remcv(first));
	}

	template <typename InputIt>
	constexpr void
	replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2,
	        STD::input_iterator_tag) {
		erase(first, last);
		insert(first, first2, last2);
	}

public:

	template <typename InputIt>
	constexpr require<isInputIterator<InputIt>, BasicString&>
	replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2) {
		auto copy = *this;
		// To ensure the strong exception guarantee.
		copy.replace(copy.begin()+(first-begin()),
		             copy.begin()+(last-begin()),
		             first2, last2, typename STD::iterator_traits<InputIt>::iterator_category{});
		return *this = copy;
	}

	constexpr BasicString& replace( const_iterator first, const_iterator last,
	                                STD::initializer_list<value_type> ilist ) {
		return replace(first, last, STD::begin(ilist), STD::end(ilist));
	}

	constexpr BasicString& replace( const_iterator first, const_iterator last,
	                                size_type count2, value_type ch ) {
		size_type count = last-first;
		if (count < count2)
			this->_createInsertionSpace(last, count2-count);
		else if (count2 < count)
			erase(first+count2, last);
		traits_type::assign(this->_address(first), count2, ch);
		return *this;
	}

	constexpr BasicString substr(size_type pos, size_type count=npos) const {
		AssertExcept<STD::out_of_range>(pos <= size(), "Invalid start position of substr");
		return BasicString(data()+pos, STD::min(count, size()-pos));
	}

	constexpr size_type copy( pointer dest, size_type count, size_type pos = 0) const {
		AssertExcept<STD::out_of_range>(pos < size(), "Invalid start position of copy");
		return traits_type::copy(dest, data()+pos, STD::min(count, size()-pos)) - dest;
	}

	void resize(size_type n) {_base::resize(n);}
	void resize(size_type n, value_type c) {_base::resize(n, c);}

	void push_back(value_type c) {_base::push_back(c);}

	template <STD::size_t OtherMax, typename OtherTraits>
	constexpr void swap( BasicString<value_type, OtherMax, OtherTraits>& other ) {
		_base::swap(other);
	}

	/**< --------- @FIND ---------*/

	//! TODO: Implement Boyer-Moore.
	constexpr size_type find(const_pointer str, size_type pos, size_type count) const {
		if (pos < size()) {
			auto it = Constainer::search(begin()+pos, end(), str, str+count,
			                             traits_type::eq);
			if (it != end())
				return it-begin();
		}
		return npos;
	}
	constexpr size_type find(const_pointer str, size_type pos=0) const {
		return find(str, pos, traits_type::length(str));
	}
	constexpr size_type find(value_type ch, size_type pos=0) const {
		if (pos < size())
			if (auto p = traits_type::find(data()+pos, size()-pos, ch))
				return p-data();

		return npos;
	}
	template <STD::size_t OtherMax>
	constexpr size_type find(ThisResized<OtherMax> const& other, size_type pos = 0) const {
		return find(other.data(), pos, other.size());
	}

	/**< --------- @RFIND ---------*/

	constexpr size_type rfind(const_pointer str, size_type pos, size_type count) const {
		if (count == 0)
			return STD::min(size(), pos);
		if (!empty()) {
			pos = STD::min(pos, size()-1);
			auto it = Constainer::search(rbegin()+(size()-pos-1), rend(),
			                             Constainer::make_reverse_iterator(str+count),
			                             Constainer::make_reverse_iterator(str),
			                             traits_type::eq);
			if (it != rend())
				return it.base()-begin()-count;
		}
		return npos;
	}
	constexpr size_type rfind(const_pointer str, size_type pos=npos) const {
		return rfind(str, pos, traits_type::length(str));
	}
	constexpr size_type rfind(value_type ch, size_type pos=npos) const {
		return rfind(&ch, pos, 1);
	}
	template <STD::size_t OtherMax>
	constexpr size_type rfind(ThisResized<OtherMax> const& other, size_type pos = npos) const {
		return rfind(other.data(), pos, other.size());
	}

	/**< --------- @FIND_FIRST_OF ---------*/

	constexpr size_type find_first_of(const_pointer str, size_type pos, size_type count) const {
		auto it = Constainer::find_first_of(begin()+pos, end(), str, str+count, traits_type::eq);
		if (it == end())
			return npos;
		return it-begin();
	}
	constexpr size_type find_first_of(const_pointer str, size_type pos=0) const {
		return find_first_of(str, pos, traits_type::length(str));
	}
	constexpr size_type find_first_of(value_type ch, size_type pos=0) const {
		return find(ch, pos);
	}
	template <STD::size_t OtherMax>
	constexpr size_type find_first_of(ThisResized<OtherMax> const& other, size_type pos = 0) const {
		return find_first_of(other.data(), pos, other.size());
	}

	/**< --------- @FIND_FIRST_NOT_OF ---------*/

	constexpr size_type find_first_not_of(const_pointer str, size_type pos, size_type count) const {
		auto it = Constainer::find_first_not_of(begin()+pos, end(), str, str+count, traits_type::eq);
		if (it == end())
			return npos;
		return it-begin();
	}
	constexpr size_type find_first_not_of(const_pointer str, size_type pos=0) const {
		return find_first_not_of(str, pos, traits_type::length(str));
	}
	constexpr size_type find_first_not_of(value_type ch, size_type pos=0) const {
		//! TODO: Implement STD::not1 and implement using find_if with not1(traits::eq)
		return find_first_not_of(&ch, pos, 1);
	}
	template <STD::size_t OtherMax>
	constexpr size_type find_first_not_of(ThisResized<OtherMax> const& other, size_type pos = 0) const {
		return find_first_not_of(other.data(), pos, other.size());
	}

	/**< --------- @FIND_LAST_OF ---------*/

	constexpr size_type find_last_of(const_pointer str, size_type pos, size_type count) const {
		pos = STD::min(pos, size()-1);
		auto it = Constainer::find_first_of(rbegin()+(size()-pos-1), rend(), str, str+count, traits_type::eq);
		if (it.base() == begin())
			return npos;
		return it.base()-1-begin();
	}
	constexpr size_type find_last_of(const_pointer str, size_type pos=npos) const {
		return find_last_of(str, pos, traits_type::length(str));
	}
	constexpr size_type find_last_of(value_type ch, size_type pos=npos) const {
		return rfind(ch, pos);
	}
	template <STD::size_t OtherMax>
	constexpr size_type find_last_of(ThisResized<OtherMax> const& other, size_type pos = npos) const {
		return find_last_of(other.data(), pos, other.size());
	}

	/**< --------- @FIND_LAST_NOT_OF ---------*/

	constexpr size_type find_last_not_of(const_pointer str, size_type pos, size_type count) const {
		pos = STD::min(pos, size()-1);
		auto it = Constainer::find_first_not_of(rbegin()+(size()-pos-1), rend(), str, str+count, traits_type::eq);
		if (it.base() == begin())
			return npos;
		return it.base()-1-begin();
	}
	constexpr size_type find_last_not_of(const_pointer str, size_type pos=npos) const {
		return find_last_not_of(str, pos, traits_type::length(str));
	}
	constexpr size_type find_last_not_of(value_type ch, size_type pos=npos) const {
		return find_last_not_of(&ch, pos, 1);
	}
	template <STD::size_t OtherMax>
	constexpr size_type find_last_not_of(ThisResized<OtherMax> const& other, size_type pos = npos) const {
		return find_last_of(other.data(), pos, other.size());
	}
};

template <typename Char, STD::size_t N1, STD::size_t N2, typename Traits1, typename Traits2>
constexpr void swap( BasicString<Char, N1, Traits1>& lhs, BasicString<Char, N2, Traits2>& rhs ) {
	lhs.swap(rhs);
}

using String64   = BasicString<char,   64>;
using String256  = BasicString<char,  256>;
using String512  = BasicString<char,  512>;
using String1024 = BasicString<char, 1024>;

using String = String256;

constexpr String256 operator"" _cstr ( char const* str, STD::size_t len ) {return {str, len};}
constexpr String512 operator"" _lcstr( char const* str, STD::size_t len ) {return {str, len};}

}

#include <ostream>

/**< The following three functions have been taken from libstdc++ and adjusted. Creds to the developers of that implementation. */

namespace Constainer {
namespace detail {
	template <typename Char, typename Traits>
	void writeTo(STD::basic_ostream<Char, Traits>& os,
	             Char const* s, STD::streamsize n) {
		if (os.rdbuf()->sputn(s, n) != n)
			os.setstate(STD::ios_base::badbit);
	}

	template <typename Char, typename Traits>
	void fill(STD::basic_ostream<Char, Traits>& os, STD::streamsize n) {
		auto c = os.fill();
		while (n-- > 0)
			if (Traits::eq_int_type(os.rdbuf()->sputc(c), Traits::eof())) {
				os.setstate(STD::ios_base::badbit);
				break;
			}
	}
}

template <typename Char, STD::size_t N, typename Traits, typename OTraits>
STD::basic_ostream<Char, OTraits>& operator<<( STD::basic_ostream<Char, OTraits>& os,
                                               BasicString<Char, N, Traits> const& str )
{
	if (typename STD::basic_ostream<Char, OTraits>::sentry sentry{os})
		try {
			auto n = str.size();
			STD::size_t w = os.width();
			if (n < w) {
				bool left_aligned = (os.flags() & STD::ios_base::adjustfield) == STD::ios_base::left;
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
			os.setstate(STD::ios_base::badbit);
		}

	return os;
}

template <typename Char, STD::size_t N1, STD::size_t N2, typename Traits>
constexpr auto operator+(BasicString<Char, N1, Traits> const& lhs,
                         BasicString<Char, N2, Traits> const& rhs) {
	BasicString<Char, STD::max(N1, N2), Traits> result = lhs;
	return result += rhs;
}

template <typename Char, STD::size_t N, typename Traits>
constexpr auto operator+(Char const* lhs, BasicString<Char, N, Traits> const& rhs) {
	BasicString<Char, N, Traits> result = rhs;
	return result.insert(0, lhs);
}
template <typename Char, STD::size_t N, typename Traits>
constexpr auto operator+(BasicString<Char, N, Traits> const& lhs, Char const* rhs) {
	auto result = lhs;
	return result += rhs;
}
template <typename Char, STD::size_t N, typename Traits>
constexpr auto operator+(BasicString<Char, N, Traits> const& lhs, Char rhs) {
	auto result = lhs;
	return result += rhs;
}
template <typename Char, STD::size_t N, typename Traits>
constexpr auto operator+(Char lhs, BasicString<Char, N, Traits> const& rhs) {
	auto result = rhs;
	return result.insert(0, lhs);
}

//! Note: The move overloads are not provided as they aren't beneficial for this implementation

/**< A load of relational operators follows. */

template <typename CharT, STD::size_t N1, STD::size_t N2, typename Traits>
constexpr bool operator==( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return lhs.compare(rhs) == 0;
}
template <typename CharT, STD::size_t N1, STD::size_t N2, typename Traits>
constexpr bool operator!=( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return !(lhs == rhs);
}
template <typename CharT, STD::size_t N1, STD::size_t N2, typename Traits>
constexpr bool operator <( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return lhs.compare(rhs) < 0;
}
template <typename CharT, STD::size_t N1, STD::size_t N2, typename Traits>
constexpr bool operator<=( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return !(lhs > rhs);
}
template <typename CharT, STD::size_t N1, STD::size_t N2, typename Traits>
constexpr bool operator >( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return rhs < lhs;
}
template <typename CharT, STD::size_t N1, STD::size_t N2, typename Traits>
constexpr bool operator>=( BasicString<CharT,N1,Traits> const& lhs,
                           BasicString<CharT,N2,Traits> const& rhs ) {
	return rhs <= lhs;
}

/**< char-ptr <=> BasicString relations */

template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator==( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return rhs.compare(lhs) == 0;
}
template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator==( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return rhs == lhs;
}

template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator!=( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs == rhs);
}
template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator!=( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return rhs != lhs;
}

template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator <( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return rhs.compare(lhs) > 0;
}
template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator <( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return rhs > lhs;
}

template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator<=( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs > rhs);
}
template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator<=( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return !(lhs > rhs);
}

template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator >( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs <= rhs);
}
template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator >( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return !(lhs <= rhs);
}

template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator>=( const CharT* lhs, const BasicString<CharT,N,Traits>& rhs ) {
	return !(lhs < rhs);
}
template <typename CharT, STD::size_t N, typename Traits>
constexpr bool operator>=( const BasicString<CharT,N,Traits>& lhs, const CharT* rhs ) {
	return !(lhs < rhs);
}

/** \brief The standard string hash function used by this library.
 *         It currently works for size_t having size 4 and 8.
 *
 * \param str The string to hash. Is not required to be null-terminated.
 * \param len The length of that string.
 * \return The hash value as computed by the FNV-1 hash function.
 *
 */

constexpr STD::size_t hash(const char* str, STD::size_t len) {
	const STD::size_t prime = sizeof(STD::size_t) == 8? 0x00000100000001b3 : 0x1000193;
	      STD::size_t h     = sizeof(STD::size_t) == 8? 0xcbf29ce484222325 : 0x811C9DC5;
	while (len--)
		h = (h ^ *str++) * prime;
	return h;
}
constexpr STD::size_t hash(const char* str) {
	return hash(str, CharTraits<char>::length(str));
}
template <STD::size_t N, typename Traits>
constexpr STD::size_t hash(BasicString<char, N, Traits> const& str) {
	return hash(str.c_str(), str.size());
}

}
