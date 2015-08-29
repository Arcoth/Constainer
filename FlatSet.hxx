/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "FlatUniqueTree.hxx"
#include "FlatMultiTree.hxx"
#include "StableVector.hxx"

namespace Constainer {

template <typename Key, typename Compare, typename Container>
using BasicFlatSet = detail::FlatUniqueTree<Key, Key, IdentityFunctor, Compare, Container>;

template <typename T, std::size_t MaxN=defaultContainerSize, typename Compare = std::less<T>>
using FlatSet = BasicFlatSet<T, Compare, Constainer::Vector<T, MaxN>>;
template <typename T, std::size_t MaxN=defaultContainerSize, typename Compare = std::less<T>>
using StableFlatSet = BasicFlatSet<T, Compare, Constainer::StableVector<T, MaxN>>;

template <typename Key, typename Compare, typename Container>
using BasicFlatMultiSet = detail::FlatMultiTree<Key, Key, IdentityFunctor, Compare, Container>;

template <typename T, std::size_t MaxN=defaultContainerSize, typename Compare = std::less<T>>
using FlatMultiSet       = BasicFlatMultiSet<T, Compare, Constainer::Vector<T, MaxN>>;
template <typename T, std::size_t MaxN=defaultContainerSize, typename Compare = std::less<T>>
using StableFlatMultiSet = BasicFlatMultiSet<T, Compare, Constainer::StableVector<T, MaxN>>;

}
