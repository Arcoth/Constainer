#include "TransformIterator.hxx"

namespace Constainer {

struct indirect_iterator_extractor {
	template <typename Iter>
	constexpr decltype(auto) operator()(Iter i) {
		return *i;
	}
};

template <typename Iter>
using indirect_iterator = transform_iterator<indirect_iterator_extractor, Iter>;

}
