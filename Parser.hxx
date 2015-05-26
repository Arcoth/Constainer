/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#ifndef PARSER_HXX_INCLUDED
#define PARSER_HXX_INCLUDED

#include "String.hxx"

namespace Constainer {


enum class ParserState {Good, Eof, Fail};

namespace detail {

	template <typename InputIt>
	constexpr ParserState skipWS(InputIt& first, InputIt last) {
		String whitespace = " \t\n\f\v\r";

		first = Constainer::find_first_not_of(first, last, whitespace.begin(), whitespace.end());

		return first == last? ParserState::Eof : ParserState::Good;
	}

	template <typename Arithmetic, typename InputIt>
	constexpr auto parseSign( InputIt& first )
	 -> typename std::conditional_t<std::is_integral<Arithmetic>{}, std::make_signed<Arithmetic>, std::remove_cv<Arithmetic>>::type
	{
		if (*first == '-') {
			++first;
			return -1;
		}
		if (*first == '+')
			++first;

		return 1;
	}
}

/** \brief Extracts an integer value from the character range specified by [first, last).
 *         Very similar to std::strtol. Errors are indicated via the second member of the
 *         returned struct, which is of type ParserState.
 *
 *         Valid values for the base range from 2 to 36, but 0 is also one;
 *         In the latter case, the bases actual value will be deduced from the prefix.
 *         If the prefix is 0x or 0X, the base is 16. If the prefix is a sole 0, the
 *         base is 8. The base is 10 otherwise.
 *
 *         If the parsed value is larger or smaller than Int can represent, the returned state
 *         will be ParserState::Fail - in that case the value of the object referred to by res is set
 *         to the clamped value of the parsed value, that is, the largest or smallest one representable
 *         by Int, respectively. If the string does not represent a valid integer, the state is Eof.
 *         It is Good otherwise.
 *
 * \param first An iterator to the first element of the character sequence to extract the values from.
 * \param first An iterator to the first element of the character sequence to extract the values from.
 * \param res A reference to an object in which the result will be stored
 * \param base The base of the representation to parse.
 *             Can be zero, in which case it will be deduced from the character sequence.
 * \return A pair that contains both the iterator to the first non-consumed character
 *         and the state that the parser had at the end of the operation.
 *
 */
template <typename Int, typename InputIt>
constexpr auto strToInt( InputIt first, InputIt last, Int& res, int base )
	-> std::enable_if_t<std::is_integral<Int>{}, std::pair<InputIt, ParserState>>
{
	String digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	res = 0; // So premature error returns set res to zero

	if (detail::skipWS(first, last) == ParserState::Eof)
		return {first, ParserState::Eof};

	auto sign = detail::parseSign<Int>(first);

	if (base == 0) {

		if (first == last) return {first, ParserState::Eof};
		auto suffix_1 = *first++;
		if (suffix_1 != '0')
			base = 10;
		else {
			if (first == last) return {first, ParserState::Eof};
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
	bool read = false;
	std::size_t found=0;
	while (first != last
	   && (found = digits.rfind(toupper(*first), base-1)) != digits.npos)
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

		read = true;
		++first;
	}

	if (!read) {res = 0; return {first, ParserState::Eof};}

	return {first, ParserState::Good};
}

template <typename Int>
constexpr auto strToInt( char const* str, std::size_t len, std::size_t* pos = 0, int base = 10 )
	-> std::enable_if_t<std::is_integral<Int>{}, Int> {
	Int ret = 0;

	auto pair = strToInt<Int>(str, str+len, ret, base);
	AssertExcept<std::invalid_argument>(pair.second != ParserState::Eof,  "Could not extract any integer");
	AssertExcept<std::out_of_range>    (pair.second != ParserState::Fail, "Integer represented is tout of bounds");

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
