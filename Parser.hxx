/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#ifndef PARSER_HXX_INCLUDED
#define PARSER_HXX_INCLUDED

#include "String.hxx"
#include "Math.hxx"

namespace Constainer {

enum class PState {Good, Eof, Fail};
template <typename Iterator>
struct ParserState {
	Iterator iterator;
	PState state;
};

namespace detail {

	template <typename InputIt>
	constexpr InputIt skipWS(InputIt first, InputIt last) {
		String whitespace = " \t\n\f\v\r";
		return Constainer::find_first_not_of(first, last, whitespace.begin(), whitespace.end());
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
 *         returned struct, which is of type PState.
 *
 *         Valid values for the base range from 2 to 36, but 0 is also one;
 *         In the latter case, the bases actual value will be deduced from the prefix.
 *         If the prefix is 0x or 0X, the base is 16. If the prefix is a sole 0, the
 *         base is 8. The base is 10 otherwise.
 *
 *         If the parsed value is larger or smaller than Int can represent, the returned state
 *         will be PState::Fail - in that case the value of the object referred to by res is set
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
constexpr auto strToInt( InputIt first, InputIt last, Int& res, int base=10 )
	-> requires<std::is_integral<Int>, ParserState<InputIt>>
{
	constexpr auto MAX = std::numeric_limits<Int>::max(),
	               MIN = std::numeric_limits<Int>::lowest();

	String digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	res = 0; // So premature error returns set res to zero

	first = detail::skipWS(first, last);
	if (first == last)
		return {first, PState::Eof};

	auto sign = detail::parseSign<Int>(first);

	if (base == 0) {

		if (first == last) return {first, PState::Eof};
		auto suffix_1 = *first++;
		if (suffix_1 != '0')
			base = 10;
		else {
			if (first == last) return {first, PState::Eof};
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

		// Overflow check. Separate because of two's complement.

		if (sign ==  1)
			if (res > MAX/base || res*base > MAX-summand) {
				res = MAX;
				return {first, PState::Fail};
			}
		if (sign == -1)
			if (res < MIN/base || res*base < MIN-summand) {
				res = MIN;
				return {first, PState::Fail};
			}

		(res *= base) += summand;

		read = true;
		++first;
	}

	if (!read) {res = 0; return {first, PState::Eof};}

	return {first, PState::Good};
}

template <typename Int>
constexpr auto strToInt( char const* str, std::size_t len, std::size_t* pos = 0, int base = 10 )
	-> requires<std::is_integral<Int>, Int> {
	Int ret = 0;

	auto st = strToInt<Int>(str, str+len, ret, base);
	AssertExcept<std::invalid_argument>(st.state != PState::Eof,  "Could not extract any integer");
	AssertExcept<std::out_of_range>    (st.state != PState::Fail, "Integer represented is out of bounds");

	if (pos)
		*pos = st.iterator - str;

	return ret;
}

template <typename Int>
constexpr auto strToInt( char const* str, std::size_t* pos = 0, int base = 10 )
	-> requires<std::is_integral<Int>, Int> {
	return strToInt<Int>(str, CharTraits<char>::length(str), pos, base);
}

template <typename Int, std::size_t N>
constexpr auto strToInt( BasicString<char, N> const& str, std::size_t* pos = 0, int base = 10 )
	-> requires<std::is_integral<Int>, Int> {
	return strToInt<Int>(str.data(), str.size(), pos, base);
}

template <typename Float, typename InputIt>
constexpr auto strToFloat( InputIt first, InputIt last, Float& res )
	-> requires<std::is_floating_point<Float>, ParserState<InputIt>>
{
	String digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	res = 0; // So a premature return (see below) does not leave res unset

	first = detail::skipWS(first, last);
	if (first == last)
		return {first, PState::Eof};

	auto sign = detail::parseSign<Float>(first);

	if (first == last) return {first, PState::Eof};

	int base = 10;

	bool read = false;

	switch (toupper(*first)) {
	case '0':
		++first;
		if (first == last) {
			res = sign*0.;
			return {first, PState::Good};
		}
		if (toupper(*first) == 'X') {
			base = 16;
			++first;
			if (first == last) return {first, PState::Eof};
		}
		else
			read = true;
		break;

	case 'I': {
		++first;
		char const  inf_str[] = "NFINITY",
		           *p         = inf_str;

		while (first != last && *p && toupper(*first) == *p) {
			++first; ++p;
		}

		switch (p-inf_str) {
		case 2: case 7:
			if (!std::numeric_limits<Float>::has_infinity) {
				return {first, PState::Fail};
			}

			res = sign * std::numeric_limits<Float>::infinity();
			return {first, PState::Good};
		default:
			return {first, PState::Fail};
		}
	}
	case 'N': {
		++first;
		char const  nan_str[] = "AN",
		           *p         = nan_str;

		while (first != last && *p && toupper(*first) == *p) {
			++first;
			++p;
		}

		if (p-nan_str != 2)
			return {first, PState::Eof};

		if (not std::numeric_limits<Float>::has_quiet_NaN)
			return {first, PState::Fail};

		if (first != last && *first == '(') {
			//! TODO: Implement proper NaN-String recognition
			first = Constainer::find_first_not_of(first+1, last, digits.begin(), digits.end());
			if (first == last || *first !=  ')')
				return {first, PState::Eof};
		}

		res = std::numeric_limits<Float>::quiet_NaN();
		return {first, PState::Good};
	}
	}

	Float multiplier = 1;
	for (; first != last; ++first)
	{
		std::size_t found = digits.rfind(toupper(*first), base-1);
		if (found == digits.npos) {
			if (*first != '.' || multiplier != 1)
				break;
			multiplier /= base;
			continue;
		}

		const auto summand = sign*multiplier*Float(found);

		constexpr auto bound = std::numeric_limits<Float>::max();
		if (abs(res) > bound/base || abs(res)*base > bound-abs(summand)) {
			res = bound;
			return {first, PState::Fail};
		}

		(res *= (multiplier == 1? base : 1)) += summand;

		read = true;

		if (multiplier != 1)
			multiplier /= base;
	}

	if (!read) {res = 0; return {first, PState::Eof};}

	// Apply exponent
	const char exp_char = base==10? 'E' : 'P';
	if (first != last && toupper(*first) == exp_char) {
		++first;
		int exp=0;
		auto p_state = strToInt(first, last, exp, 10); // exponent is always decimal
		first = p_state.iterator;
		if (p_state.state != PState::Good) {
			res = 0;
			return {first, p_state.state};
		}
		auto factor = pow(Float(2), exp);
		res = safeMul(res, factor);
	}

	return {first, PState::Good};
}

template <typename Float>
constexpr auto strToFloat( char const* str, std::size_t len, std::size_t* pos = 0 )
	-> requires<std::is_floating_point<Float>, Float> {
	Float ret = 0;

	auto st = strToFloat<Float>(str, str+len, ret);
	AssertExcept<std::invalid_argument>(st.state != PState::Eof,  "Could not extract any integer");
	AssertExcept<std::out_of_range>    (st.state != PState::Fail, "Floating point represented is out of bounds");

	if (pos)
		*pos = st.iterator - str;

	return ret;
}

template <typename Float>
constexpr auto strToFloat( char const* str, std::size_t* pos = 0 )
	-> requires<std::is_floating_point<Float>, Float> {
	return strToFloat<Float>(str, CharTraits<char>::length(str), pos);
}


}

#endif // PARSER_HXX_INCLUDED
