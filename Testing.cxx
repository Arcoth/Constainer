/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#include <initializer_list>
#include "RangeAccess.hxx"
#include "Parser.hxx"
#include "Vector.hxx"
#include "Bitset.hxx"
#include "ChunkPool.hxx"
#include "StableVector.hxx"
#include "FlatSet.hxx"
#include "FlatMap.hxx"
#include "StaticPrintf.hxx"

// Check ADL range access
static_assert( STD::is_same<decltype(begin(Constainer::String())), Constainer::String::iterator>{} );
static_assert( *Constainer::next(Constainer::begin(STD::initializer_list<int>{1, 2, 3}), 1) == 2 );

using namespace Constainer;

constexpr Array<int, 10> a {{1, 2, 3, 4, 5}};
constexpr Array<int, 10> a2{{1, 2, 3, 4, 5}};
static_assert( a == a2 );

constexpr Array<int, 10> a3{{1, 2}};
constexpr Array<int, 10> a4{{1, 2, 3}};
constexpr Array<int, 10> a5{{1, 2, 4}};
static_assert( a3 < a4 && a4 < a5 );

constexpr auto d() {
	ChunkPool<int, 17> c;
	int* ptr = c.grab();
	assert( c.used() == 1 );
	c.free(ptr);
	return 0;
}
static_assert( d() == 0 );

constexpr auto e() {
	Bitset<55> b;
	b.set(45);
	assert(b.test(45) && b.count() == 1 && b.any() && b.leading(0) == 45);
	b.flip(45);
	assert(b.count() == 0 && b.none() && b.leading(0) == 55);
	assert(b.flip().count() == 55 && b.all() && b.leading(0) == 0 && b.leading(1) == 55);
	assert(b.reset(7).reset(54).count() == 53 && b.leading(1) == 7);
	b[1] = ~b[10].flip();
	assert(b[1] == true && b[10] == false);
	return 0;
}
static_assert( e() == 0 );

constexpr auto f()
{
	Vector<int, 100> vec(6, 7);
	Vector<int, 50> vec2{4, 5, 10};
	// vec: 7 7 7 7 7 7
	vec.insert(vec.begin()+3, {1, 2, 3});
	// vec: 7 7 7 1 2 3 7 7 7
	vec.insert(begin(vec)+1, begin(vec2), end(vec2)-1);
	// vec: 7 4 5 7 7 1 2 3 7 7 7
	vec.erase(begin(vec)+2, begin(vec)+4);
	// vec: 7 4 7 1 2 3 7 7 7
	swap(vec, vec2);
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
static_assert( f() == Vector<int, 20>{7, 4, 7, 1, 2, 3, 7, 7, 7, 0, 5} );

constexpr auto g() {
	Vector <int, 10> v(7);
	iota(rbegin(v), rend(v), 0);
	// 6 5 4 3 2 1 0
	v.insert( v.begin()+2, {101, 102, 103} );
	// 6 5 101 102 103 4 3 2 1 0
	return v;
}
static_assert( g() == Vector<int, 20>{6, 5, 101, 102, 103, 4, 3, 2, 1, 0} );

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
	assert(t.find('*') == 4);
	assert(t.rfind('*') == 8);
	t = t.substr(2);
	//! "3 4 * * * * * x x x r  l  d  ! "
	//!  0 1 2 3 4 5 6 7 8 9 10 11 12 13

	assert(t.rfind("***", 6) == 4);
	assert(t.rfind("***", 5) == 3);
	assert(t.rfind("***", 3) == String::npos);

	assert(t.find("") == 0);
	assert(t.rfind("*", 6) == 6);

	assert(t.find_first_of("-.,") == String::npos);
	assert(t.find_first_of("x*4") == 1);
	assert(t.find_last_of("x*4") == 9);
	assert(t.find_first_not_of("34*") == 7);
	assert(t.find_last_not_of("34*") == 13);

	return t;
}
static_assert( h() == "34*****xxxrld!" );

constexpr auto i() {
	StableVector<int, 64> s{1, 2, 3, 5};
	s.insert(s.end()-1, 4);
	// 1 2 3 4 5
	auto it = s.end()-1;
	s.erase(s.begin()+1, s.begin()+3);
	// 1 4 5
	s.push_back(*it);
	// 1 4 5 5
	int i=0;
	for (auto& j : makeIteratorRange(rbegin(s), rend(s)))
		j += ++i;
	// 5, 7, 7, 6
	return s;
}
static_assert( i() == StableVector<int, 50>{5, 7, 7, 6} );

constexpr auto j() {
	StableFlatMap<int, char, 64, STD::greater<>> map(ordered_unique_range, {{3, 'a'}, {2, 'b'}});
	assert( map.insert_or_assign(5, 'd').second );
	map[2] = 'e';
	map.insert(ordered_unique_range, {{4, 'd'}, {3, 'w'}, {1,'c'}});
	return map;
}
static_assert( j() == StableFlatMap<int, char, 10, STD::greater<>>{{5, 'd'}, {4, 'd'}, {3, 'a'}, {2, 'e'}, {1, 'c'}} );

constexpr auto k() {
	StableFlatMultiMap<int, char, 64, STD::greater<>> map(ordered_unique_range, {{3, 'c'}, {2, 'b'}});

	auto i = map.begin()+1, j = map.end();

	map.insert({{2, 'c'}, {3, 'd'}});
	auto range = map.equal_range(2);
	assert(range.first == i && range.second == j);

	return map;
}
static_assert( k() == StableFlatMultiMap<int, char, 10, STD::greater<>>{{3, 'c'}, {3, 'd'}, {2, 'b'}, {2, 'c'}} );


static_assert( strToInt<int>(" 6849.") == 6849 );
static_assert( strToInt<signed char>(" -128aefws") == -128 );
static_assert( strToInt<unsigned>(" \t-0") == 0 );
static_assert( strToInt<unsigned>(" -0x0Xx", 0, 0) == 0 );
static_assert( strToInt<unsigned>(" +0xFF", 0, 0) == 0xFF );
static_assert( strToInt<unsigned>(" +077", 0, 0) == 7+8*7 );
static_assert( strToInt<unsigned>("11000", 0, 2) == 24 );

static_assert( safeMul<double>(-0., 0) == 0 );
static_assert( safeMul<double>(-STD::numeric_limits<double>::infinity(),  0) == -STD::numeric_limits<double>::infinity() );
static_assert( safeMul<double>(-STD::numeric_limits<double>::infinity(), -1) == STD::numeric_limits<double>::infinity() );

/**< These should go well on most implementations. */
static_assert( strToFloat<double>("+123.456789e0") == 123.456789 );
static_assert( strToFloat<double>("+123.456789e0") == 123.456789 );
static_assert( strToFloat<double>("-0x1.Bc70a3D70A3d7p+6") == -111.11 );
static_assert( strToFloat<double     >("-1.18973e+4932") == -STD::numeric_limits<double>::infinity() );
static_assert( strToFloat<long double>("-1.18973e+4932") != -STD::numeric_limits<long double>::infinity() );
static_assert( strToFloat<double>("-0x.8p-1") == -0.25 );

static_assert( abs(STD::numeric_limits<double>::infinity())  == STD::numeric_limits<double>::infinity() );
static_assert( abs(-STD::numeric_limits<double>::infinity()) == STD::numeric_limits<double>::infinity() );

// Taken from http://osxr.org/glibc/source/stdio-common/tst-printf.c#0373
namespace TC001
{
	constexpr double g = 5.0000001;
	constexpr double d = 321.7654321;
	const int i = 12345;
	const int h = 1234;
	const unsigned long l = 1234567890;
	constexpr char s[] = "test-string";

	static_assert( "%1$*5$d %2$*6$i %3$*7$o %4$*8$f %9$*12$e %10$*13$g %11$*14$s"_ConstainerStaticPrintf (i, h, l, d, 8, 5, 14, 14, d, g, s, 14, 3, 14)
	            == "   12345  1234    11145401322     321.765432   3.217654e+02   5    test-string" );
}

static_assert( 0.123456789456 == strToFloat<double>("%a\n"_ConstainerStaticPrintf(0.123456789456).c_str()) );

