#ifndef TRAITS_HXX_INCLUDED
#define TRAITS_HXX_INCLUDED

#include <iterator>
#include <type_traits>

namespace Constainer {
	template <typename...>
	using void_t = void;

	template <typename T, typename R=void>
	using requires = std::enable_if_t<T{}, R>;

	/*! The following traits depend on the resolution of LWG issue #2408 which is not included in C++14 but already resolved in
	    libc++ and libstdc++ */

	namespace detail {
		template <typename, typename, typename=void>
		struct hasIterCategoryConvTo : std::false_type {};
		template <typename I, typename C>
		struct hasIterCategoryConvTo<I, C,
			std::enable_if_t<std::is_convertible<
				typename std::iterator_traits<I>::iterator_category, C
			>{}>>
			: std::true_type {};
	}

	template <typename I>
	using         isInputIterator = detail::hasIterCategoryConvTo<I, std::        input_iterator_tag>;
	template <typename I>
	using       isForwardIterator = detail::hasIterCategoryConvTo<I, std::      forward_iterator_tag>;
	template <typename I>
	using        isOutputIterator = detail::hasIterCategoryConvTo<I, std::       output_iterator_tag>;
	template <typename I>
	using  isRandomAccessIterator = detail::hasIterCategoryConvTo<I, std::random_access_iterator_tag>;
	template <typename I>
	using isBidirectionalIterator = detail::hasIterCategoryConvTo<I, std::bidirectional_iterator_tag>;

	/*template <typename I>
	using isContiguousIterator = detail::hasIterCategoryConvTo<I, std::contiguous_iterator_tag>;*/
}

#endif // TRAITS_HXX_INCLUDED
