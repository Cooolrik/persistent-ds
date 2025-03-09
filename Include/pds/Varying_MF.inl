// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include <pds/EntityWriter.h>
#include <pds/EntityReader.h>
#include <pds/EntityValidator.h>

#include "Varying_MF.h"

namespace pds
{
#include "_pds_macros.inl"


status Varying::MF::Clear( Varying &obj )
{
	if( obj.data_m == nullptr )
		return status::ok;

	bool success = dynamic_types::clear( obj.type_m, obj.container_type_m, obj.data_m );
	ctValidate( success, status::corrupted ) << "Could not clear the dynamic type" << ctValidateEnd;

	return status::ok;
}

status Varying::MF::DeepCopy( Varying &dest, const Varying *source )
{
	bool success = {};

	// clear the current type of dest
	success = dest.Deinitialize();
	ctValidate( success, status::corrupted ) << "Could not clear Varying type" << ctValidateEnd;

	// if source is nullptr or empty we are done
	if( !source || !source->IsInitialized() )
		return status::ok;

	// set type and clear any currently allocated data
	success = SetType( dest, source->type_m, source->container_type_m );
	ctValidate( success, status::corrupted ) << "Cannot set Varying type." << ctValidateEnd;

	// copy the data from source to dest
	success = dynamic_types::copy( dest.type_m, dest.container_type_m, dest.data_m, source->data_m );
	ctValidate( success, status::corrupted ) << "Cannot copy Varying type data." << ctValidateEnd;

	return status::ok;
}

bool Varying::MF::Equals( const Varying *lvar, const Varying *rvar )
{
	// early out if the pointers are equal (includes nullptr)
	if( lvar == rvar )
		return true;

	// early out if one of the pointers is nullptr (both can't be null because of above test)
	if( !lvar || !rvar )
		return false;

	// both pointers are valid and different pointers, compare type data
	if( lvar->type_m != rvar->type_m )
		return false;
	if( lvar->container_type_m != rvar->container_type_m )
		return false;

	// types match, compare data in type
	return dynamic_types::equals( lvar->type_m, lvar->container_type_m, lvar->data_m, rvar->data_m );
}

status Varying::MF::Write( const Varying &obj, EntityWriter &writer )
{
	ctValidate( obj.IsInitialized(), status::not_initialized ) << "Cannot write uninitialized Varying to stream. Use optional_value template for optional Varying data." << ctValidateEnd;

	// store the data type index
	if( !writer.Write( pdsKeyMacro( Type ), (u16)obj.type_m ) )
		return status::cant_write;

	// store the container type index
	if( !writer.Write( pdsKeyMacro( ContainerType ), (u16)obj.container_type_m ) )
		return status::cant_write;

	// store the data
	if( !dynamic_types::write( obj.type_m, obj.container_type_m, pdsKeyMacro( Data ), writer, obj.data_m ) )
		return status::cant_write;

	return status::ok;
}

status Varying::MF::Read( Varying &obj, EntityReader &reader )
{
	// if initialized, deinitalize
	ctValidate( obj.Deinitialize(), status::corrupted ) << "Cannot Deinitialize varying data object, it is corrupted." << ctValidateEnd;

	u16 type_u16 = {};
	u16 container_type_u16 = {};

	// load the data type index
	if( !reader.Read( pdsKeyMacro( Type ), type_u16 ) )
		return status::cant_read;

	// load the container type index
	if( !reader.Read( pdsKeyMacro( ContainerType ), container_type_u16 ) )
		return status::cant_read;

	// set the data type
	if( !SetType( obj, (data_type_index)type_u16, (container_type_index)container_type_u16 ) )
		return status::cant_read;

	// store the data
	if( !dynamic_types::read( obj.type_m, obj.container_type_m, pdsKeyMacro( Data ), reader, obj.data_m ) )
		return status::cant_read;

	return status::ok;
}

status Varying::MF::Validate( const Varying &obj, EntityValidator &validator )
{
	if( !obj.IsInitialized() )
	{
		pdsValidationError( ValidationError::NullNotAllowed )
			<< "Object is not initialized, and does not have a type set. All Varying objects need to be initialized to be valid. To have an optional Varying object, use the optional_value template."
			<< pdsValidationErrorEnd;
	}

	return status::ok;
}

status Varying::MF::SetType( Varying &obj, data_type_index dataType, container_type_index containerType )
{
	bool success = {};

	// clear current type if it is set
	ctValidate( obj.Deinitialize(), status::corrupted ) << "Cannot Deinitialize varying data object, it is corrupted." << ctValidateEnd;

	// set type and allocate the data
	obj.type_m = dataType;
	obj.container_type_m = containerType;
	std::tie( obj.data_m, success ) = dynamic_types::new_type( obj.type_m, obj.container_type_m );
	ctValidate( success, status::not_initialized ) << "Could not set type of varying object" << ctValidateEnd;

	return status::ok;
}


#include "_pds_undef_macros.inl"
}
// namespace pds