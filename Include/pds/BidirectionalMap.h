// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__BIDIRECTIONALMAP_H__
#define __PDS__BIDIRECTIONALMAP_H__

#include "fwd.h"

#include <ctle/bimap.h>

namespace pds
{

// bi-directional unordered_map, so has (on average) O(1) lookup of both key->value and value->key
// both key and value are stored directly, and needs to have std::hash functors defined
template <
	class _Kty, 
	class _Vty, 
	class _Base /* = ctle::bimap<_Kty, _Vty> */
> class BidirectionalMap : public _Base
{
public:
	using base_type = _Base;

	class MF;

	// ctors/dtor and copy/move operators
	BidirectionalMap() = default;
	BidirectionalMap( const BidirectionalMap &rval ) = default;
	BidirectionalMap &operator=( const BidirectionalMap &rval ) = default;
	BidirectionalMap( BidirectionalMap &&rval ) = default;
	BidirectionalMap &operator=( BidirectionalMap &&rval ) = default;
	~BidirectionalMap() = default;

	// value compare operators
	bool operator==( const BidirectionalMap &rval ) const;
	bool operator!=( const BidirectionalMap &rval ) const;

protected:
	friend MF;
};

}
// namespace pds

#include "BidirectionalMap.inl"

#endif//__PDS__BIDIRECTIONALMAP_H__