// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__INDEXEDVECTOR_H__
#define __PDS__INDEXEDVECTOR_H__


#include "fwd.h"

namespace pds
{

// @brief IndexedVector implements an indexed vector, default based on the idx_vector from ctle.
template <
	class _Ty,		/* the type of the value in the vector */
	class _IdxTy,	/* = u32 the type of the index in the vector */
	class _Base		/* = ctle::idx_vector<_Ty,std::vector<_IdxTy>>. the base class of the vector, use to override implementation and allocations etc. */
> class IndexedVector : public _Base
{
public:
	class MF;

	using base_type = _Base;

	// ctors/dtor and copy/move operators
	IndexedVector() = default;
	IndexedVector( const IndexedVector &rval ) = default;
	IndexedVector &operator=( const IndexedVector &rval ) = default;
	IndexedVector( IndexedVector &&rval ) = default;
	IndexedVector &operator=( IndexedVector &&rval ) = default;
	~IndexedVector() = default;

	// value compare operators
	bool operator==( const IndexedVector &rval ) const;
	bool operator!=( const IndexedVector &rval ) const;

protected:
	friend MF;
};

}
// namespace pds

#include "IndexedVector.inl"

#endif//__PDS__INDEXEDVECTOR_H__
