/* Copyright 2015, 2016 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "impl/Fundamental.hxx"
#include "FlatUniqueTree.hxx"
#include "FlatMultiTree.hxx"
#include "StableVector.hxx"

namespace Constainer {
	template <typename Key, typename Value>
	class ConstKeyPair : public TotallyOrdered<ConstKeyPair<Key, Value>> {
		using _base = STD::tuple<Key, Value>;

		Key _key; Value _value;

		template <typename, typename>
		friend class ConstKeyPair;

	public:

		struct KeyOfPair {
			template <typename T>
			constexpr decltype(auto) operator()(T&& t) const {return STD::forward<T>(t).key();}
		};

		using key_type = Key;
		using value_type = Value;

		template <typename T, typename U>
		constexpr ConstKeyPair(key_type const& k, value_type const& v)
			: _key(k), _value(v) {}

		template <typename T, typename U>
		constexpr ConstKeyPair(T&& t, U&& u)
			: _key(STD::forward<T>(t)), _value(STD::forward<U>(u)) {}

		template <typename T, typename U>
		constexpr ConstKeyPair(ConstKeyPair<T, U> const& other)
			: ConstKeyPair(other.key(), other.value()) {}
		template <typename T, typename U>
		constexpr ConstKeyPair(ConstKeyPair<T, U> && other)
			: _key(STD::move(other)._key), _value(STD::move(other)._value) {}

private:
		template <typename... Args1, typename... Args2, STD::size_t... Indices1, STD::size_t... Indices2>
		constexpr ConstKeyPair( STD::tuple<Args1...> const& first_args,
		                        STD::tuple<Args2...> const& second_args,
		                        STD::index_sequence<Indices1...>, STD::index_sequence<Indices2...> )
			: _key(STD::get<Indices1>(first_args)...), _value(STD::get<Indices2>(second_args)...)
		{}

public:
		template <typename... Args1, typename... Args2>
		constexpr ConstKeyPair( STD::piecewise_construct_t,
		                        STD::tuple<Args1...> first_args,
		                        STD::tuple<Args2...> second_args )
			: ConstKeyPair(first_args, second_args,
			               STD::index_sequence_for<Args1...>{}, STD::index_sequence_for<Args2...>{})
		{}


		constexpr ConstKeyPair() = default;
		constexpr ConstKeyPair& operator=(ConstKeyPair const& ) = default;
		constexpr ConstKeyPair& operator=(ConstKeyPair && other) = default;
		constexpr ConstKeyPair(ConstKeyPair const& ) = default;
		constexpr ConstKeyPair(ConstKeyPair && ) = default;

		template <typename K, typename V>
		constexpr ConstKeyPair& operator=(ConstKeyPair<K, V> const& other) {
			STD::get<0>(*this) = other.key();
			STD::get<1>(*this) = other.value();
			return *this;
		}
		template <typename K, typename V>
		constexpr ConstKeyPair& operator=(ConstKeyPair<K, V> && other) {
			STD::get<0>(*this) = STD::get<0>(STD::move(other));
			STD::get<1>(*this) = STD::get<1>(STD::move(other));
			return *this;
		}

		constexpr key_type   const&   key() const& {return _key;}
		constexpr value_type      & value()      & {return _value;}
		constexpr value_type const& value() const& {return _value;}
		constexpr key_type   const&&   key() const&& {return STD::move(_key);}
		constexpr value_type      && value()      && {return STD::move(_value);}
		constexpr value_type const&& value() const&& {return STD::move(_value);}

		template <typename K, typename V>
		friend constexpr bool operator==(ConstKeyPair<K, V> const& lhs, ConstKeyPair<K, V> const& rhs) {
			return lhs.key() == rhs.key() && lhs.value() == rhs.value();
		}
		template <typename K, typename V>
		friend constexpr bool operator <(ConstKeyPair<K, V> const& lhs, ConstKeyPair<K, V> const& rhs) {
			return lhs.key()<rhs.key() || lhs.value()<rhs.value();
		}
	};
}

CONSTAINER_DIAGNOSTIC_PUSH
CONSTAINER_DIAGNOSTIC_IGNORE("-Wmismatched-tags")
namespace std {
	template <typename K, typename V>
	struct tuple_size<Constainer::ConstKeyPair<K, V>> : Constainer::STD::integral_constant<Constainer::STD::size_t, 2> {};

	template <typename K, typename V>
	struct tuple_element<0, Constainer::ConstKeyPair<K, V>> {using type = K;};

	template <typename K, typename V>
	struct tuple_element<1, Constainer::ConstKeyPair<K, V>> {using type = V;};
}
CONSTAINER_DIAGNOSTIC_POP

namespace Constainer {

namespace detail {
template <typename Key, typename Mapped, typename Compare, typename Container,
          template <typename...> class Base>
class BasicFlatMap
	: public      Base<Key, ConstKeyPair<Key, Mapped>,
	                   typename ConstKeyPair<Key, Mapped>::KeyOfPair, Compare, Container> {
	using _base = Base<Key, ConstKeyPair<Key, Mapped>,
	                   typename ConstKeyPair<Key, Mapped>::KeyOfPair, Compare, Container>;

public:
	using _base::_base;

	using mapped_type = Mapped;

	// For convenience inside this template definition
	using typename _base::key_type;
	using typename _base::value_type;
	using typename _base::reference;
	using typename _base::const_reference;
	using typename _base::iterator;
	using typename _base::const_iterator;

private:
	template <typename K>
	constexpr mapped_type& _subscript_op(K&& key) {
		auto pos = this->lower_bound(key);
		if (pos != this->end() and not this->_key_comp()(key, pos->key()))
			return pos->value();
		return this->emplace_hint(pos, STD::forward<K>(key), mapped_type())->value();
	}

public:
	constexpr mapped_type& operator[](key_type const& key) {return _subscript_op(          key );}
	constexpr mapped_type& operator[](key_type     && key) {return _subscript_op(STD::move(key));}

	constexpr       Mapped& at(key_type const& key) {
		auto pos = this->find(key);
		AssertExcept<STD::out_of_range>(pos != this->end(), "");
		return pos->value();
	}
	constexpr Mapped const& at(key_type const& key) const {
		auto pos = this->find(key);
		AssertExcept<STD::out_of_range>(pos != this->cend(), "");
		return pos->value();
	}

private:
	template <typename K, typename M>
	constexpr STD::pair<iterator, bool> _insert_or_assign(K&& key, M&& obj) {
		auto pos = this->lower_bound(key);
		if (pos != this->end() and not this->_key_comp()(key, pos->key())) {
			pos->value() = STD::forward<M>(obj);
			return {pos, false};
		}
		return {this->emplace_hint(pos, STD::forward<K>(key), STD::forward<M>(obj)), true};
	}
	template <typename K, typename M>
	constexpr iterator _insert_or_assign(const_iterator hint, K&& key, M&& obj) {
		auto pos = this->lower_bound(hint, key);
		if (pos != this->end() and not this->_key_comp()(key, pos->key())) {
			pos->value() = STD::forward<M>(obj);
			return pos;
		}
		return this->emplace_hint(pos, STD::forward<K>(key), STD::forward<M>(obj));
	}

public:
	template <typename M>
	constexpr STD::pair<iterator, bool> insert_or_assign(key_type const& key, M&& obj) {
		return _insert_or_assign(key, STD::forward<M>(obj));
	}
	template <typename M>
	constexpr STD::pair<iterator, bool> insert_or_assign(key_type && key, M&& obj) {
		return _insert_or_assign(STD::move(key), STD::forward<M>(obj));
	}
	template <typename M>
	constexpr STD::pair<iterator, bool> insert_or_assign(const_iterator hint, key_type const& key, M&& obj) {
		return _insert_or_assign(hint, key, STD::forward<M>(obj));
	}
	template <typename M>
	constexpr STD::pair<iterator, bool> insert_or_assign(const_iterator hint, key_type && key, M&& obj) {
		return _insert_or_assign(hint, STD::move(key), STD::forward<M>(obj));
	}

	template <typename... Args>
	constexpr STD::pair<iterator, bool> try_emplace(key_type const& key, Args&&... args) {
		mapped_type v(STD::forward<Args>(args)...);
		return insert_or_assign(key, STD::move(v));
	}
	template <typename... Args>
	constexpr STD::pair<iterator, bool> try_emplace(key_type && key, Args&&... args) {
		mapped_type v(STD::forward<Args>(args)...);
		return insert_or_assign(STD::move(key), STD::move(v));
	}
	template <typename... Args>
	constexpr iterator try_emplace(const_iterator hint, key_type const& key, Args&&... args) {
		mapped_type v(STD::forward<Args>(args)...);
		return insert_or_assign(hint, key, STD::move(v));
	}
	template <typename... Args>
	constexpr iterator try_emplace(const_iterator hint,key_type && key, Args&&... args) {
		mapped_type v(STD::forward<Args>(args)...);
		return insert_or_assign(hint, STD::move(key), STD::move(v));
	}


};
}

template <typename K, typename M, typename C, typename Cont>
using BasicFlatMap = detail::BasicFlatMap<K, M, C, Cont, detail::FlatUniqueTree>;
template <typename K, typename M, typename C, typename Cont>
using BasicFlatMultiMap = detail::BasicFlatMap<K, M, C, Cont, detail::FlatMultiTree>;

template <typename K, typename V, STD::size_t MaxN=defaultContainerSize, typename Compare = STD::less<K>>
using FlatMap = BasicFlatMap<K, V, Compare, Constainer::Vector<ConstKeyPair<K, V>, MaxN>>;
template <typename K, typename V, STD::size_t MaxN=defaultContainerSize, typename Compare = STD::less<K>>
using StableFlatMap = BasicFlatMap<K, V, Compare, Constainer::StableVector<ConstKeyPair<K, V>, MaxN>>;

template <typename K, typename V, STD::size_t MaxN=defaultContainerSize, typename Compare = STD::less<K>>
using FlatMultiMap = BasicFlatMultiMap<K, V, Compare, Constainer::Vector<ConstKeyPair<K, V>, MaxN>>;
template <typename K, typename V, STD::size_t MaxN=defaultContainerSize, typename Compare = STD::less<K>>
using StableFlatMultiMap = BasicFlatMultiMap<K, V, Compare, Constainer::StableVector<ConstKeyPair<K, V>, MaxN>>;

}
