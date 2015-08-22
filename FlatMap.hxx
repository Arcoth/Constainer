/* Copyright 2015 Robert Haberlach
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt) */

#pragma once

#include "UniqueFlatTree.hxx"
#include "StableVector.hxx"

namespace Constainer {
	template <typename Key, typename Value>
	class ConstKeyPair : public TotallyOrdered<ConstKeyPair<Key, Value>> {
		using _base = std::tuple<Key, Value>;

		Key _key; Value _value;

		template <typename, typename>
		friend class ConstKeyPair;

	public:

		struct KeyOfPair {
			template <typename T>
			constexpr decltype(auto) operator()(T&& t) const {return std::forward<T>(t).key();}
		};

		using key_type = Key;
		using value_type = Value;

		template <typename T, typename U>
		constexpr ConstKeyPair(key_type const& k, value_type const& v)
			: _key(k), _value(v) {}

		template <typename T, typename U>
		constexpr ConstKeyPair(T&& t, U&& u)
			: _key(std::forward<T>(t)), _value(std::forward<U>(u)) {}

		template <typename T, typename U>
		constexpr ConstKeyPair(ConstKeyPair<T, U> const& other)
			: ConstKeyPair(other.key(), other.value()) {}
		template <typename T, typename U>
		constexpr ConstKeyPair(ConstKeyPair<T, U> && other)
			: _key(std::move(other)._key), _value(std::move(other)._value) {}

private:
		template <typename... Args1, typename... Args2, std::size_t... Indices1, std::size_t... Indices2>
		constexpr ConstKeyPair( std::tuple<Args1...> const& first_args,
		                        std::tuple<Args2...> const& second_args,
		                        std::index_sequence<Indices1...>, std::index_sequence<Indices2...> )
			: _key(std::get<Indices1>(first_args)...), _value(std::get<Indices2>(second_args)...)
		{}

public:
		template <typename... Args1, typename... Args2>
		constexpr ConstKeyPair( std::piecewise_construct_t,
		                        std::tuple<Args1...> first_args,
		                        std::tuple<Args2...> second_args )
			: ConstKeyPair(first_args, second_args,
			               std::index_sequence_for<Args1...>{}, std::index_sequence_for<Args2...>{})
		{}


		constexpr ConstKeyPair() = default;
		constexpr ConstKeyPair& operator=(ConstKeyPair const& ) = default;
		constexpr ConstKeyPair& operator=(ConstKeyPair && other) = default;
		constexpr ConstKeyPair(ConstKeyPair const& ) = default;
		constexpr ConstKeyPair(ConstKeyPair && ) = default;

		template <typename K, typename V>
		constexpr ConstKeyPair& operator=(ConstKeyPair<K, V> const& other) {
			std::get<0>(*this) = other.key();
			std::get<1>(*this) = other.value();
			return *this;
		}
		template <typename K, typename V>
		constexpr ConstKeyPair& operator=(ConstKeyPair<K, V> && other) {
			std::get<0>(*this) = std::get<0>(std::move(other));
			std::get<1>(*this) = std::get<1>(std::move(other));
			return *this;
		}

		constexpr key_type   const&   key() const& {return _key;}
		constexpr value_type      & value()      & {return _value;}
		constexpr value_type const& value() const& {return _value;}
		constexpr key_type   const&&   key() const&& {return std::move(_key);}
		constexpr value_type      && value()      && {return std::move(_value);}
		constexpr value_type const&& value() const&& {return std::move(_value);}

		template <typename K, typename V>
		friend constexpr bool operator==(ConstKeyPair<K, V> const& lhs, ConstKeyPair<K, V> const& rhs) {
			return lhs.key() == rhs.key() && lhs.value() == rhs.value();
		}
		template <typename K, typename V>
		friend constexpr bool operator <(ConstKeyPair<K, V> const& lhs, ConstKeyPair<K, V> const& rhs) {
			return std::tie(lhs.key(), lhs.value()) < std::tie(rhs.key(), rhs.value());
		}
	};
}

namespace std {
	template <typename K, typename V>
	struct tuple_size<Constainer::ConstKeyPair<K, V>> : std::integral_constant<std::size_t, 2> {};

	template <typename K, typename V>
	struct tuple_element<0, Constainer::ConstKeyPair<K, V>> {using type = K;};

	template <typename K, typename V>
	struct tuple_element<1, Constainer::ConstKeyPair<K, V>> {using type = V;};
}

namespace Constainer {

template <typename Key, typename Mapped, typename Compare, typename Container>
class BasicFlatMap
	: public      detail::UniqueFlatTree<Key, ConstKeyPair<Key, Mapped>,
	                                     typename ConstKeyPair<Key, Mapped>::KeyOfPair, Compare, Container> {
	using _base = detail::UniqueFlatTree<Key, ConstKeyPair<Key, Mapped>,
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

	constexpr mapped_type& operator[](key_type const& key) {
		auto pos = this->lower_bound(key);
		if (pos != this->end() and not this->_key_comp()(key, pos->key()))
			return pos->value();
		return *this->emplace_hint(pos, key, mapped_type())->value();
	}
	constexpr mapped_type& operator[](key_type && key) {
		auto pos = this->lower_bound(key);
		if (pos != this->end() and not this->_key_comp()(key, pos->key()))
			return pos->value();
		return this->emplace_hint(pos, std::move(key), mapped_type())->value();
	}

	constexpr reference at(key_type const& key) {
		auto pos = this->find(key);
		AssertExcept<std::out_of_range>(pos != this->end(), "");
		return *pos;
	}
	constexpr const_reference at(key_type const& key) const {
		auto pos = this->find(key);
		AssertExcept<std::out_of_range>(pos != this->cend(), "");
		return *pos;
	}

private:
	template <typename K, typename M>
	constexpr std::pair<iterator, bool> _insert_or_assign(K&& key, M&& obj) {
		auto pos = this->lower_bound(key);
		if (pos != this->end() and not this->_key_comp()(key, pos->key())) {
			pos->value() = std::forward<M>(obj);
			return {pos, false};
		}
		return {this->emplace_hint(pos, std::forward<K>(key), std::forward<M>(obj)), true};
	}
	template <typename K, typename M>
	constexpr iterator _insert_or_assign(const_iterator hint, K&& key, M&& obj) {
		auto pos = this->lower_bound(hint, key);
		if (pos != this->end() and not this->_key_comp()(key, pos->key())) {
			pos->value() = std::forward<M>(obj);
			return pos;
		}
		return this->emplace_hint(pos, std::forward<K>(key), std::forward<M>(obj));
	}

public:
	template <typename M>
	constexpr std::pair<iterator, bool> insert_or_assign(key_type const& key, M&& obj) {
		return _insert_or_assign(key, std::forward<M>(obj));
	}
	template <typename M>
	constexpr std::pair<iterator, bool> insert_or_assign(key_type && key, M&& obj) {
		return _insert_or_assign(std::move(key), std::forward<M>(obj));
	}
	template <typename M>
	constexpr std::pair<iterator, bool> insert_or_assign(const_iterator hint, key_type const& key, M&& obj) {
		return _insert_or_assign(hint, key, std::forward<M>(obj));
	}
	template <typename M>
	constexpr std::pair<iterator, bool> insert_or_assign(const_iterator hint, key_type && key, M&& obj) {
		return _insert_or_assign(hint, std::move(key), std::forward<M>(obj));
	}

	template <typename... Args>
	constexpr std::pair<iterator, bool> try_emplace(key_type const& key, Args&&... args) {
		mapped_type v(std::forward<Args>(args)...);
		return insert_or_assign(key, std::move(v));
	}
	template <typename... Args>
	constexpr std::pair<iterator, bool> try_emplace(key_type && key, Args&&... args) {
		mapped_type v(std::forward<Args>(args)...);
		return insert_or_assign(std::move(key), std::move(v));
	}
	template <typename... Args>
	constexpr iterator try_emplace(const_iterator hint, key_type const& key, Args&&... args) {
		mapped_type v(std::forward<Args>(args)...);
		return insert_or_assign(hint, key, std::move(v));
	}
	template <typename... Args>
	constexpr iterator try_emplace(const_iterator hint,key_type && key, Args&&... args) {
		mapped_type v(std::forward<Args>(args)...);
		return insert_or_assign(hint, std::move(key), std::move(v));
	}
};

template <typename K, typename V, std::size_t MaxN, typename Compare = std::less<K>>
using FlatMap = BasicFlatMap<K, V, Compare, Constainer::Vector<ConstKeyPair<K, V>, MaxN>>;
template <typename K, typename V, std::size_t MaxN, typename Compare = std::less<K>>
using StableFlatMap = BasicFlatMap<K, V, Compare, Constainer::StableVector<ConstKeyPair<K, V>, ChunkPool<int, MaxN>>>;

}
