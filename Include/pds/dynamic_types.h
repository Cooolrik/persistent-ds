// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__DYNAMIC_TYPES_H__
#define __PDS__DYNAMIC_TYPES_H__

#include "fwd.h"

namespace pds
{
namespace dynamic_types
{

// dynamically allocate a data of data type and container combination
status_return<void*> new_type( element_type_index dataType, container_type_index containerType );

// delete a previously allocated data object
// Warning: Input data is only checked for nullptr, so make sure to supply the correct type combo to the function.
status delete_type( element_type_index dataType, container_type_index containerType, void *data );

// clear the contents for data object.
// Warning: Input data is only checked for nullptr, so make sure to supply the correct type combo to the function.
status clear( element_type_index dataType, container_type_index containerType, void *data );

// write the data to an entity writer stream.
// Warning: Input data is only checked for nullptr, so make sure to supply the correct type combo to the function.
status write( element_type_index dataType, container_type_index containerType, const char *key, const u8 key_length, EntityWriter &writer, const void *data );

// read the data to from an entity reader stream.
// Warning: Input data is only checked for nullptr, so make sure to supply the correct type combo to the function.
status read( element_type_index dataType, container_type_index containerType, const char *key, const u8 key_length, EntityReader &reader, void *data );

// copy data from src to dest. both data object must be the same type and be allocated.
// Warning: Input data is only checked for nullptr, so make sure to supply the correct type combo to the function.
status copy( element_type_index dataType, container_type_index containerType, void *dest, const void *src );

// check if two data objects have the same internal data
// Warning: Input data is only checked for nullptr, so make sure to supply the correct type combo to the function.
status_return<bool> equals( element_type_index dataType, container_type_index containerType, const void *dataA, const void *dataB );

}
// namespace dynamic_types
}
// namespace pds

#ifdef PDS_IMPLEMENTATION
#include "dynamic_types.inl"
#endif//PDS_IMPLEMENTATION

#endif//__PDS__DYNAMIC_TYPES_H__