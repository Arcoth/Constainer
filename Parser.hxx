/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#ifndef PARSER_HXX_INCLUDED
#define PARSER_HXX_INCLUDED

#include "String.hxx"

namespace Constainer {

enum class ParserState {Good, Eof, Fail};

template <typename Int, typename InputIt>
constexpr auto strToInt( InputIt first, InputIt last, Int& res, int base )
	-> std::enable_if_t<std::is_integral<Int>{}, std::pair<InputIt, ParserState>>
{
	String whitespace = " \t\n\f\v\r";
	String digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	std::make_signed_t<Int> sign = 1;
	first = Constainer::find_first_not_of(first, last, whitespace.begin(), whitespace.end());

	if (first == last) {res = 0; return {first, ParserState::Eof};}

	if (*first == '-') {
		sign = -1;
		++first;
	}
	else if (*first == '+')
		++first;

	if (base == 0) {

		if (first == last) {res = 0; return {first, ParserState::Eof};}
		auto suffix_1 = *first++;
		if (suffix_1 != '0')
			base = 10;
		else {
			if (first == last) {res = 0; return {first, ParserState::Eof};}
			auto suffix_2 = *first;
			if (suffix_2 == 'X' || suffix_2 == 'x') {
				base = 16;
				++first;
			}
			else
				base = 8;
		}
	}

	res = 0;
	bool written = false;
	typename decltype(digits)::size_type found=0;
	while (first != last
	   && (found = digits.find(toupper(*first))) != digits.npos)
	{
		const auto summand = sign*Int(found);

		constexpr auto MAX = std::numeric_limits<Int>::max(),
				   MIN = std::numeric_limits<Int>::min();

		// Overflow check. Separate because of two's complement.

		if (sign ==  1)
			if (res > MAX/base || res*base > MAX-summand) {
				res = MAX;
				return {first, ParserState::Fail};
			}
		if (sign == -1)
			if (res < MIN/base || res*base < MIN-summand) {
				res = MIN;
				return {first, ParserState::Fail};
			}

		(res *= base) += summand;

		written = true;
		++first;
	}

	if (!written) {res = 0; return {first, ParserState::Eof};}

	return {first, ParserState::Good};
}

template <typename Int>
constexpr auto strToInt( char const* str, std::size_t len, std::size_t* pos = 0, int base = 10 )
	-> std::enable_if_t<std::is_integral<Int>{}, Int> {
	Int ret = 0;

	auto pair = strToInt<Int>(str, str+len, ret, base);
	AssertExcept<std::invalid_argument>(pair.second != ParserState::Eof,  "Could not extract any integer");
	AssertExcept<std::out_of_range>    (pair.second != ParserState::Fail, "Integer represented is too large");

	if (pos)
		*pos = pair.first - str;

	return ret;
}

template <typename Int>
constexpr auto strToInt( char const* str, std::size_t* pos = 0, int base = 10 )
	-> std::enable_if_t<std::is_integral<Int>{}, Int> {
	return strToInt<Int>(str, CharTraits<char>::length(str), pos, base);
}

template <typename Int, std::size_t N>
constexpr auto strToInt( BasicString<char, N> const& str, std::size_t* pos = 0, int base = 10 )
	-> std::enable_if_t<std::is_integral<Int>{}, Int> {
	return strToInt<Int>(str.data(), str.size(), pos, base);
}

}

#endif // PARSER_HXX_INCLUDED
