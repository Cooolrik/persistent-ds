// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__ENTITY_H__
#define __PDS__ENTITY_H__

#include "pds.h"

namespace pds
{

// Entity is base for all entities (atomic objects, which owns all values within the object, and is serialized in one go)
class Entity
{
public:
	Entity() = default;
	Entity( const Entity &other ) = default;
	Entity &operator=( const Entity &other ) = default;
	Entity( Entity &&other ) = default;
	Entity &operator=( Entity &&other ) = default;
	virtual ~Entity() = default;

	virtual const char *EntityTypeString() const = 0;
};

}
// namespace pds
#endif//__PDS__ENTITY_H__
