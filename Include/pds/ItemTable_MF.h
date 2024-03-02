// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include <ctle/log.h>

#include "ItemTable.h"

#include "EntityWriter.h"
#include "EntityReader.h"
#include "EntityValidator.h"

namespace pds
{
#include "_pds_macros.inl"

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
class ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF
{
	using _MgmCl = ItemTable<_Kty, _Ty, _Flags, _MapTy>;

public:
	static void Clear( _MgmCl &obj );
	static void DeepCopy( _MgmCl &dest, const _MgmCl *source );
	static bool Equals( const _MgmCl *lval, const _MgmCl *rval );

	static bool Write( const _MgmCl &obj, EntityWriter &writer );
	static bool Read( _MgmCl &obj, EntityReader &reader );

	static bool Validate( const _MgmCl &obj, EntityValidator &validator );

	// additional validation with external data
	template<class _Table> static bool ValidateAllKeysAreContainedInTable( const _MgmCl &obj, EntityValidator &validator, const _Table &otherTable, const char *otherTableName );

	// support methods for validation
	static bool ContainsKey( const _MgmCl &obj, const _Kty &key );
};

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
void ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Clear( _MgmCl &obj )
{
	obj.v_Entries.clear();
}

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
void ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::DeepCopy( _MgmCl &dest, const _MgmCl *source )
{
	MF::Clear( dest );
	if( !source )
		return;

	for( const auto &ent : source->v_Entries )
	{
		// make a new copy of the value, if original is not nullptr
		dest.v_Entries.emplace( ent.first, std::unique_ptr<_Ty>( ( ent.second ) ? new _Ty( *ent.second ) : nullptr ) );
	}
}

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
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


template<class _Kty, class _Ty, uint _Flags, class _MapTy>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Write( const _MgmCl &obj, EntityWriter &writer )
{
	// collect the keys into a vector, and store in stream as an array
	std::vector<_Kty> keys( obj.v_Entries.size() );
	size_t index = 0;
	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it, ++index )
	{
		keys[index] = it->first;
	}
	if( !writer.Write( pdsKeyMacro( "IDs" ), keys ) )
		return false;
	keys.clear();

	// create a sections array for the entities
	EntityWriter *section_writer = writer.BeginWriteSectionsArray( pdsKeyMacro( "Entities" ), obj.v_Entries.size() );
	if( !section_writer )
		return false;

	// write out all the entities as an array
	// for each non-empty entity, call the write method of the entity
	index = 0;
	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it, ++index )
	{
		if( !writer.BeginWriteSectionInArray( section_writer, index ) )
			return false;
		if( it->second )
		{
			if( !_Ty::MF::Write( *( it->second ), *( section_writer ) ) )
				return false;
		}
		if( !writer.EndWriteSectionInArray( section_writer, index ) )
			return false;
	}

	// sanity check, make sure all sections were written
	ctSanityCheck( index == obj.v_Entries.size() );

	// end the Entries sections array
	if( !writer.EndWriteSectionsArray( section_writer ) )
		return false;

	return true;
}

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Read( _MgmCl &obj, EntityReader &reader )
{
	EntityReader *section_reader = {};
	size_t map_size = {};
	bool success = {};
	typename _MgmCl::iterator it = {};

	// read in the keys as a vector
	std::vector<_Kty> keys;
	if( !reader.Read( pdsKeyMacro( "IDs" ), keys ) )
		return false;

	// begin the named sections array
	std::tie( section_reader, map_size, success ) = reader.BeginReadSectionsArray( pdsKeyMacro( "Entities" ), false );
	if( !success )
		return false;
	ctSanityCheck( section_reader );
	if( map_size != keys.size() )
	{
		ctLogError << "Invalid size in ItemTable, the Keys and Entities arrays do not match in size." << ctLogEnd;
		return false;
	}

	// read in all the entities, push into map as key-value pairs
	obj.v_Entries.clear();
	for( size_t index = 0; index < map_size; ++index )
	{
		bool has_data = false;
		if( !reader.BeginReadSectionInArray( section_reader, index, &has_data ) )
			return false;

		if( has_data )
			std::tie( it, success ) = obj.v_Entries.emplace( keys[index], std::make_unique<_Ty>() );
		else
			std::tie( it, success ) = obj.v_Entries.emplace( keys[index], nullptr );

		if( !success )
		{
			ctLogError << "Failed inserting key-value pair in ItemTable" << ctLogEnd;
			return false;
		}

		if( it->second )
		{
			if( !_Ty::MF::Read( *( it->second ), *( section_reader ) ) )
				return false;
		}

		if( !reader.EndReadSectionInArray( section_reader, index ) )
			return false;
	}

	// end the sections array
	if( !reader.EndReadSectionsArray( section_reader ) )
		return false;

	return true;
}

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::Validate( const _MgmCl &obj, EntityValidator &validator )
{
	// check if a zero key exists in the dictionary
	if( _MgmCl::type_no_zero_keys )
	{
		if( obj.v_Entries.find( data_type_information<_Kty>::zero ) != obj.v_Entries.end() )
		{
			pdsValidationError( ValidationError::NullNotAllowed ) << "This Directory has a zero-value key, which is not allowed. (DictionaryFlags::NoZeroKeys)" << pdsValidationErrorEnd;
		}
	}

	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it )
	{
		// check value
		if( it->second )
		{
			if( !_Ty::MF::Validate( *( it->second ), validator ) )
				return false;
		}
		else if( _MgmCl::type_no_null_entities )
		{
			// value is empty, and this is not allowed in this dictionary
			pdsValidationError( ValidationError::NullNotAllowed ) << "Non allocated entities (values) are not allowed in this Directory. (DictionaryFlags::NoNullEntities)" << pdsValidationErrorEnd;
		}
	}

	return true;
}

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
template<class _Table>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::ValidateAllKeysAreContainedInTable( const _MgmCl &obj, EntityValidator &validator, const _Table &otherTable, const char *otherTableName )
{
	for( auto it = obj.v_Entries.begin(); it != obj.v_Entries.end(); ++it )
	{
		if( !_Table::MF::ContainsKey( otherTable, it->first ) )
		{
			pdsValidationError( ValidationError::MissingObject ) << "The key " << it->first << " is missing in " << otherTableName << pdsValidationErrorEnd;
		}
	}
	return true;
}

template<class _Kty, class _Ty, uint _Flags, class _MapTy>
bool ItemTable<_Kty, _Ty, _Flags, _MapTy>::MF::ContainsKey( const _MgmCl &obj, const _Kty &key )
{
	return obj.v_Entries.find( key ) != obj.v_Entries.end();
}

#include "_pds_undef_macros.inl"
};