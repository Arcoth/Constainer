#include <iostream>

#include "Vector.hxx"

using namespace Constainer;

// Extremely marginal at the moment. Will be updated soon.

constexpr Array<int, 10> a {{1, 2, 3, 4, 5}};
constexpr Array<int, 10> a2{{1, 2, 3, 4, 5}};
static_assert( a == a2 );

constexpr Array<int, 10> a3{{1, 2}};
constexpr Array<int, 10> a4{{1, 2, 3}};
constexpr Array<int, 10> a5{{1, 2, 4}};
static_assert( a3 < a4 && a4 < a5 );

constexpr auto f()
{
	Vector<int, 100> vec(6, 7), vec2{4, 5, 10};
	// vec: 7 7 7 7 7 7
	vec.insert(vec.begin()+3, {1, 2, 3});
	// vec: 7 7 7 1 2 3 7 7 7
	vec.insert(vec.begin()+1, vec2.begin(), vec2.end()-1);
	// vec: 7 4 5 7 7 1 2 3 7 7 7
	vec.erase(vec.begin()+2, vec.begin()+4);
	// vec: 7 4 7 1 2 3 7 7 7
	vec2.swap(vec);
	vec = vec2;
	// vec: 7 4 7 1 2 3 7 7 7, the above is a noop wrt to vec
	vec.push_back(vec < vec2);
	// 7 4 7 1 2 3 7 7 7 0
	return vec;
}
static_assert( f() == Constainer::Vector<int, 20>{7, 4, 7, 1, 2, 3, 7, 7, 7, 0} );
