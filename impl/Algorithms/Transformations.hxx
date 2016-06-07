#pragma once

#include "../../Assert.hxx"

#include "Copy.hxx"
#include "NonModifying.hxx"

namespace Constainer {

/** A binder for operator==. */
template <typename T>
struct CompareFunctor {
	T const& t;
	template <typename U>
	constexpr bool operator()(U const& u) const {
		return t == u;
	}
};

namespace detail {
	template <typename T>
	constexpr void swap(T& a, T& b, ...) {
		T tmp = STD::move(a);
		a = STD::move(b);
		b = STD::move(tmp);
	}

	template <typename T>
	constexpr auto swap(T& a, T& b, int) -> decltype(a.swap(b)) {
		a.swap(b);
	}
}

template <typename T>
constexpr void swap(T& a, T& b) {detail::swap(a,b,0);}

template <typename It>
constexpr void iter_swap(It a, It b) {
	Constainer::swap(*a, *b); // Can be changed to iter_swap once that's constexpr.
}

template <typename BiDirIt>
constexpr void reverse(BiDirIt first, BiDirIt last) {
	while ((first != last) && (first != --last))
		Constainer::swap(*first++, *last);
}

template <typename OutputIt, typename T>
constexpr auto fill(OutputIt first, OutputIt last, T const& value) {
	while (first != last)
		*first++ = value;

	return first;
}
template <typename OutputIt, typename SizeType, typename T>
constexpr auto fill_n(OutputIt first, SizeType count, T const& value) {
	for (SizeType i = 0; i++ < count;)
		*first++ = value;

	return first;
}

template <typename Container, typename BiDirPosIt, typename BiDirValueIt>
constexpr void insert_ordered_at(Container& container, typename Container::size_type left, BiDirValueIt last_value,
                                 BiDirPosIt last_pos)
{
	constexpr typename Container::size_type empty_pos(-1);
	const auto orig_size = container.size();
	AssertExcept<STD::out_of_range>(container.size() < container.max_size() - typename Container::size_type(left),
	                                "Too many values to insert");
	container.resize(container.size() + left);
	auto last = container.begin() + orig_size;
	for (; left != 0; --left) {
		auto pos = *--last_pos;
		while (pos == empty_pos) {
			--last_value;
			pos = *--last_pos;
		}
		AssertExcept<STD::out_of_range>( pos <= orig_size, "Invalid position passed!");
		auto first = container.begin() + pos;
		Constainer::move_backward(first, last, last+left);
		first[left-1] = *--last_value;
		last = first;
	}
}


template <typename InputIt, typename OutputIt, typename UnaryOp>
constexpr OutputIt transform(InputIt first, InputIt last,
                             OutputIt out, UnaryOp op)
{
	while (first != last)
		*out++ = op(*first++);

	return out;
}
template <typename InputIt1, typename InputIt2,
          typename OutputIt, typename BinaryOperation>
constexpr OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                             OutputIt out, BinaryOperation op)
{
	while (first1 != last1)
		*out++ = op(*first1++, *first2++);

	return out;
}

template <typename ForwardIt, typename Generator>
constexpr void generate(ForwardIt first, ForwardIt last, Generator g)
{
	while (first != last)
		*first++ = g();
}
template <typename OutputIt, typename Diff, typename Generator>
constexpr OutputIt generate_n(OutputIt first, Diff n, Generator g)
{
	for (Diff i = 0; i < n; ++i)
		*first++ = g();

	return first;
}

template <typename ForwardIt, typename UnaryPred>
constexpr ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPred pred)
{
	first = Constainer::find_if(first, last, pred);
	if (first != last)
		for (ForwardIt i = first; ++i != last; )
			if (!pred(*i))
				*first++ = STD::move(*i);
	return first;
}
template <typename ForwardIt, typename T>
constexpr ForwardIt remove(ForwardIt first, ForwardIt last, T const& value){
	return Constainer::remove_if(first, last, CompareFunctor<T>{value});}


template <typename InputIt, typename OutputIt, typename UnaryPred>
constexpr OutputIt remove_copy_if(InputIt first, InputIt last,
                                  OutputIt out, UnaryPred pred)
{
	for (; first != last; ++first)
		if (!pred(*first))
			*out++ = *first;

	return out;
}
template <typename InputIt, typename OutputIt, typename T>
constexpr OutputIt remove_copy(InputIt first, InputIt last,
                               OutputIt out, T const& value) {
	return Constainer::remove_copy_if(first, last, out, CompareFunctor<T>{value});}


template <typename ForwardIt, typename UnaryPred, typename T>
constexpr void replace_if(ForwardIt first, ForwardIt last,
                UnaryPred pred, T const& to)
{
	for (; first != last; ++first)
		if (pred(*first))
			*first = to;
}
template <typename ForwardIt, typename T>
constexpr void replace(ForwardIt first, ForwardIt last, T const& from, T const& to) {
	return Constainer::replace(first, last, CompareFunctor<T>{from}, to);}


template <typename InputIt, typename OutputIt, typename UnaryPred, typename T>
constexpr OutputIt replace_copy_if(InputIt first, InputIt last, OutputIt out,
                                   UnaryPred pred, T const& val)
{
	for (; first != last; ++first)
		*out++ = p( *first ) ? val : *first;

	return out;
}

template <typename InputIt, typename OutputIt, typename T>
constexpr OutputIt replace_copy(InputIt first, InputIt last, OutputIt out,
                                T const& from, T const& to) {
	return Constainer::replace_copy_if(first, last, out, CompareFunctor<T>{from}, to);}

template <typename BidirIt, typename OutputIt>
constexpr OutputIt reverse_copy(BidirIt first, BidirIt last, OutputIt out)
{
	while (first != last)
		*out++ = *--last;

	return out;
}


template <typename ForwardIt>
constexpr void rotate(ForwardIt first, ForwardIt new_first, ForwardIt last)
{
	auto next = new_first;
	while (first != next) {
		Constainer::iter_swap(first++, next++);
		if (next == last)
			next = new_first;
		else if (first == new_first)
			new_first = next;
	}
}
template <typename ForwardIt, typename OutputIt>
constexpr OutputIt rotate_copy(ForwardIt first, ForwardIt new_first,
                               ForwardIt last, OutputIt out) {
    return Constainer::copy(first, new_first, Constainer::copy(new_first, last, out));}


template <typename ForwardIt, typename BinaryPred>
constexpr ForwardIt unique(ForwardIt first, ForwardIt last, BinaryPred pred)
{
	if (first == last)
		return last;

	auto result = first;
	while (++first != last)
		if (!pred(*result, *first) && ++result != first)
			*result = STD::move(*first);

	return ++result;
}
template <typename ForwardIt>
constexpr ForwardIt unique(ForwardIt first, ForwardIt last) {
	return Constainer::unique(first, last, STD::equal_to<>{});}

template <typename ForwardIt, typename OutputIt, typename BinaryPred>
constexpr ForwardIt unique_copy(ForwardIt first, ForwardIt last, OutputIt out, BinaryPred pred) {
	for (;;) {
		first = Constainer::adjacent_find(first, last, STD::not_equal_to<>{});
		if (first == last)
			break;
		*out++ = *first++;
	}

	return out;
}

}
