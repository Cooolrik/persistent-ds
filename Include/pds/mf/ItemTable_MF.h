// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__ITEMTABLE_MF_H__
#define __PDS__ITEMTABLE_MF_H__

#include <ctle/log.h>

#include "../ItemTable.h"

#include "../EntityWriter.h"
#include "../EntityReader.h"
#include "../EntityValidator.h"

namespace pds
{
#include "../_pds_macros.inl"

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
class ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF
{
	using _MgmCl = ItemTable<_Kty, _Ty, _Flags, _MapTy>;

public:
	static status Clear( _MgmCl &obj );
	static status DeepCopy( _MgmCl &dest, const _MgmCl *source );
	static bool Equals( const _MgmCl *lval, const _MgmCl *rval );

	static status Write( const _MgmCl &obj, EntityWriter &writer );
	static status Read( _MgmCl &obj, EntityReader &reader );

	static status Validate( const _MgmCl &obj, EntityValidator &validator );

	// additional validation with external data
	template<class _Table> static bool ValidateAllKeysAreContainedInTable( const _MgmCl &obj, EntityValidator &validator, const _Table &otherTable, const char *otherTableName );

	// support methods for validation
	static bool ContainsKey( const _MgmCl &obj, const _Kty &key );
};

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
status ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Clear( _MgmCl &obj )
{
	obj.v_Entries.clear();
	return status::ok;
}

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
status ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::DeepCopy( _MgmCl &dest, const _MgmCl *source )
{
	MF::Clear( dest );
	if( !source )
		return status::ok;

	for( const auto &ent : source->v_Entries )
	{
		// make a new copy of the value, if original is not nullptr
		dest.v_Entries.emplace( ent.first, std::unique_ptr<_Ty>( ( ent.second ) ? new _Ty( *ent.second ) : nullptr ) );
	}
	return status::ok;
}

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Equals( const _MgmCl *lval, const _MgmCl *rval )
{
	// early out if the pointers are equal (includes nullptr)
	if( lval == rval )
		return true;

	// early out if one of the pointers is nullptr (both can't be null because of above test)
	if( !lval || !rval )
		return false;

	// early out if the sizes are not the same 
	if( lval->Size() != rval->Size() )
		return false;

	// compare all the entries
	auto lval_it = lval->v_Entries.begin();
	while( lval_it != lval->v_Entries.end() )
	{
		// find the key in the right object, should always find
		auto rval_it = rval->v_Entries.find( lval_it->first );
		if( rval_it == rval->v_Entries.end() )
			return false;

		// compare values ptrs 
		// if pointers are not equal, check contents
		auto lval_value_ptr = lval_it->second.get();
		auto rval_value_ptr = rval_it->second.get();
		if( lval_value_ptr != rval_value_ptr )
		{
			// first, make sure both pointers are valid (since they are not both nullptr)
			if( !lval_value_ptr || !rval_value_ptr )
				return false;

			// dereference and compare contents
			if( !( ( *lval_value_ptr ) == ( *rval_value_ptr ) ) )
				return false;
		}

		// step
		++lval_it;
	}

	return true;
}


template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
status ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Write( const _MgmCl &obj, EntityWriter &writer )
{
	// collect the keys into a vector, and store in stream as an array
	std::vector<_Kty> keys( obj.v_Entries.size() );
	size_t index = 0;
	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it, ++index )
	{
		keys[index] = it->first;
	}
	if( !writer.Write( pdsKeyMacro( IDs ), keys ) )
		return status::cant_write;
	keys.clear();

	// create a sections array for the entities
	EntityWriter *section_writer;
	ctStatusReturnCall( section_writer, writer.BeginWriteSectionsArray( pdsKeyMacro( Ents ), obj.v_Entries.size() ) );

	// write out all the entities as an array
	// for each non-empty entity, call the write method of the entity
	index = 0;
	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it, ++index )
	{
		if( !writer.BeginWriteSectionInArray( section_writer, index ) )
			return status::cant_write;
		if( it->second )
		{
			ctStatusCall( _Ty::MF::Write( *( it->second ), *( section_writer ) ) );
		}
		if( !writer.EndWriteSectionInArray( section_writer, index ) )
			return status::cant_write;
	}

	// sanity check, make sure all sections were written
	ctSanityCheck( index == obj.v_Entries.size() );

	// end the Entries sections array
	ctStatusCall( writer.EndWriteSectionsArray( section_writer ) );

	return status::ok;
}

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
status ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Read( _MgmCl &obj, EntityReader &reader )
{
	EntityReader *section_reader = {};
	size_t map_size = {};
	bool success = {};
	typename _MgmCl::iterator it = {};

	// read in the keys as a vector
	std::vector<_Kty> keys;
	if( !reader.Read( pdsKeyMacro( IDs ), keys ) )
		return status::cant_read;

	// begin the named sections array
	ctStatusReturnCall( section_reader, reader.BeginReadSectionsArray( pdsKeyMacro( Ents ), false ) );
	ctSanityCheck( section_reader );
	if( map_size != keys.size() )
	{
		ctLogError << "Invalid size in ItemTable, the Keys and Entities arrays do not match in size." << ctLogEnd;
		return status::corrupted;
	}

	// read in all the entities, push into map as key-value pairs
	obj.v_Entries.clear();
	for( size_t index = 0; index < map_size; ++index )
	{
		bool has_data = false;
		if( !reader.BeginReadSectionInArray( section_reader, index, &has_data ) )
			return status::cant_read;

		if( has_data )
			std::tie( it, success ) = obj.v_Entries.emplace( keys[index], std::make_unique<_Ty>() );
		else
			std::tie( it, success ) = obj.v_Entries.emplace( keys[index], nullptr );

		if( !success )
		{
			ctLogError << "Failed inserting key-value pair in ItemTable" << ctLogEnd;
			return status::cant_read;
		}

		if( it->second )
		{
			ctStatusCall( _Ty::MF::Read( *( it->second ), *( section_reader ) ) );
		}

		if( !reader.EndReadSectionInArray( section_reader, index ) )
			return status::cant_read;
	}

	// end the sections array
	if( !reader.EndReadSectionsArray( section_reader ) )
		return status::cant_read;

	return status::ok;
}

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
status ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Validate( const _MgmCl &obj, EntityValidator &validator )
{
	// check if a zero key exists in the dictionary
	if( _MgmCl::type_no_zero_keys )
	{
		if( obj.v_Entries.find( element_type_information<_Kty>::zero ) != obj.v_Entries.end() )
		{
			pdsValidationError( validation_error_flags::null_not_allowed ) << "This Directory has a zero-value key, which is not allowed. (item_table_flags::zero_keys is not set)" << pdsValidationErrorEnd;
		}
	}

	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it )
	{
		// check value
		if( it->second )
		{
			ctStatusCall(_Ty::MF::Validate( *( it->second ), validator ) );
		}
		else if( _MgmCl::type_no_null_entities )
		{
			// value is empty, and this is not allowed in this dictionary
			pdsValidationError( validation_error_flags::null_not_allowed ) << "Non allocated entities (values) are not allowed in this Directory. (item_table_flags::null_entities is not set)" << pdsValidationErrorEnd;
		}
	}

	return status::ok;
}

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
template<class _Table>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::ValidateAllKeysAreContainedInTable( const _MgmCl &obj, EntityValidator &validator, const _Table &otherTable, const char *otherTableName )
{
	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it )
	{
		if( !_Table::MF::ContainsKey( otherTable, it->first ) )
		{
			pdsValidationError( validation_error_flags::missing_object ) << "The key " << it->first << " is missing in " << otherTableName << pdsValidationErrorEnd;
		}
	}
	return true;
}

template<class _Kty, class _Ty, item_table_flags _Flags, class _MapTy>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::ContainsKey( const _MgmCl &obj, const _Kty &key )
{
	return obj.v_Entries.find( key ) != obj.v_Entries.end();
}

#include "../_pds_undef_macros.inl"

}
// namespace pds

#endif//__PDS__ITEMTABLE_MF_H__
