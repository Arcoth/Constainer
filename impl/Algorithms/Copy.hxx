#pragma once

#include "../Iterator/MoveIterator.hxx"

namespace Constainer {

template <typename InputIt, typename OutputIt>
constexpr auto copy(InputIt first, InputIt last, OutputIt out) {
	while (first != last)
		*out++ = *first++;

	return out;
}
template <typename InputIt, typename SizeType, typename OutputIt>
constexpr auto copy_n(InputIt first, SizeType count, OutputIt out) {
	while (count-- != 0)
		*out++ = *first++;

	return out;
}

template <typename BiDir, typename BiDir2>
constexpr auto copy_backward(BiDir  first, BiDir last, BiDir2 last2) {
	while (last != first)
		*--last2 = *--last;

	return last2;
}
template <typename BiDir, typename BiDir2>
constexpr auto move_backward(BiDir first, BiDir last, BiDir2 last2) {
	return Constainer::copy_backward(make_move_iterator(first), make_move_iterator(last), last2);
}

template <typename InputIt, typename OutputIt>
constexpr auto move(InputIt first, InputIt last, OutputIt out) {
	return Constainer::copy(make_move_iterator(first), make_move_iterator(last), out);
}
template <typename InputIt, typename SizeType, typename OutputIt>
constexpr auto move_n(InputIt first, SizeType count, OutputIt out) {
	return Constainer::copy_n(make_move_iterator(first), count, out);
}

}
