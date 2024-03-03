// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "pds.h"

namespace pds
{

// Entity is base for all entities (atomic objects in the graph, which ows all values within the object)
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
