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


Varying::Varying( const Varying &rval )
{
	this->type_m = {};
	this->container_type_m = {};
	this->data_m = {};

	Varying::MF::DeepCopy( *this, &rval );
}

Varying &Varying::operator=( const Varying &rval )
{
	Varying::MF::DeepCopy( *this, &rval );
	return *this;
}

Varying::Varying( Varying &&rval ) noexcept
{
	this->type_m = rval.type_m;
	rval.type_m = {};

	this->container_type_m = rval.container_type_m;
	rval.container_type_m = {};

	this->data_m = rval.data_m;
	rval.data_m = {};
}

Varying &Varying::operator=( Varying &&rval ) noexcept
{
	this->type_m = rval.type_m;
	rval.type_m = {};

	this->container_type_m = rval.container_type_m;
	rval.container_type_m = {};

	this->data_m = rval.data_m;
	rval.data_m = {};

	return *this;
}

Varying::~Varying()
{
	this->Deinitialize();
}

bool Varying::operator==( const Varying &rval ) const
{
	return MF::Equals( this, &rval );
};

bool Varying::operator!=( const Varying &rval ) const
{
	return !( operator==( rval ) );
}

bool Varying::Deinitialize()
{
	// delete allocated data if nonempty
	if( this->IsInitialized() )
	{
		bool success = dynamic_types::delete_type( this->type_m, this->container_type_m, this->data_m );
		if( !success )
		{
			ctLogError << "Error in call to dynamic_types::delete_type" << ctLogEnd;
			return false;
		}

		// clear values
		this->type_m = {};
		this->container_type_m = {};
		this->data_m = {};
	}

	return true;
}

bool Varying::Initialize( data_type_index dataType, container_type_index containerType )
{
	return MF::SetType( *this, dataType, containerType );
}

std::tuple<data_type_index, container_type_index> Varying::Type() const noexcept
{
	return std::pair<data_type_index, container_type_index>( this->type_m, this->container_type_m );
}

// Returns true if the object has been initialized (ie has a data object)
bool Varying::IsInitialized() const noexcept
{
	return this->data_m != nullptr;
}


#include "_pds_undef_macros.inl"
}
// namespace pds