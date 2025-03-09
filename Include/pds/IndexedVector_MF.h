// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "IndexedVector.h"

#include "EntityWriter.h"
#include "EntityReader.h"
#include "EntityValidator.h"

namespace pds
{
#include "_pds_macros.inl"


template<class _Ty, class _Base>
class IndexedVector<_Ty, _Base>::MF
{
	using _MgmCl = IndexedVector<_Ty, _Base>;

public:
	static status Clear( _MgmCl &obj );
	static status DeepCopy( _MgmCl &dest, const _MgmCl *source );
	static bool Equals( const _MgmCl *lval, const _MgmCl *rval );

	static status Write( const _MgmCl &obj, EntityWriter &writer );
	static status Read( _MgmCl &obj, EntityReader &reader );

	static status Validate( const _MgmCl &obj, EntityValidator &validator );
};

template<class _Ty, class _Base>
status IndexedVector<_Ty, _Base>::MF::Clear( _MgmCl &obj )
{
	obj.clear();
	return status::ok;
}

template<class _Ty, class _Base>
status IndexedVector<_Ty, _Base>::MF::DeepCopy( _MgmCl &dest, const _MgmCl *source )
{
	MF::Clear( dest );
	if( !source )
		return status::ok;
	dest = *source;
	return status::ok;
}

template<class _Ty, class _Base>
bool IndexedVector<_Ty, _Base>::MF::Equals( const _MgmCl *lval, const _MgmCl *rval )
{
	// early out if the pointers are equal (includes nullptr)
	if( lval == rval )
		return true;

	// early out if one of the pointers is nullptr (both can't be null because of above test)
	if( !lval || !rval )
		return false;

	// deep compare the values
	const IndexedVector<_Ty, _Base>::base_type &_lval = *lval;
	const IndexedVector<_Ty, _Base>::base_type &_rval = *rval;
	return ( _lval == _rval );
}

template<class _Ty, class _Base>
status IndexedVector<_Ty, _Base>::MF::Write( const _MgmCl &obj, EntityWriter &writer )
{
	const IndexedVector<_Ty, _Base>::base_type &_obj = obj;
	if( !writer.Write( pdsKeyMacro( Values ), _obj ) )
		return status::cant_write;
	return status::ok;
}

template<class _Ty, class _Base>
status IndexedVector<_Ty, _Base>::MF::Read( _MgmCl &obj, EntityReader &reader )
{
	IndexedVector<_Ty, _Base>::base_type &_obj = obj;
	if( !reader.Read( pdsKeyMacro( Values ), _obj ) )
		return status::cant_read;
	return status::ok;
}

template<class _Ty, class _Base>
status IndexedVector<_Ty, _Base>::MF::Validate( const _MgmCl &obj, EntityValidator &validator )
{
	if( obj.values().size() > (size_t)i32_sup )
	{
		pdsValidationError( ValidationError::InvalidCount ) << "This IndexedVector has too many values in the values vector. The limit is 2^31 values, which can be indexed by a 32-bit int." << pdsValidationErrorEnd;
	}

	// cap the count to 32 bit int
	const u32 values_count = (u32)(std::min)( obj.values().size(), (size_t)i32_sup );
	for( size_t i = 0; i < obj.index().size(); ++i )
	{
		if( (u32)obj.index()[i] >= values_count )
		{
			pdsValidationError( ValidationError::InvalidValue ) << "The value " << obj.index()[i] << " at position " << i << " of the index vector is out of bounds." << pdsValidationErrorEnd;
		}
	}

	return status::ok;
}


#include "_pds_undef_macros.inl"
}
// namespace pds