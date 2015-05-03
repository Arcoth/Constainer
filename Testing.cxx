#include "Array.hxx"

using namespace Constainer;

// Extremely marginal at the moment. Will be updated soon.

constexpr Array<int, 10> a {{1, 2, 3, 4, 5}};
constexpr Array<int, 10> a2{{1, 2, 3, 4, 5}};
static_assert( a == a2 );

constexpr Array<int, 10> a3{{1, 2}};
constexpr Array<int, 10> a4{{1, 2, 3}};
constexpr Array<int, 10> a5{{1, 2, 4}};
static_assert( a3 < a4 && a4 < a5 );
