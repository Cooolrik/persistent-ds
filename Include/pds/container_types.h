// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once
#ifndef __PDS__CONTAINER_TYPES_H__
#define __PDS__CONTAINER_TYPES_H__

// container_types.h - information of the container types supported by pds

#include "fwd.h"

#include <ctle/idx_vector.h>
#include <ctle/optional_value.h>
#include <ctle/optional_vector.h>
#include <ctle/optional_idx_vector.h>

namespace pds
{

// enumeration of container types
enum class container_type_index : uint
{
	ct_none = 0x0,
	ct_optional_value = 0x1,
	ct_vector = 0x10,
	ct_optional_vector = 0x11,
	ct_idx_vector = 0x20,
	ct_optional_idx_vector = 0x21,
};

}
// namespace pds

#endif//__PDS__CONTAINER_TYPES_H__