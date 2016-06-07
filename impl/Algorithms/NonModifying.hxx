#pragma once

namespace Constainer {

template <typename CharT>
constexpr STD::size_t strlen(CharT const* s) {
	STD::size_t l = 0;
	while (*s) {
		++s; ++l;
	}
	return l;
}

template <typename InputIterator, typename T, typename Comp>
constexpr auto find(InputIterator first, InputIterator last, T const& val, Comp comp) {
	while (first != last && !comp(*first, val))
		++first;
	return first;
}
template <typename InputIterator, typename T>
constexpr auto find(InputIterator first, InputIterator last, T const& val) {
	return Constainer::find(first, last, val, STD::equal_to<>{});
}

template <typename InputIterator, typename Pred>
constexpr auto find_if(InputIterator first, InputIterator last, Pred p) {
	while (first != last && !p(*first))
		++first;
	return first;
}

template <typename InputIterator, typename Pred>
constexpr auto find_if_not(InputIterator first, InputIterator last, Pred p) {
	while (first != last && p(*first))
		++first;
	return first;
}

template <typename InputIt, typename UnaryPred>
constexpr bool all_of(InputIt first, InputIt last, UnaryPred pred) {
	return Constainer::find_if_not(first, last, pred) == last;}

template <typename InputIt, typename UnaryPred>
constexpr bool any_of(InputIt first, InputIt last, UnaryPred pred) {
	return Constainer::find_if(first, last, pred) != last;}

template <typename InputIt, typename UnaryPred>
constexpr bool none_of(InputIt first, InputIt last, UnaryPred pred) {
	return Constainer::find_if(first, last, pred) == last;}


template <typename InputIt, typename UnaryOp>
constexpr UnaryOp for_each(InputIt first, InputIt last, UnaryOp op) {
	while (first != last)
		op(*first++);

	return op;
}
template <typename InputIt, typename Diff, typename UnaryOp>
constexpr UnaryOp for_each_n(InputIt first, Diff n, UnaryOp op) {
	while (n-- != 0)
		op(*first++);

	return op;
}

template <typename ForwardIt, typename BinaryPred>
constexpr ForwardIt adjacent_find(ForwardIt first, ForwardIt last, BinaryPred pred)
{
	if (first == last)
		return last;

	auto next = first;
	for (; ++next != last; ++first)
		if (pred(*first, *next))
			return first;

	return last;
}
template <typename ForwardIt, typename BinaryPred>
constexpr ForwardIt adjacent_find(ForwardIt first, ForwardIt last) {
	return Constainer::adjacent_find(first, last, STD::equal_to<>{});}

template <typename ForwardIt, typename Diff, typename T, typename BinaryPred>
constexpr ForwardIt search_n(ForwardIt first, ForwardIt last,
                             Diff n, T const& value, BinaryPred pred)
{
	for(; first != last; ++first)
		if (pred(*first, value)) {
			auto begin = first;
			Diff m = 0;
			do {
				if (++m == n)
					return begin;
				if (++first == last)
					return last;
			} while(pred(*first, value));
		}

	return last;
}
template <typename ForwardIt, typename Diff, typename T, typename BinaryPred>
constexpr ForwardIt search_n(ForwardIt first, ForwardIt last,
                             Diff n, T const& value) {
	return Constainer::search_n(first, last, n, value, STD::equal_to<>{});}

template <typename InputIt, typename T>
constexpr typename STD::iterator_traits<InputIt>::difference_type count(InputIt first, InputIt last, T const& val)
{
	typename STD::iterator_traits<InputIt>::difference_type ret = 0;
	while (first != last)
		if (*first++ == val)
			++ret;
	return ret;
}
template <typename InputIt, typename T>
constexpr typename STD::iterator_traits<InputIt>::difference_type count_not(InputIt first, InputIt last, T const& val)
{
	typename STD::iterator_traits<InputIt>::difference_type ret = 0;
	while (first != last)
		if (*first++ != val)
			++ret;
	return ret;
}
template <typename InputIt, typename UnaryPredicate>
constexpr typename STD::iterator_traits<InputIt>::difference_type count_if(InputIt first, InputIt last, UnaryPredicate p)
{
	typename STD::iterator_traits<InputIt>::difference_type ret = 0;
	while (first != last)
		if (p(*first++))
			ret++;
	return ret;
}



template <typename InputIt, typename ForwardIt, typename Comparator>
constexpr InputIt find_first_of(InputIt first, InputIt last,
                                ForwardIt sfirst, ForwardIt slast,
                                Comparator comp) {
	for (; first != last; ++first)
		for (auto it = sfirst; it != slast; ++it)
			if (comp(*first, *it))
				return first;

	return last;
}
template <typename InputIt, typename ForwardIt>
constexpr InputIt find_first_of(InputIt first, InputIt last,
                                ForwardIt sfirst, ForwardIt slast) {
	return find_first_of(first, last, sfirst, slast, STD::equal_to<>{});}

template <typename InputIt, typename ForwardIt, typename Comparator>
constexpr InputIt find_first_not_of(InputIt first, InputIt last,
                                    ForwardIt s_first, ForwardIt s_last,
                                    Comparator comp) {
	for (; first != last; ++first) {
		auto it = s_first;
		for (; it != s_last; ++it)
			if (comp(*first, *it))
				break;
		if (it == s_last)
			return first;
	}
	return last;
}
template <typename InputIt, typename ForwardIt>
constexpr InputIt find_first_not_of(InputIt f, InputIt l,
                                    ForwardIt sf, ForwardIt sl) {
	return Constainer::find_first_not_of(f, l, sf, sl, STD::equal_to<>{});}

template <typename ForwardIt, typename T, typename Comparator>
constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, T const& value, Comparator comp) {
	auto count = Constainer::distance(first, last);
	while (count > 0) {
		auto intervall_size = count / 2;
		auto it = Constainer::next(first, intervall_size);
		if (comp(*it, value)) {
			first = ++it;
			count -= intervall_size + 1;
		}
		else
			count = intervall_size;
	}
	return first;
}
template <typename ForwardIt, typename T>
constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, T const& value) {
	return Constainer::lower_bound(first, last, value, STD::less<>{});}
template<typename ForwardIt, typename T, typename Comparator>
constexpr ForwardIt upper_bound(ForwardIt first, ForwardIt last, T const& value, Comparator comp) {
	return Constainer::lower_bound(first, last, value, makeNegateFunctor(comp));
}
template<typename ForwardIt, typename T>
constexpr ForwardIt upper_bound(ForwardIt first, ForwardIt last, T const& value) {
	return Constainer::upper_bound(first, last, value, STD::less<>{});}

template <typename ForwardIt, typename T, typename Comparator>
constexpr ForwardIt binary_search(ForwardIt first, ForwardIt last, T const& value, Comparator comp) {
	first = Constainer::lower_bound(first, last, value, comp);
	return first != last && !comp(value, *first)? first : last;
}
template <typename ForwardIt, typename T>
constexpr ForwardIt binary_search(ForwardIt first, ForwardIt last, T const& value) {
	return Constainer::binary_search(first, last, value, STD::less<>{});}

template<class ForwardIt, class T, class Compare>
constexpr auto equal_range(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
	return STD::make_pair(Constainer::lower_bound(first, last, value, comp),
	                      Constainer::upper_bound(first, last, value, comp));
}
template<class ForwardIt, class T, class Compare>
constexpr auto equal_range(ForwardIt first, ForwardIt last, const T& value) {
	return Constainer::equal_range(first, last, value, STD::less<>{});}

}
