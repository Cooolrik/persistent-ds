// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__ITEM_REF_H__
#define __PDS__ITEM_REF_H__

#include "fwd.h"
#include <ctle/uuid.h>

// entity_ref - Reference to another entity package. 
namespace pds
{
	// Reference to an element within the same package. Acts like a memory handle. 
	// It can be created, held and copied, but cannot be set to a specific value other than null.
	class item_ref
	{
	private:
		uuid id_m;

		// create from uuid, only allow this internally and from EntityReader
		constexpr item_ref( const uuid &id ) { this->id_m = id; }
		static item_ref from_uuid( const uuid &id ) { return item_ref(id); }
		friend class EntityReader;

	public:
		// ctors/dtor
		constexpr item_ref() noexcept : id_m( uuid::zero() ) {}
		constexpr item_ref( const item_ref &other ) noexcept : id_m( other.id_m ) {}
		item_ref &operator=( const item_ref &other ) noexcept { this->id_m = other.id_m; return *this; }
		constexpr item_ref( item_ref &&other ) noexcept : id_m( other.id_m ) { other.id_m = uuid::zero(); }
		item_ref &operator=( item_ref&& other ) noexcept { this->id_m = other.id_m; other.id_m = uuid::zero(); return *this; }
		~item_ref() = default;

		// make a new reference with a unique uuid
		static item_ref make_ref();

		// comparing & sorting
		bool operator==( const item_ref &rval ) const { return this->id_m == rval.id_m; }
		bool operator!=( const item_ref &rval ) const { return this->id_m != rval.id_m; }
		bool operator<( const item_ref &rval ) const { return this->id_m < rval.id_m; }

		// conversions to boolean (behaves like a pointer, true if non-null) 
		operator bool() const { return this->id_m != uuid::zero(); }
		operator uuid() const { return this->id_m; }

		// returns the "null" item_ref value 
		static const item_ref &null() { static item_ref null_id; return null_id; }
		
		// inf and sup values, for comparing
		static constexpr const item_ref zero() { return item_ref(uuid::zero()); }
		static constexpr const item_ref inf() { return item_ref(uuid::inf()); }
		static constexpr const item_ref sup() { return item_ref(uuid::sup()); }
	};

	constexpr const item_ref item_ref_zero( item_ref::zero() );
	constexpr const item_ref item_ref_inf( item_ref::inf() );
	constexpr const item_ref item_ref_sup( item_ref::sup() );
}
// namespace pds

#ifdef PDS_IMPLEMENTATION

// std::hash generation implementation
template<>
struct std::hash<pds::item_ref>
{
	std::size_t operator()(pds::item_ref const& val) const noexcept
	{
		return std::hash<ctle::uuid>{}( ctle::uuid(val) );
	}
};

// ctle::to_string implementation
template <>
std::string ctle::to_string<pds::item_ref>( const pds::item_ref &val )
{
	return ctle::to_string<ctle::uuid>( ctle::uuid(val) );
}

namespace pds
{

// std::ostream operator implementation
std::ostream &operator<<( std::ostream &os, const item_ref &ref )
{
	return os << ctle::to_string( ctle::uuid(ref) );
}

// make a new item reference
item_ref item_ref::make_ref()
{
	return item_ref::from_uuid( uuid::generate() );
}

}
// namespace pds


#endif//PDS_IMPLEMENTATION

#endif//__PDS__ITEM_REF_H__