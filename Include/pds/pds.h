// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include <ctle/status.h>

#include "ElementTypes.h"

namespace pds
{
class EntityValidator;
class EntityWriter;
class EntityReader;
class EntityManager;
class MemoryReadStream;
class MemoryWriteStream;

using uint = uint32_t;
using ctle::status;
}
// namespace pds

#include "Entity.h"

#ifdef PDS_IMPLEMENTATION

#include <ctle/log.h>

#include "Enums.h"
#include "DynamicTypes.h"
#include "ValueTypes.h"
#include "ElementValuePointers.h"

#include "MemoryWriteStream.h"
#include "MemoryReadStream.h"

#include "EntityWriter.h"
#include "EntityReader.h"
#include "EntityValidator.h"

#include "Varying_MF.h"
#include "BidirectionalMap_MF.h"
#include "DirectedGraph_MF.h"
#include "IndexedVector_MF.h"
#include "ItemTable_MF.h"

namespace pds
{

// maximum size of a name of a value of subchunk in the entities
const size_t EntityMaxKeyLength = 40;

item_ref item_ref::make_ref()
{
	return item_ref::from_uuid( uuid::generate() );
}

}
// namespace pds

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include "ElementTypes.inl"
#include "ValueTypes.inl"
#include "EntityWriter.inl"
#include "EntityReader.inl"
#include "DynamicTypes.inl"
#include "MemoryWriteStream.inl"
#include "MemoryReadStream.inl"
#include "EntityManager.inl"
#include "Varying.inl"
#include "Varying_MF.inl"

#endif