// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include "value_types.h"

#include <ctle/status_error.h>
#include <ctle/log.h>

namespace pds
{
#include "_pds_macros.inl"

template <class _Ty> _Ty &Varying::Initialize()
{
	ctStatusCallThrow( this->Initialize( combined_type_information<_Ty>::type_index, combined_type_information<_Ty>::container_index ) )
	return *( (_Ty *)data_m );
}

// Check if the data is of the template class type _Ty
template<class _Ty> bool Varying::IsA() const noexcept
{
	return this->type_m == value_type_information<_Ty>::type_index
		&& this->container_type_m == value_type_information<_Ty>::container_index;
}

// Retreive a reference to the data in the object
template<class _Ty> const _Ty &Varying::Data() const
{
	ctValidate( this->IsA<_Ty>() , status::not_initialized ) << "Trying to dereferece a non-initialized object" << ctValidateThrow;
	return *( (const _Ty *)data_m );
};
template<class _Ty> _Ty &Varying::Data()
{
	return const_cast<_Ty&>( static_cast<const Varying *>(this)->Data<_Ty>() );
};

#include "_pds_undef_macros.inl"
}
// namespace pds

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Implementation section

#ifdef PDS_IMPLEMENTATION

#include "dynamic_types.h"

#include "mf/Varying_MF.h"

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

status Varying::Initialize( element_type_index elementType, container_type_index containerType )
{
	return MF::SetType( *this, elementType, containerType );
}

std::tuple<element_type_index, container_type_index> Varying::Type() const noexcept
{
	return std::pair<element_type_index, container_type_index>( this->type_m, this->container_type_m );
}

// Returns true if the object has been initialized (ie has a data object)
bool Varying::IsInitialized() const noexcept
{
	return this->data_m != nullptr;
}

#include "_pds_undef_macros.inl"
}
// namespace pds

#endif//PDS_IMPLEMENTATION