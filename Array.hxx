#include "Algorithms.hxx"

#include <cstddef> // size_t

namespace Constainer {

template <typename T, std::size_t Size>
class Array {
public:

	using size_type = std::size_t;
	static auto constexpr size() {return Size;}

	using difference_type = std::ptrdiff_t;

	using pointer = T*;
	using const_pointer = T const*;
	using reference = T&;
	using const_reference = T const&;
	using iterator = pointer;
	using const_iterator = const_pointer;

private:

	T _storage[size()] = {};

public:

	constexpr pointer       data()       {return _storage;}
	constexpr const_pointer data() const {return _storage;}

	constexpr       reference front()       {return _storage[0       ];}
	constexpr const_reference front() const {return _storage[0       ];}
	constexpr       reference back ()       {return _storage[size()-1];}
	constexpr const_reference back () const {return _storage[size()-1];}

	constexpr iterator       begin()       {return _storage         ;}
	constexpr const_iterator begin() const {return _storage         ;}
	constexpr iterator       end  ()       {return _storage + size();}
	constexpr const_iterator end  () const {return _storage + size();}

	constexpr reference       operator[](size_type index)       {return _storage[index];}
	constexpr const_reference operator[](size_type index) const {return _storage[index];}

	constexpr void fill(T const& value) {
		for (auto& val : *this)
			val = value;
	}

	constexpr void swap(Array& other) {
		swap( _storage, other._storage );
	}
};

template <typename T, std::size_t N>
constexpr void swap( Array<T, N>& lhs, Array<T, N>& rhs ) {
	lhs.swap(rhs);
}

}
