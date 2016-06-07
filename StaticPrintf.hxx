/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "String.hxx"
#include "Stack.hxx"
#include "Math.hxx"
#include "RangeAccess.hxx"

#include <type_traits>

namespace Constainer { namespace detail {

template <typename StringType, typename T, typename P>
constexpr require<STD::is_floating_point<T>> writeDigitsReversed ( StringType& str, T t, int base, P digits ) {
	do {
		str += digits[ (STD::size_t) remainder (t, base)];
		t /= base;
	}
	while (t >= 1 || t <= -1);
}
template <typename StringType, typename T, typename P>
constexpr require<STD::is_integral<T>> writeDigitsReversed ( StringType& str, T t, int base, P digits ) {
	do {
		str += digits[t % base];
		t /= base;
	}
	while (t != 0);
}

template <typename Char>
struct DefaultTokens;
template <>
struct DefaultTokens<char> {
	static constexpr char minus          = '-', plus           = '+',
	                      string         = 's', character      = 'c',
	                      decimalInt     = 'd', signedInt      = 'i', unsignedInt    = 'u',
	                      upperHex       = 'X', lowerHex       = 'x',
	                      upperExp       = 'E', lowerExp       = 'e',
	                      upperHexp      = 'P', lowerHexp      = 'p',
	             /*lower*/octal          = 'o',
	                      upperFixed     = 'F', lowerFixed     = 'f',
	                      upperHybrid    = 'G', lowerHybrid    = 'g',
	                      upperHexfloat  = 'A', lowerHexfloat  = 'a',

	                      percent        = '%', dollar         = '$', space          = ' ',
	                      decimalPoint   = '.', zero           = '0', asterisk       = '*',
	                      universal      = '@', internal_align = '_', central_align  = '=',
	                      alternative    = '#';

	static constexpr char const *lowerInf = "inf", *upperInf = "INF",
	                            *lowerNaN = "nan", *upperNaN = "NAN";

	static constexpr char const *lowerHexPrefixReversed = "x0", *upperHexPrefixReversed = "X0";


	static constexpr char const* get_digits (bool upper) {
		return (char const*) "0123456789abcdef0123456789ABCDEF" + upper*16;
	}

	static constexpr bool is_dec_digit ( char c ) {
		return c >= '0' && c <= '9';
	}
};


/**< The class template surrounding the parser utility and parametrising character type and tokens. */
template <typename CharT, typename Tokens=DefaultTokens<CharT>, typename Traits=CharTraits<CharT>>
struct Parser {

	enum class IndexConsistency {NoCommitment, Running, NotRunning};

	template <IndexConsistency C> static constexpr void _assertRunning() {
		static_assert(C == IndexConsistency::NoCommitment || C == IndexConsistency::   Running, "Can't interchangeably use running and specified indices!");
	}
	template <IndexConsistency C> static constexpr void _assertNotRunning() {
		static_assert(C == IndexConsistency::NoCommitment || C == IndexConsistency::NotRunning, "Can't interchangeably use running and specified indices!");
	}

	struct Info
	{
		int width = 0, precision = -1;
		int base = 10;
		CharT pad = Tokens::space;
		CharT prepend = '\0';
		enum FieldAlignment { left_justified=0b000,
		                     right_justified=0b001,
		                            internal=0b011,
		                             central=0b100} alignment = right_justified;
		bool alternative = false;
		bool upper = false;
	};

	template <typename Stream>
	static constexpr void applyInfoToStream( Stream& stream, Info const& info ) {
		stream.precision(info.precision);
		stream.width(info.width);
		stream.fill(info.pad);

		// This is not necessary, as we pad the field ourselves
		/*switch (info.alignment) {
		case Info::left_justified:
			stream.setf(Stream::left, Stream::adjustfield);
		break;
		case Info::right_justified: case Info::central:
			stream.setf(Stream::right, Stream::adjustfield);
		break;
		default: // Info::internal
			stream.setf(Stream::internal, Stream::adjustfield);
		};*/

		if (info.upper)
			stream.setf(Stream::uppercase);
		if (info.prepend == '+')
			stream.setf(Stream::showpos);

		if (info.base == 10) stream.setf(Stream::dec, Stream::basefield);
		else if (info.base == 16) stream.setf(Stream::hex, Stream::basefield);
		else /*info.base == 8*/ stream.setf(Stream::oct, Stream::basefield);
	}

	template <typename StringType>
	static constexpr void pad_central ( StringType& str, typename StringType::size_type needed , CharT c ) {
		str.append (needed/2, c);
		str.insert (0, (needed+1)/2, c);
	}

	template <typename StringType>
	static constexpr void pad_unformatted ( StringType& str, Info const& info )
	{
		if (str.size() < (STD::size_t)info.width) {
			auto needed = info.width - str.size();
			switch (info.alignment) {
				case Info::left_justified:
					str.append (needed, info.pad);
				break;
				case Info::central:
					pad_central(str, needed, info.pad);
				break;
				default: // internal and right-justified
					str.insert (0, needed, info.pad);
			}
		}
	}

	template <bool needsSign, typename StringType>
	static constexpr void pad_and_reverse ( STD::bool_constant<needsSign>, StringType& str, Info const& info )
	{
		if (needsSign && str.back() != Tokens::minus && info.prepend != '\0')
			str += info.prepend;

		if (str.size() < (STD::size_t)info.width)
		{
			auto needed = info.width - str.size();
			if (info.alignment & Info::right_justified)
			{
				auto pos = str.size();
				if (info.alignment == Info::internal) {
					if (str[pos-1]==Tokens::space
					 || str[pos-1]==Tokens::plus
					 || str[pos-1]==Tokens::minus)
						--pos;
					constexpr CharT hex[] {Tokens::lowerHex, Tokens::upperHex, '\0'};
					if (str.find_last_of (hex, pos-2) != String::npos)
						pos -= 2;
				}
				str.insert (pos, needed, info.pad);
			}
			// As the string is inverted at the moment, left justification means inserting padding on the left side
			else if (info.alignment == Info::left_justified)
				str.insert (0, needed, info.pad);
			else /* central alignment */
				pad_central(str,  needed, info.pad);
		}

		Constainer::reverse (str.begin(), str.end() );
	}

	template <typename StringType, typename Real>
	static constexpr StringType handleAbnormality ( Real r, bool upper ) {
		StringType s;

		if (r < STD::numeric_limits<Real>::lowest() ) {
			s = Tokens::minus;
			r = -r;
		}
		if (r > STD::numeric_limits<Real>::max() )
			s += upper? Tokens::upperInf : Tokens::lowerInf;
		else if (r != r)
			s = upper? Tokens::upperNaN : Tokens::lowerNaN;
		Constainer::reverse(s.begin(), s.end());
		return s;
	}

	template <typename StringType, typename Arg>
	static constexpr auto printFloat (bool trimTrailingZeroes, Info const& info, Arg arg, bool* rounding_overflow=nullptr)
	  -> require<STD::is_floating_point<Arg>, StringType>
	{
		auto str = handleAbnormality<StringType>(arg, info.upper);
		if (not str.empty())
			return str;

		bool negative = is_negative(arg);
		if (negative)
			arg = -arg;

		int precision = info.precision>=0? info.precision : 6;

		if (precision != 0 or info.alternative)
			str += Tokens::decimalPoint;
		auto arg_tmp = fractional (arg);
		auto digits = Tokens::get_digits (info.upper);

		for (; precision > 1; --precision)
		{
			arg_tmp *= info.base;
			STD::size_t rem = arg_tmp;
			str += digits[rem];
			arg_tmp -= rem;
		}
		// Last digit is rounded
		if (precision == 1)
		{
			arg_tmp *= info.base;
			auto digit_index = rounded_remainder (arg_tmp, info.base);
			if (digit_index == info.base)
			{
				digit_index = 0;
				int pos = str.size();
				while (--pos != 0)   // Until we reach the dot (which we know to have been written)
				{
					auto digit = Constainer::find (digits, digits+info.base, str[pos])-digits;
					if (digit == info.base-1)
						str[pos] = Tokens::zero;
					else
					{
						str[pos] = digits[digit+1];
						break;
					}
				}
				if (pos == 0)
				{
					++arg;
					if (arg >= info.base and rounding_overflow != nullptr)
					{
						arg = 1;
						*rounding_overflow = true;
					}
				}
			}
			str += digits[digit_index];
		}

		if (trimTrailingZeroes) {
			str.erase(str.find_last_not_of(Tokens::zero)+1); // Remove trailing zeroes
			if (str.back() == Tokens::decimalPoint and not info.alternative)
				str.pop_back();
		}

		Constainer::reverse (str.begin(), str.end() );
		writeDigitsReversed (str, arg, info.base, digits);
		if (negative)
			str += '-';

		return str;
	}

	template <typename StringType, typename Arg>
	static constexpr StringType printExponential (bool trimTrailingZeroes, Info info, Arg arg, int exponent)
	{
		auto str = handleAbnormality<StringType>(arg, info.upper);
		if (not str.empty())
			return str;

		constexpr CharT e_letter_arr[2][2] = {{Tokens::lowerExp , Tokens::upperExp},
		                                      {Tokens::lowerHexp, Tokens::upperHexp}};
		auto e_letter = e_letter_arr[info.base == 16][info.upper];

		if (info.precision < 0)
		{
			if (info.base == 16)
				// ::digits includes signbit, so +2 instead of +3
				info.precision = (STD::numeric_limits<Arg>::digits + 2) / 4;
			else
				info.precision = 6;
		}

		bool negative = is_negative (arg);
		if (negative)
			arg = -arg;

		auto digits = Tokens::get_digits (info.upper);

		bool rounding_overflow = false;
		auto mantissaString = printFloat<StringType>(trimTrailingZeroes, info, arg, &rounding_overflow);
		if (rounding_overflow)
			++exponent;
		writeDigitsReversed (str, abs (exponent), 10, digits);

		if (-info.base<exponent && exponent<info.base && info.base!=16)
			str += Tokens::zero;
		str += (exponent<0) ? Tokens::minus : Tokens::plus;
		str += e_letter;
		str += mantissaString;

		if (info.base == 16)
			str += info.upper? Tokens::upperHexPrefixReversed : Tokens::lowerHexPrefixReversed;

		if (negative)
			str += Tokens::minus;

		return str;
	}

	template <typename T>
	static constexpr T log10_2 = 0.3010299956639811952137388947244L;
	template <typename StringType, typename Arg>
	static constexpr StringType printExponential (bool trimTrailingZeroes, Info const& info, Arg arg)
	{
		static_assert( STD::is_floating_point<Arg>{}, "Invalid argument for exponential FP-formatting!" );

		auto norm = normalize (2, arg);
		if (info.base == 10) {
			auto exp = norm.second * log10_2<Arg>;
			auto mant_exp = exp - int(exp);
			auto mant = norm.first * pow10(mant_exp);
			if (mant >= 10) {
				mant /= 10;
				++exp;
			}
			else if (mant < 1) {
				mant *= 10;
				--exp;
			}
			return printExponential<StringType> (trimTrailingZeroes, info, mant, exp);
		}
		return printExponential<StringType> (trimTrailingZeroes, info, norm.first, norm.second);
	}

	template <typename StringType, typename Arg>
	static constexpr StringType printFloatHybrid (Info const& info, Arg arg)
	{
		static_assert( STD::is_floating_point<Arg>{}, "Invalid argument for hybrid FP-formatting!" );

		auto str = handleAbnormality<StringType>(arg, info.upper);
		if (not str.empty() )
			return str;

		auto data = normalize (info.base, arg);
		Arg norm_arg = data.first;
		int exp = data.second;
		int precision = info.precision;

		if (precision  < 0) precision = 6;
		if (precision == 0) precision = 1;

		auto info_adjusted = info;
		info_adjusted.precision = precision - 1;
		if (precision > exp && exp >= -4)
		{
			info_adjusted.precision -= exp;
			str = printFloat<StringType>(not info.alternative, info_adjusted, arg);
		}
		else
		{
			str = Parser::printExponential<StringType>(not info.alternative, info_adjusted, norm_arg, exp);
			//! str is still reversed!
			if (not info.alternative)
			{
				auto last_pos = str.find_first_not_of ("0123456789+-")+1;
				auto last_nonnull_pos = str.find_first_not_of (Tokens::zero, last_pos);
				auto count = last_nonnull_pos-last_pos;
				auto it = str.erase (last_pos, count);
				if (str[last_nonnull_pos-count] == Tokens::decimalPoint)
					str.erase (last_nonnull_pos-count, 1);
			}
		}
		return str;
	}

	template <typename StringType, typename Arg>
	static constexpr StringType printInt (Info const& info, Arg arg)
	{
		static_assert( STD::is_integral<Arg>{}, "Invalid use of integer formatting!" );

		// Default precision for integers is zero.
		int precision = info.precision >= 0? info.precision : 1;

		StringType str;
		// If precision is zero and the argument is zero, conversion results in no characters
		if (arg != Arg (0) )
			writeDigitsReversed (str, arg, info.base, Tokens::get_digits(info.upper) );
		if (str.size() < (STD::size_t)precision)
			str.append (precision-str.size(), Tokens::zero);
		// If the precision does not cause an additional 0 to be prepended, '#' does.
		else if (info.base == 8 && info.alternative)
			str += Tokens::zero;

		// If we're writing hexadec, a 0X/0x is prepended to any nonzero argument if '#' is set
		if (arg != 0 && info.alternative)
			str += info.upper? Tokens::upperHexPrefixReversed : Tokens::lowerHexPrefixReversed;

		if (arg < 0)
			str += Tokens::minus;
		return str;
	}

	template <typename StringType, typename P>
	static constexpr StringType printString ( int precision, P s ) {
		typename StringType::size_type len=0;
		while (precision-- && *s)
		{
			++len;
			++s;
		}
		return {s-len, len};
	}

	template <CharT... ch>
	struct parse;

	template <CharT... ch>
	struct handleRest : Info
	{
		static constexpr bool indexLeap = 1;

		using type = parse<ch...>;

		template <typename NeedSign, typename StringType>
		constexpr StringType pad_and_reverse (NeedSign b, StringType str) const {
			Parser::pad_and_reverse (b, str, *this);
			return str;
		}
		template <typename StringType>
		constexpr StringType pad_unformatted (StringType str) const {
			Parser::pad_unformatted (str, *this);
			return str;
		}
	};

	template <CharT... ch>
	struct handleFormatSpecifier;
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::character, ch...> : handleRest<ch...> {
		template <typename StringType>
		constexpr void evaluate ( StringType& string, CharT c ) {
			string += this->pad_and_reverse (STD::false_type {}, StringType{c} );
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::string, ch...> : handleRest<ch...> {
		template <typename StringType>
		constexpr void evaluate ( StringType& string, CharT const* s ) {
			string += this->pad_unformatted (printString<StringType>(this->precision, s) );
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::signedInt, ch...> : handleFormatSpecifier<Tokens::decimalInt, ch...> {};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::decimalInt, ch...> : handleFormatSpecifier<Tokens::universal, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			static_assert( STD::is_integral<Arg>{}, "Invalid use of format specifier for (signed) integers!" );
			handleFormatSpecifier<Tokens::universal, ch...>::evaluate(string, (STD::make_signed_t<Arg>)arg);
		}
	};

	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::unsignedInt, ch...> : handleFormatSpecifier<Tokens::universal, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			static_assert( STD::is_integral<Arg>{}, "Invalid use of format specifier for (unsigned) integers!" );
			handleFormatSpecifier<Tokens::universal, ch...>::evaluate(string, (STD::make_unsigned_t<Arg>)arg);
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::upperExp, ch...> : handleFormatSpecifier<Tokens::lowerExp, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			this->upper = true;
			handleFormatSpecifier<Tokens::lowerExp, ch...>::evaluate (string, arg);
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::lowerExp, ch...> : handleRest<ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			// Hexfloat fractional trailing zeroes are trimmed (at least with libc++)
			string += this->pad_and_reverse (STD::true_type {}, Parser::printExponential<StringType>(this->base==16, *this, arg) );
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::upperHybrid, ch...> : handleFormatSpecifier<Tokens::lowerHybrid, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg const& arg ) {
			this->upper = true;
			return handleFormatSpecifier<Tokens::lowerHybrid, ch...>::evaluate (string, arg);
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::lowerHybrid, ch...> : handleRest<ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& str, Arg arg ) {
			str += this->pad_and_reverse (STD::true_type {}, printFloatHybrid<StringType>(*this, arg) );
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::upperFixed, ch...> : handleFormatSpecifier<Tokens::lowerFixed, ch...> {
		template <typename StringType, typename Arg>
		constexpr StringType evaluate ( Arg arg ) {
			this->upper = true;
			return handleFormatSpecifier<Tokens::lowerFixed, ch...>::evaluate (arg);
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::lowerFixed, ch...> : handleRest<ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			string += this->pad_and_reverse(STD::true_type{}, printFloat<StringType>(STD::false_type{}, *this, arg));
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::upperHexfloat, ch...> : handleFormatSpecifier<Tokens::lowerHexfloat, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			this->upper = true;
			return handleFormatSpecifier<Tokens::lowerHexfloat, ch...>::evaluate (string, arg);
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::lowerHexfloat, ch...> : handleFormatSpecifier<Tokens::lowerExp, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			this->base = 16;
			handleFormatSpecifier<Tokens::lowerExp, ch...>::evaluate(string, arg);
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::upperHex, ch...> : handleFormatSpecifier<ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg const& arg ) {
			this->upper = true;
			this->base = 16;
			handleFormatSpecifier<ch...>::evaluate (string, arg);
		}
	};
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::lowerHex, ch...> : handleFormatSpecifier<Tokens::unsignedInt, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			this->base = 16;
			handleFormatSpecifier<Tokens::unsignedInt, ch...>::evaluate (string, arg);
		}
	};
	/*template <CharT... ch>
	struct handleFormatSpecifier<Tokens::upperOctal, ch...> : handleFormatSpecifier<Tokens::octal, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg const& arg ) {
			this->upper = true;
			handleFormatSpecifier<Tokens::octal, ch...>::evaluate (string, arg);
		}
	};*/
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::octal, ch...> : handleFormatSpecifier<Tokens::unsignedInt, ch...> {
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg arg ) {
			this->base = 8;
			handleFormatSpecifier<Tokens::unsignedInt, ch...>::evaluate (string, arg);
		}
	};
	/**< Generic format specifier */
	template <CharT... ch>
	struct handleFormatSpecifier<Tokens::universal, ch...> : handleRest<ch...>
	{
	private:
		template <typename StringType>
		struct buffer_t : STD::basic_streambuf<CharT>
		{
		private:
			StringType _str;

		public:
			using typename STD::basic_streambuf<CharT>::int_type;

			auto const& str() const {return _str;}

		protected:
			int_type overflow (int_type i) override {
				if (Traits::eq_int_type (i, Traits::eof() ) )
					return i;
				_str += i;
				return i;
			}

			STD::streamsize xsputn (CharT const* s, STD::streamsize count) override {
				_str.append (s, count);
				return count;
			}
		};

	public:

		template <typename StringType>
		constexpr void call ( detail::rank<1>, StringType& str, CharT const* s ) {
			str += this->pad_unformatted (printString<StringType>(this->precision, s));
		}

		template <typename StringType>
		constexpr void call ( detail::rank<1>, StringType& str, CharT c ) {
			str += this->pad_and_reverse (STD::false_type {}, StringType{c});
		}

		template <typename StringType, typename Arg>
		constexpr auto call ( detail::rank<1>, StringType& str, Arg arg )
		  -> require<STD::is_integral<Arg>>
		{
			str += this->pad_and_reverse (STD::is_signed<Arg>{}, printInt<StringType>(*this, arg) );
		}
		template <typename StringType, typename Arg>
		constexpr auto call ( detail::rank<1>, StringType& str, Arg arg )
		  -> require<STD::is_floating_point<Arg>>
		{
			str += this->pad_and_reverse (STD::true_type{}, printFloatHybrid<StringType>(*this, arg) );
		}
		template <typename StringType, typename Arg>
		void call ( detail::rank<10>, StringType& str, Arg const& arg )
		{
			buffer_t<StringType> buf;
			// Use standard library char_traits
			STD::basic_ostream<CharT> stream (&buf);
			applyInfoToStream(stream, *this);
			stream << arg;
			str += this->pad_unformatted( buf.str() );
		}

		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& str, Arg const& arg ) {
			call (detail::rank<0> {}, str, arg);
		}
	};

	template <typename, CharT... ch>
	struct handlePrecisionDigits : handleFormatSpecifier<ch...>
	{
		template <typename StringType, typename Arg>
		constexpr decltype (auto) evaluate ( StringType& string, Arg const& arg ) {
			return handleFormatSpecifier<ch...>::evaluate (string, arg);
		}
	};
	template <CharT c, CharT... ch>
	struct handlePrecisionDigits<STD::enable_if_t<Tokens::is_dec_digit (c) >, c, ch...>
		: handlePrecisionDigits<void, ch...>
	{
		template <typename StringType, typename Arg>
		constexpr void evaluate ( StringType& string, Arg const& arg )
		{
			this->precision *= 10;
			this->precision += c - Tokens::zero;
			handlePrecisionDigits<void, ch...>::evaluate (string, arg);
		}
	};

	/**< Primary template is not defined. If a decimal integer follows *, it must be part of an index terminating with $. */
	template <STD::size_t, typename, CharT... ch>
	struct handlePrecisionIndex;
	template <STD::size_t precision_index, CharT c, CharT... ch>
	struct handlePrecisionIndex<precision_index, STD::enable_if_t<Tokens::is_dec_digit(c)>, c, ch...>
		: handlePrecisionIndex<precision_index*10 + c-Tokens::zero, void, ch...> {};
	template <STD::size_t precision_index, CharT... ch>
	struct handlePrecisionIndex<precision_index, void, Tokens::dollar, ch...>
		: handleFormatSpecifier<ch...> {
		template <STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args ) {
			this->precision = STD::get<precision_index-1>(args);
			handleFormatSpecifier<ch...>::template evaluate (string, STD::get<index>(args));
		}
	};

	template <typename, CharT... ch>
	struct handlePrecision : handleFormatSpecifier<ch...>
	{
		template <IndexConsistency, STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args ) {
			handleFormatSpecifier<ch...>::evaluate (string, STD::get<index>(args));
		}
	};
	template <typename LessSpecialized, CharT... ch>
	struct handlePrecision<LessSpecialized, Tokens::decimalPoint, Tokens::asterisk, ch...>  : handleFormatSpecifier<ch...>
	{
		static constexpr auto indexLeap = handleFormatSpecifier<ch...>::indexLeap + 1;
		template <IndexConsistency consistency, STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args )
		{
			_assertRunning<consistency>();
			this->precision = STD::get<index> (args);
			handleFormatSpecifier<ch...>::evaluate (string, STD::get<index+1> (args) );
		}
	};
	template <CharT c, CharT... ch>
	struct handlePrecision<STD::enable_if_t<Tokens::is_dec_digit(c)>, Tokens::decimalPoint, Tokens::asterisk, c, ch...>
		: handlePrecisionIndex<c-Tokens::zero, void, ch...>
	{
		template <IndexConsistency consistency, STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args )
		{
			_assertNotRunning<consistency>();
			this->precision = STD::get<index> (args);
			handlePrecisionIndex<c-Tokens::zero, void, ch...>::template evaluate <index> (string, args);
		}
	};
	template <typename LessSpecialized, CharT... ch>
	struct handlePrecision<LessSpecialized, Tokens::decimalPoint, ch...> : handlePrecisionDigits<void, ch...>
	{
		template <IndexConsistency, STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args )
		{
			this->precision = 0;
			handlePrecisionDigits<void, ch...>::template evaluate (string, STD::get<index>(args) );
		}
	};

	template <typename, CharT... ch> struct handleWidth;

	template <STD::size_t width_or_index, typename, CharT... ch>
	struct handleWidthDigits : handlePrecision<void, ch...> {
		template <IndexConsistency consistency, STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args ) {
			this->width = width_or_index;
			handlePrecision<void, ch...>::template evaluate <consistency, index> (string, args);
		}
	};
	template <STD::size_t width_or_index, CharT c, CharT... ch>
	struct handleWidthDigits<width_or_index, STD::enable_if_t<Tokens::is_dec_digit (c) >, c, ch...>
		: handleWidthDigits<width_or_index*10 + c-Tokens::zero, void, ch...> {};

	/**< Primary template is not defined. If a decimal integer follows *, it must be part of an index terminating with $. */
	template <STD::size_t, typename, CharT... ch>
	struct handleWidthIndex;
	template <STD::size_t width_index, CharT c, CharT... ch>
	struct handleWidthIndex<width_index, STD::enable_if_t<Tokens::is_dec_digit(c)>, c, ch...>
		: handleWidthIndex<width_index*10 + c-Tokens::zero, void, ch...> {};
	template <STD::size_t width_index, CharT... ch>
	struct handleWidthIndex<width_index, void, Tokens::dollar, ch...>
		: handlePrecision<void, ch...> {
		template <IndexConsistency consistency, STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args ) {
			_assertNotRunning<consistency>();
			this->width = STD::get<width_index-1>(args);
			handlePrecision<void, ch...>::template evaluate <IndexConsistency::NotRunning, index> (string, args);
		}
	};

	template <typename, CharT... ch>
	struct handleWidth : handlePrecision<void, ch...> {};
	template <typename LessSpecialized, CharT... ch>
	struct handleWidth<LessSpecialized, Tokens::asterisk, ch...> : handlePrecision<void, ch...>
	{
		static constexpr auto indexLeap = handlePrecision<void, ch...>::indexLeap + 1;
		template <IndexConsistency consistency, STD::size_t index, typename StringType, typename ArgsTuple>
		constexpr void evaluate ( StringType& string, ArgsTuple const& args )
		{
			_assertRunning<consistency>();
			this->width = STD::size_t (STD::get<index> (args) );
			// negative width results in '-' being set and the corresponding positive width
			if (this->width < 0)
			{
				this->alignment = Info::left_justified;
				this->width = -this->width;
			}
			handlePrecision<void, ch...>::template evaluate <IndexConsistency::Running, index+1> (string, args);
		}
	};
	template <CharT c, CharT... ch>
	struct handleWidth<STD::enable_if_t<Tokens::is_dec_digit(c)>, Tokens::asterisk, c, ch...>
		: handleWidthIndex<c-Tokens::zero, void, ch...> {};
	template <CharT c, CharT... ch>
	struct handleWidth<STD::enable_if_t<Tokens::is_dec_digit(c)>, c, ch...>
		: handleWidthDigits<c-Tokens::zero, void, ch...> {};

	template <CharT... ch>
	struct handleFlags : handleWidth<void, ch...> {};

#define CONSTAINER_HANDLE_WIDTH_SPEC(c, ...)                                 \
	template <CharT... ch>                                                   \
	struct handleFlags<c, ch...> : handleFlags<ch...> {                      \
		template <IndexConsistency C, STD::size_t I, typename S, typename A> \
		constexpr void evaluate( S& s, A const& args ) {                     \
			__VA_ARGS__                                                      \
			handleFlags<ch...>::template evaluate<C, I>(s, args);            \
		}                                                                    \
	};

	CONSTAINER_HANDLE_WIDTH_SPEC (Tokens::plus, this->prepend = Tokens::plus;)
	CONSTAINER_HANDLE_WIDTH_SPEC (Tokens::space, if (this->prepend != Tokens::plus)
	                                                 this->prepend = Tokens::space;)
	CONSTAINER_HANDLE_WIDTH_SPEC (Tokens::minus, this->alignment = Info::left_justified; this->pad = Tokens::space;)
	CONSTAINER_HANDLE_WIDTH_SPEC (Tokens::alternative, this->alternative = true;)
	CONSTAINER_HANDLE_WIDTH_SPEC (Tokens::internal_align, this->alignment = Info::internal;)
	CONSTAINER_HANDLE_WIDTH_SPEC (Tokens::central_align, this->alignment = Info::central;)
	CONSTAINER_HANDLE_WIDTH_SPEC (Tokens::zero, if (this->alignment == Info::right_justified) {
	                                                this->pad = Tokens::zero;
	                                                this->alignment = Info::internal;})

#undef CONSTAINER_HANDLE_WIDTH_SPEC

	template <CharT... ch>
	struct genIndexInfo {
	private:
		struct indexInfo {
			STD::size_t index=0;
			STD::size_t read=1; // For the terminating char
			bool isPercentSignTerm=false;
		};
		static constexpr indexInfo isIndex(CharT const* str) {
			indexInfo info;

			for (; Tokens::is_dec_digit(*str); ++str) {
				info.index *= 10; info.index += *str - Tokens::zero;
				++info.read;
			}

			if (*str == Tokens::percent)
				info.isPercentSignTerm = true;
			else if (*str != Tokens::dollar)
				info.read = 0;

			return info;
		}

	public:
		static constexpr CharT str[] {ch...};
		static constexpr indexInfo info = isIndex(str);
	};

	/**< The bool parameter is true if we have a format specifier a la %2%, as e.g. allowed by Boost.Format. */
	template <bool isPercentTerminated, typename Arr, typename=STD::make_index_sequence<sizeof(Arr::str)-Arr::info.read>>
	struct handleArgIndex;
	template <typename Arr, STD::size_t... indices>
	struct handleArgIndex<false, Arr, STD::index_sequence<indices...>>
		: handleFlags<Arr::str[Arr::info.read + indices]...> {
		template <IndexConsistency C, STD::size_t I, typename StringType, typename ArgsTuple>
		constexpr void evaluate( StringType& string, ArgsTuple const& args ) {
			_assertNotRunning<C>();
			handleFlags<Arr::str[Arr::info.read + indices]...>::template evaluate<IndexConsistency::NotRunning, Arr::info.index-1>(string, args);
		}
	};
	template <typename Arr, STD::size_t... indices>
	struct handleArgIndex<true, Arr, STD::index_sequence<indices...>>
		: handleFormatSpecifier<Tokens::universal> {

		template <IndexConsistency C, STD::size_t, typename StringType, typename ArgsTuple>
		constexpr void evaluate( StringType& string, ArgsTuple const& args ) {
			_assertNotRunning<C>();
			handleFormatSpecifier<Tokens::universal>::evaluate(string, STD::get<Arr::info.index-1>(args) );
		}
	};

	template <typename, CharT... ch>
	struct handleArgIndexOpt : handleFlags<ch...> {
		template <IndexConsistency C, STD::size_t I, typename StringType, typename ArgsTuple>
		constexpr void evaluate( StringType& string, ArgsTuple const& args ) {
			_assertRunning<C>();
			handleFlags<ch...>::template evaluate<IndexConsistency::Running, I>(string, args);
		}
	};
	template <CharT... ch>
	struct handleArgIndexOpt<STD::enable_if_t<genIndexInfo<ch...>::info.read != 0>, ch...>
		: handleArgIndex<genIndexInfo<ch...>::info.isPercentSignTerm, genIndexInfo<ch...>> {};

	template <typename P>
	static constexpr P findFirstFormatSpec ( P str )
	{
		while (*str && (*str != Tokens::percent || *++str == Tokens::percent) )
			++str;
		return str;
	}

	template <CharT... ch>
	struct parse
	{
	private:
		template <CharT...>
		friend struct parse;

		static constexpr const CharT str[] {ch..., '\0'};
		static constexpr auto formatSpecPos = findFirstFormatSpec(str)-str;
		static_assert (formatSpecPos < sizeof str);

		template <IndexConsistency, STD::size_t, typename StringType, typename ArgsTuple>
		constexpr void call ( StringType& string, STD::index_sequence<>, ArgsTuple const&) {
			string.append(str, sizeof(str)-1);
		}

		template <IndexConsistency consistency, STD::size_t index, STD::size_t... indices, typename StringType, typename ArgsTuple>
		constexpr void call ( StringType& string, STD::index_sequence<indices...>, ArgsTuple const& args )
		{
			string.append(str, formatSpecPos-1);
			using handle_type = handleArgIndexOpt<void, str[formatSpecPos+indices]...>;
			using next_parser_type = typename handle_type::type;
			handle_type {} .template evaluate <consistency, index> (string, args);
			next_parser_type {}.template call<consistency, index + handle_type::indexLeap>(string, args);
		}

		template <IndexConsistency C, STD::size_t I, typename StringType, typename ArgsTuple>
		constexpr void call (StringType& string, ArgsTuple const& args) {
			call<C, I>(string, STD::make_index_sequence<sizeof(str)-1-formatSpecPos> {}, args);
		}

	public:
		template <typename StringType, typename... Args>
		constexpr StringType evaluate( Args&& ... args ) {
			StringType s;
			call<IndexConsistency::NoCommitment, 0>( s, STD::forward_as_tuple(STD::forward<Args>(args)...) );
			return s;
		}
		template <typename... Args>
		constexpr auto operator()( Args&& ... args ) {
			return evaluate<BasicString<CharT, 256>>(STD::forward<Args>(args)...);
		}
	};

}; // end template Parser

template <typename CharT, typename Tokens, typename Traits>
template <CharT... c>
constexpr CharT Parser<CharT, Tokens, Traits>::parse<c...>::str[];

} // end namespace detail

/**< Needs string literal operator templates. Should be introduced with C++17; According to Richard Smith,
 "EWG feedback was that they didn't want to accept the core language feature without some library support.
  Since then, two papers have proposed such support […] At this point, it looks likely that
  one of these will be accepted in C++17; […]" */
CONSTAINER_DIAGNOSTIC_PUSH
#ifdef __clang__
CONSTAINER_DIAGNOSTIC_IGNORE("-Wgnu-string-literal-operator-template")
#endif // __clang__
template <typename CharT, CharT... ch>
constexpr typename detail::Parser<CharT>::template parse<ch...> operator""_ConstainerStaticPrintf() {return {};}
CONSTAINER_DIAGNOSTIC_POP

template <typename Arg>
constexpr String toString(Arg const& arg) {
	return "%@"_ConstainerStaticPrintf(arg);
}

} // end namespace Constainer
