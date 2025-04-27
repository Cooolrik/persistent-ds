// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__ITEMTABLE_H__
#define __PDS__ITEMTABLE_H__

#include <unordered_map>
#include "pds.h"

namespace pds
{

// ItemTable holds a map of key values to unique memory mapped objects. 
// This table class is the main holder of most objects in ISD.

enum class item_table_flags : uint
{
	zero_keys = 0x1,		// if set, validation will allow zero value keys (0 for ints, all 0s in uuids, empty strings) 
	null_entities = 0x02,	// if set, validation will allow that null entities exist in the table
};

template<
	class _Kty, 
	class _Ty, 
	item_table_flags _Flags, // = 0, a combination of item_table_flags flags for the behaviour of the item table
	class _MapTy			 // = std::unordered_map<_Kty, std::unique_ptr<_Ty>>
> class ItemTable
{
public:
	using key_type = _Kty;
	using mapped_type = _Ty;
	using map_type = _MapTy;

	using value_type = typename map_type::value_type;
	using iterator = typename map_type::iterator;
	using const_iterator = typename map_type::const_iterator;

	static const bool type_no_zero_keys = ( _Flags & ItemTableFlags::ZeroKeys ) == 0;
	static const bool type_no_null_entities = ( _Flags & ItemTableFlags::NullEntities ) == 0;

	class MF;
	friend MF;

	// ctors/dtor and copy/move operators
	ItemTable() = default;
	ItemTable( const ItemTable &rval ) { MF::DeepCopy( *this, &rval ); }
	ItemTable &operator=( const ItemTable &rval ) { MF::DeepCopy( *this, &rval ); return *this; }
	ItemTable( ItemTable &&rval ) = default;
	ItemTable &operator=( ItemTable &&rval ) = default;
	~ItemTable() = default;

	// value compare operators
	bool operator==( const ItemTable &rval ) const { return MF::Equals( this, &rval ); }
	bool operator!=( const ItemTable &rval ) const { return !( MF::Equals( this, &rval ) ); }

private:
	map_type v_Entries;

public:
	// returns the number of entries in the ItemTable
	size_t Size() const noexcept { return this->v_Entries.size(); }

	// direct access to the Entries map
	map_type &Entries() noexcept { return this->v_Entries; }
	const map_type &Entries() const noexcept { return this->v_Entries; }

	// index access operator. node, dereferences the mapped value, so will throw if the value does not exist.
	mapped_type &operator[]( const key_type &key ) { return *( this->v_Entries[key].get() ); }
	const mapped_type &operator[]( const key_type &key ) const { return *( this->v_Entries[key].get() ); }

	// insert a key and new empty value, returns reference to value
	mapped_type &Insert( const key_type &key ) 
	{
		this->v_Entries.emplace( key, std::make_unique<mapped_type>() ); 
		return *( this->v_Entries[key].get() );
	}
};

};
#endif//__PDS__ITEMTABLE_H__
