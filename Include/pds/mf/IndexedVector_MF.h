// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__INDEXEDVECTOR_MF_H__
#define __PDS__INDEXEDVECTOR_MF_H__

#include "../IndexedVector.h"

#include "../EntityWriter.h"
#include "../EntityReader.h"
#include "../EntityValidator.h"

namespace pds
{
#include "../_pds_macros.inl"

template<class _Ty, class _IdxTy, class _Base>
class IndexedVector<_Ty, _IdxTy, _Base>::MF
{
	using _MgmCl = IndexedVector<_Ty, _IdxTy, _Base>;

public:
	static status Clear( _MgmCl &obj );
	static status DeepCopy( _MgmCl &dest, const _MgmCl *source );
	static bool Equals( const _MgmCl *lval, const _MgmCl *rval );

	static status Write( const _MgmCl &obj, EntityWriter &writer );
	static status Read( _MgmCl &obj, EntityReader &reader );

	static status Validate( const _MgmCl &obj, EntityValidator &validator );
};

template<class _Ty, class _IdxTy, class _Base>
inline status IndexedVector<_Ty, _IdxTy, _Base>::MF::Clear( _MgmCl &obj )
{
	obj.clear();
	return status::ok;
}

template<class _Ty, class _IdxTy, class _Base>
inline status IndexedVector<_Ty, _IdxTy, _Base>::MF::DeepCopy( _MgmCl &dest, const _MgmCl *source )
{
	MF::Clear( dest );
	if( !source )
		return status::ok;
	dest = *source;
	return status::ok;
}

template<class _Ty, class _IdxTy, class _Base>
inline bool IndexedVector<_Ty, _IdxTy, _Base>::MF::Equals( const _MgmCl *lval, const _MgmCl *rval )
{
	// early out if the pointers are equal (includes nullptr)
	if( lval == rval )
		return true;

	// early out if one of the pointers is nullptr (both can't be null because of above test)
	if( !lval || !rval )
		return false;

	// deep compare the values
	const typename _Base &_lval = *lval;
	const typename _Base &_rval = *rval;
	return ( _lval == _rval );
}

template<class _Ty, class _IdxTy, class _Base>
inline status IndexedVector<_Ty, _IdxTy, _Base>::MF::Write( const _MgmCl &obj, EntityWriter &writer )
{
	const typename _Base &_obj = obj;
	return writer.Write( pdsKeyMacro( Values ), _obj );
}

template<class _Ty, class _IdxTy, class _Base>
inline status IndexedVector<_Ty, _IdxTy, _Base>::MF::Read( _MgmCl &obj, EntityReader &reader )
{
	typename _Base &_obj = obj;
	return reader.Read( pdsKeyMacro( Values ), _obj );
}

template<class _Ty, class _IdxTy, class _Base>
inline status IndexedVector<_Ty, _IdxTy, _Base>::MF::Validate( const _MgmCl &obj, EntityValidator &validator )
{
	const size_t max_allowed_index = (size_t)element_type_information<typename _IdxTy>::sup;

	if( obj.values().size() == 0 )
	{
		// values vector is empty
		if( obj.index().size() != 0 )
		{
			pdsValidationError( validation_error_flags::invalid_count ) 
				<< "The value vector of IndexedVector can only be empty if the index vector is also empty, and it is not."
				<< pdsValidationErrorEnd;
		}
	}
	else
	{
		// values vector has at least one value
		const size_t values_vector_max_index = obj.values().size()-1;
		const size_t max_index_value = std::min( values_vector_max_index, max_allowed_index );

		if( values_vector_max_index > max_allowed_index )
		{
			pdsValidationError( validation_error_flags::invalid_count ) 
				<< "This IndexedVector has too many values in the values vector."
				<< " The maximum supported value is:" << element_type_information<typename _IdxTy>::sup
				<< pdsValidationErrorEnd;
		}

		for( size_t i = 0; i < obj.index().size(); ++i )
		{
			if( (size_t)obj.index()[i] >= max_index_value )
			{
				pdsValidationError( validation_error_flags::invalid_value ) 
					<< "The value " << obj.index()[i] 
					<< " at position " << i 
					<< " of the index vector is out of bounds." 
					<< " Max allowed index: " << max_index_value
					<< pdsValidationErrorEnd;
			}
		}
	}

	return status::ok;
}

#include "../_pds_undef_macros.inl"

}
// namespace pds

#endif//__PDS__INDEXEDVECTOR_MF_H__
