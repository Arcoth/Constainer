#pragma once

namespace Constainer {

template <typename Container>
struct insert_iterator_base {
	using container_type = Container;
	typedef void value_type, difference_type, pointer, reference;
	using iterator_category = STD::output_iterator_tag;

protected:
	container_type* container;

public:
	constexpr insert_iterator_base(container_type& c) :
		container(&c) {}

	constexpr insert_iterator_base& operator*()     {return *this;}
	constexpr insert_iterator_base& operator++()    {return *this;}
	constexpr insert_iterator_base& operator++(int) {return *this;}
};

template <typename Container>
struct back_insert_iterator : insert_iterator_base<Container> {
	using insert_iterator_base<Container>::insert_iterator_base;
	template <typename T>
	constexpr back_insert_iterator& operator=(T&& value) {
		this->container->push_back(STD::forward<T>(value));
		return *this;
	}
};
template <typename Container>
struct front_insert_iterator : insert_iterator_base<Container> {
	using insert_iterator_base<Container>::insert_iterator_base;
	template <typename T>
	constexpr front_insert_iterator& operator=(T&& value) {
		this->container->push_front(STD::forward<T>(value));
		return *this;
	}
};
template <typename Container>
struct insert_iterator : insert_iterator_base<Container> {
protected:
	typename Container::iterator iter;

public:
	insert_iterator(Container& c, typename Container::iterator i) :
		insert_iterator_base<Container>(c), iter(i) {}
	template <typename T>
	constexpr insert_iterator& operator=(T&& value) {
		iter = ++(this->container->insert(iter, STD::forward<T>(value)));
		return *this;
	}
};

}
