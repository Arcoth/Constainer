#ifndef VECTOR_HXX_INCLUDED
#define VECTOR_HXX_INCLUDED

#include "Array.hxx"
#include "Assert.hxx"

namespace Constainer {

template <typename T, std::size_t MaxN>
class Vector : protected Array<T, MaxN> {

	using _base = Array<T, MaxN>;

public:
	using typename _base::size_type;
	using typename _base::value_type;
	using typename _base::iterator;
	using typename _base::const_iterator;
	using typename _base::reference;
	using typename _base::const_reference;

private:
	size_type _size = 0;

	constexpr void _verifySize() const {
		AssertExcept<std::bad_alloc>( size() <= MaxN );
	}
	constexpr void _sizeIncable() const {
		AssertExcept<std::bad_alloc>( size() <= MaxN-1 );
	}
	constexpr void _verifiedSizeInc() {
		++_size; _verifySize();
	}

public:

	constexpr auto size() const {return _size;}

	constexpr Vector() :_base{},  _size(0) {}

	constexpr Vector( size_type s ) : _base{}, _size(s) {_verifySize();}

	constexpr Vector( size_type s, value_type const& v ) : Vector(s) {
		fill_n( this->begin(), size(), v );
	}

	constexpr iterator       end()       {return this->begin() + size();}
	constexpr const_iterator end() const {return this->begin() + size();}

	constexpr       reference back ()       {return end()[-1];}
	constexpr const_reference back () const {return end()[-1];}

	template <typename... Args>
	constexpr void emplace_back( Args&&... args ) {
		push_back( value_type(std::forward<Args>(args)...) );
	}

private:
	template <typename U>
	constexpr void _push_back( U&& u ) {
		_sizeIncable();
		*this->end() = std::forward<U>(u);
		++_size;
	}

public:

	constexpr void push_back( value_type const& v ) {
		_push_back(v);
	}
	constexpr void push_back( value_type&& v ) {
		_push_back(std::move(v));
	}

	constexpr void erase( const_iterator first, const_iterator last ) {
		move_backward( const_cast<iterator>(last), this->end(),
		               const_cast<iterator>(first) + (this->end() - last));
		_size -= last-first;
	}
	constexpr void erase( const_iterator it ) {
		erase(it, it+1);
	}
};

}

#endif // VECTOR_HXX_INCLUDED
