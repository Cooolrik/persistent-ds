// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__PDS_H__
#define __PDS__PDS_H__

#include <ctle/status.h>

#include "element_types.h"

namespace pds
{
class EntityValidator;
class EntityWriter;
class EntityReader;
class EntityManager;
class MemoryReadStream;
class MemoryWriteStream;

}
// namespace pds

//#include "Entity.h"

#ifdef PDS_IMPLEMENTATION

#include <ctle/log.h>

//#include "DynamicTypes.h"
#include "value_types.h"
#include "element_value_ptrs.h"

//#include "EntityWriter.h"
//#include "EntityReader.h"
//#include "EntityValidator.h"
//
//#include "Varying_MF.h"
//#include "BidirectionalMap_MF.h"
//#include "DirectedGraph_MF.h"
//#include "IndexedVector_MF.h"
//#include "ItemTable_MF.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include "WriteStream.h"
#include "ReadStream.h"

//#include "element_types.inl"
//#include "ValueTypes.inl"
//#include "EntityWriter.inl"
//#include "EntityReader.inl"
//#include "DynamicTypes.inl"
//#include "MemoryWriteStream.inl"
//#include "MemoryReadStream.inl"
//#include "EntityManager.inl"
//#include "Varying.inl"
//#include "Varying_MF.inl"

#endif//PDS_IMPLEMENTATION

#endif//__PDS__PDS_H__