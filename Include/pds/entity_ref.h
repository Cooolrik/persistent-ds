// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__ENTITY_REF_H__
#define __PDS__ENTITY_REF_H__

#include "fwd.h"
#include <ctle/digest.h>

// entity_ref - Reference to another entity package. 
namespace pds
{
	class entity_ref
	{
	private:
		hash hash_m;

	public:
		// ctors/dtor
		entity_ref() noexcept : hash_m( hash::zero() ) {}
		constexpr entity_ref( const hash &_hash ) noexcept : hash_m( _hash ) {}
		entity_ref( const entity_ref &other ) noexcept : hash_m( other.hash_m ) {}
		entity_ref &operator=( const entity_ref &other ) noexcept { this->hash_m = other.hash_m; return *this; }
		entity_ref( entity_ref &&other ) noexcept : hash_m( other.hash_m ) { other.hash_m = hash::zero(); }
		entity_ref &operator=( entity_ref &&other ) noexcept { this->hash_m = other.hash_m; other.hash_m = hash::zero(); return *this; }
		~entity_ref() = default;

		// explicitly create from hash
		static entity_ref from_hash( const hash &hsh ) { return entity_ref(hsh); }

		// comparing & sorting
		bool operator==( const entity_ref &rval ) const { return this->hash_m == rval.hash_m; }
		bool operator!=( const entity_ref &rval ) const { return this->hash_m != rval.hash_m; }
		bool operator<( const entity_ref &rval ) const { return this->hash_m < rval.hash_m; }

		// conversions to boolean (behaves like a pointer, true if non-null) 
		operator bool() const { return this->hash_m != hash::zero(); }
		operator hash() const { return this->hash_m; }

		// returns the "null" entity_ref value 
		static const entity_ref &null() { static entity_ref null_id; return null_id; }
	};

	constexpr const entity_ref entity_ref_zero( hash::zero() );
	constexpr const entity_ref entity_ref_inf( hash::inf() );
	constexpr const entity_ref entity_ref_sup( hash::sup() );

}
// namespace pds

#ifdef PDS_IMPLEMENTATION

// std::hash generation implementation
template<>
struct std::hash <pds::entity_ref>
{
	std::size_t operator()( pds::entity_ref const &val ) const noexcept
	{
		return std::hash<pds::hash>{}( pds::hash( val ) );
	}
};

// ctle::to_string implementation
template <>
std::string ctle::to_string<pds::entity_ref>( const pds::entity_ref &val )
{
	return ctle::to_string<pds::hash>( pds::hash( val ) );
}

namespace pds
{

// std::ostream operator implementation
std::ostream &operator<<( std::ostream &os, const entity_ref &ref )
{
	return os << hash( ref );
}

}
// namespace pds

#endif//PDS_IMPLEMENTATION

#endif//__PDS__ENTITY_REF_H__