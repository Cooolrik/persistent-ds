// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "BidirectionalMap.h"

namespace pds 
{
#include "_pds_macros.inl"


template<class _Kty, class _Vty, class _Base>
class BidirectionalMap<_Kty, _Vty, _Base>::MF
{
	using _MgmCl = BidirectionalMap<_Kty, _Vty, _Base>;

public:
	static status Clear( _MgmCl &obj );
	static status DeepCopy( _MgmCl &dest, const _MgmCl *source );
	static bool Equals( const _MgmCl *lval, const _MgmCl *rval );

	static status Write( const _MgmCl &obj, EntityWriter &writer );
	static status Read( _MgmCl &obj, EntityReader &reader );

	static status Validate( const _MgmCl &obj, EntityValidator &validator );

	// support methods for validation
	static bool ContainsKey( const _MgmCl &obj, const _Kty &key );
};

template<class _Kty, class _Vty, class _Base>
status BidirectionalMap<_Kty, _Vty, _Base>::MF::Clear( _MgmCl &obj )
{
	obj.clear();
	return status::ok;
}

template<class _Kty, class _Vty, class _Base>
status BidirectionalMap<_Kty, _Vty, _Base>::MF::DeepCopy( _MgmCl &dest, const _MgmCl *source )
{
	MF::Clear( dest );
	if( !source )
		return status::ok;
	dest = *source;
	return status::ok;
}

template<class _Kty, class _Vty, class _Base>
bool BidirectionalMap<_Kty, _Vty, _Base>::MF::Equals( const _MgmCl *lval, const _MgmCl *rval )
{
	// early out if the pointers are equal (includes nullptr)
	if( lval == rval )
		return true;

	// early out if one of the pointers is nullptr (both can't be null because of above test)
	if( !lval || !rval )
		return false;

	// deep compare the values
	const BidirectionalMap<_Kty, _Vty, _Base>::base_type &_lval = *lval;
	const BidirectionalMap<_Kty, _Vty, _Base>::base_type &_rval = *rval;
	return ( _lval == _rval );
}

template<class _Kty, class _Vty, class _Base>
status BidirectionalMap<_Kty, _Vty, _Base>::MF::Write( const _MgmCl &obj, EntityWriter &writer )
{
	// enumerate all keys and values to two separate vectors
	std::vector<_Kty> keys( obj.size() );
	std::vector<_Vty> values( obj.size() );
	size_t index = 0;
	for( auto it = obj.begin(); it != obj.end(); ++it, ++index )
	{
		keys[index] = it->first;
		values[index] = it->second;
	}

	// write vectors 
	if( !writer.Write( pdsKeyMacro( Keys ), keys ) )
		return status::cant_write;
	if( !writer.Write( pdsKeyMacro( Values ), values ) )
		return status::cant_write;

	return status::ok;
}

template<class _Kty, class _Vty, class _Base>
status BidirectionalMap<_Kty, _Vty, _Base>::MF::Read( _MgmCl &obj, EntityReader &reader )
{
	obj.clear();

	std::vector<_Kty> keys;
	std::vector<_Vty> values;

	// read in vectors with keys and values
	if( !reader.Read( pdsKeyMacro( Keys ), keys ) )
		return status::cant_read;
	if( !reader.Read( pdsKeyMacro( Values ), values ) )
		return status::cant_read;

	// insert into map
	for( size_t index = 0; index < keys.size(); ++index )
	{
		obj.insert( keys[index], values[index] );
	}

	return status::ok;
}

template<class _Kty, class _Vty, class _Base>
status BidirectionalMap<_Kty, _Vty, _Base>::MF::Validate( const _MgmCl &/*obj*/, EntityValidator &/*validator*/ )
{
	return status::ok;
}

template<class _Kty, class _Vty, class _Base>
bool BidirectionalMap<_Kty, _Vty, _Base>::MF::ContainsKey( const _MgmCl &obj, const _Kty &key )
{
	return obj.contains_key( key );
}


#include "_pds_undef_macros.inl"
}
// namespace pds