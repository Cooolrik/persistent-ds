// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "Varying.h"

namespace pds
{


class Varying::MF
{
public:
	static status Clear( Varying &obj );
	static status DeepCopy( Varying &dest, const Varying *source );
	static bool Equals( const Varying *lvar, const Varying *rvar );

	static status Write( const Varying &obj, EntityWriter &writer );
	static status Read( Varying &obj, EntityReader &reader );

	static status Validate( const Varying &obj, EntityValidator &validator );

	// Method to set the type of the data in the varying object, either using a parameter, or as a template method
	static status SetType( Varying &obj, data_type_index dataType, container_type_index containerType );
	template <class _Ty> static status SetType( Varying &obj )
	{
		return SetType( obj, combined_type_information<_Ty>::type_index, combined_type_information<_Ty>::container_index );
	}
};



}
// namespace pds
