// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include <future>
#include <ctle/readers_writer_lock.h>

#include "pds.h"

namespace pds
{


class EntityManager
{
public:
	class PackageRecord
	{
	public:
		// create a new writable entity of the named type
		virtual std::shared_ptr<Entity> New( const char *entityTypeString ) const = 0;

		// write an entity to a stream
		virtual bool Write( const Entity *obj, EntityWriter &writer ) const = 0;

		// read an entity from a stream
		virtual bool Read( Entity *obj, EntityReader &reader ) const = 0;

		// validate an entity
		virtual bool Validate( const Entity *obj, EntityValidator &validator ) const = 0;
	};

private:
	std::string Path;

	std::unordered_map<entity_ref, std::shared_ptr<const Entity>> Entities;
	ctle::readers_writer_lock EntitiesLock;
	std::vector<const PackageRecord *> Records;

	void InsertEntity( const entity_ref &ref, const std::shared_ptr<const Entity> &entity );

	static status ReadTask( EntityManager *pThis, const entity_ref ref );
	static std::pair<entity_ref, status> WriteTask( EntityManager *pThis, std::shared_ptr<const Entity> entity );

public:
	status Initialize( const std::string &path, const std::vector<const PackageRecord *> &records );

	// Asks the handler to load an entity and insert into the Entities map. 
	std::future<status> LoadEntityAsync( const entity_ref &ref );
	status LoadEntity( const entity_ref &ref );

	// Unloads all entities which are not referenced outside of the EntityHandler
	// To make sure an entity is kept around, keep a reference to the entity using the 
	// std::shared_ptr<const Entity> returned by GetLoadedEntity().
	status UnloadNonReferencedEntities();

	// Checks if an entity is loaded. 
	bool IsEntityLoaded( const entity_ref &ref );

	// Returns a loaded entity, or nullptr if the entity is not loaded.
	std::shared_ptr<const Entity> GetLoadedEntity( const entity_ref &ref );

	// Transfers ownership of a writable entity to the handler. The entity is serialized
	// and written to disk, and is from now on locked and immutable. 
	// The method returns the entity reference to the entity on return. 
	// Note! The ownership is transfered to the handler, and the entity data must be treated as
	// read-only.
	// Note! If the exact same entity data (same hash of the serialized data) is added, the 
	// existing reference will be returned and the status will be WAlreadyExists
	std::future<std::pair<entity_ref, status>> AddEntityAsync( const std::shared_ptr<const Entity> &entity );
	std::pair<entity_ref, status> AddEntity( const std::shared_ptr<const Entity> &entity );


};


}
// namespace pds
