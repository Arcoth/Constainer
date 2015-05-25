#include <iostream>

#include "Vector.hxx"
#include "String.hxx"

using namespace Constainer;

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
	vec.resize(12, 5);
	// 7 4 7 1 2 3 7 7 7 0 5 5
	vec.resize(11);
	// 7 4 7 1 2 3 7 7 7 0 5
	return vec;
}
static_assert( f() == Constainer::Vector<int, 20>{7, 4, 7, 1, 2, 3, 7, 7, 7, 0, 5} );


constexpr auto g() {
	Vector <int, 10> v(7);
	iota(rbegin(v), rend(v), 0);
	// 6 5 4 3 2 1 0
	v.insert( v.begin()+2, {101, 102, 103} );
	// 6 5 101 102 103 4 3 2 1 0
	return v;
}
static_assert( g() == Constainer::Vector<int, 20>{6, 5, 101, 102, 103, 4, 3, 2, 1, 0} );

constexpr auto h() {
	auto t = "Hello " + String("World") + '!';
	// "Hello World!"
	t.erase(1, 2);
	// "Hlo World!"
	t.replace(4, 2, "xxx");
	// "Hlo xxxrld!"
	t.replace(begin(t), begin(t)+3, {'4', '5'});
	// "45 xxxrld!"
	t.replace(begin(t)+1, begin(t)+3, 5, '*');
	// "4*****xxxrld!"
	t.insert(0, "123");
	// "1234*****xxxrld!"
	Assert(t.find('*') == 4);
	Assert(t.rfind('*') == 8);
	t = t.substr(2);
	//! "3 4 * * * * * x x x r  l  d  ! "
	//!  0 1 2 3 4 5 6 7 8 9 10 11 12 13

	Assert(t.rfind("***", 6) == 4);
	Assert(t.rfind("***", 5) == 3);
	Assert(t.rfind("***", 3) == String::npos);

	Assert(t.find("") == 0);
	Assert(t.rfind("", 5) == 5);

	Assert(t.find_first_of("-.,") == String::npos);
	Assert(t.find_first_of("x*4") == 1);
	Assert(t.find_last_of("x*4") == 9);
	Assert(t.find_first_not_of("34*") == 7);
	Assert(t.find_last_not_of("34*") == 13);

	return t;
}
static_assert( h() == "34*****xxxrld!" );


static_assert( Constainer::sToInt<int>(String(" 684")) == 684 );
static_assert( Constainer::sToInt<char>(String(" -128")) == -128 );
static_assert( Constainer::sToInt<unsigned>(String(" \t-0")) == 0 );
static_assert( Constainer::sToInt<unsigned>(String(" -0x0"), 0, 0) == 0 );
static_assert( Constainer::sToInt<unsigned>(String(" +0xFF"), 0, 0) == 0xFF );
static_assert( Constainer::sToInt<unsigned>(String(" +077"), 0, 0) == 7+8*7 );
static_assert( Constainer::sToInt<unsigned>(String("11000"), 0, 2) == 24 );
