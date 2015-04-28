#ifndef ASSERT_HXX_INCLUDED
#define ASSERT_HXX_INCLUDED

#include <stdexcept>

#include <cstdlib>

namespace Constainer {

// In the abscence of a better name...
template <typename Except=std::logic_error, typename... Args>
constexpr void AssertExcept(bool b, Args&&... args) {
	if (!b) throw Except(std::forward<Args>(args)...);
}

constexpr void Assert(bool b) {
	if (!b) std::abort();
}

}

#endif // ASSERT_HXX_INCLUDED
