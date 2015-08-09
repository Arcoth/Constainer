#ifndef CHUNKALLOC_HXX_INCLUDED
#define CHUNKALLOC_HXX_INCLUDED

#include "Array.hxx"
#include "Bitset.hxx"

namespace Constainer {

template <typename T, std::size_t N>
class ChunkPool : private Array<T, N> {

	using _base = Array<T, N>;

public:

	template <typename U>
	using rebind = ChunkPool<U, N>;

	using typename _base::size_type;
	using typename _base::value_type;
	using typename _base::pointer;
	using typename _base::const_pointer;
	using typename _base::reference;
	using typename _base::const_reference;

	constexpr std::size_t max_size() const {return _base::size();}

private:
	Bitset<N> _used;

public:

	constexpr ChunkPool() : _base{}, _used{} {}

	constexpr size_type used() const {
		return _used.count();
	}

	constexpr bool available() const {
		return !_used.all();
	}

	constexpr pointer grab() {
		auto pos = _used.leading(1);
		AssertExcept<std::bad_alloc>(pos != this->size());
		_used.set(pos);
		return &_base::operator[](pos);
	}

	constexpr void free() {
		_used.reset();
	}
	constexpr void free( const_pointer p ) {
		auto i = p-_base::data();
		Assert( _used.test(i) );
		_used.reset(i);
	}
};

}

#endif // CHUNKALLOC_HXX_INCLUDED
