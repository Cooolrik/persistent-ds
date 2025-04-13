// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__PDS_H__
#define __PDS__PDS_H__

#include "fwd.h"

#ifdef PDS_IMPLEMENTATION

#include <ctle/log.h>

#include "element_types.h"
#include "value_types.h"
#include "element_value_ptrs.h"

#include "WriteStream.h"
#include "ReadStream.h"

#include "EntityWriter.h"
#include "EntityReader.h"
//#include "EntityValidator.h"
//#include "EntityManager.h"

#include "dynamic_types.h"



//
//#include "Varying_MF.h"
//#include "BidirectionalMap_MF.h"
//#include "DirectedGraph_MF.h"
//#include "IndexedVector_MF.h"
//#include "ItemTable_MF.h"


//#include "Varying.inl"
//#include "Varying_MF.inl"

#endif//PDS_IMPLEMENTATION

#endif//__PDS__PDS_H__