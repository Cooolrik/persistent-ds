// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "ValueTypes.h"

#include "EntityWriter.h"
#include "EntityReader.h"
#include "EntityValidator.h"

namespace pds
{


template <class _Ty, class _Base = idx_vector<_Ty> >
class IndexedVector : public _Base
{
public:
	using base_type = _Base;

	class MF;
	friend MF;

	// ctors/dtor and copy/move operators
	IndexedVector() = default;
	IndexedVector( const IndexedVector &rval ) = default;
	IndexedVector &operator=( const IndexedVector &rval ) = default;
	IndexedVector( IndexedVector &&rval ) = default;
	IndexedVector &operator=( IndexedVector &&rval ) = default;
	~IndexedVector() = default;

	// value compare operators
	bool operator==( const IndexedVector &rval ) const
	{
		return _Base::operator==( rval );
	}
	bool operator!=( const IndexedVector &rval ) const
	{
		return _Base::operator!=( rval );
	}
};



}
// namespace pds