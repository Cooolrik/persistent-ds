// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include "EntityManager.h"

#include <ctle/file_funcs.h>

namespace pds
{
#include "_pds_macros.inl"


static std::shared_ptr<Entity> entityNew( const std::vector<const EntityManager::PackageRecord *> &records, const char *entityTypeString )
{
	if( !entityTypeString )
	{
		ctLogError << "Invalid parameter, entityTypeString must be a pointer to a string" << ctLogEnd;
		return nullptr;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->New( entityTypeString );
		if( ret )
			return ret;
	}

	ctLogError << "Unrecognized entity, cannot allocate entity of type: " << entityTypeString << " is not registered with any package." << ctLogEnd;
	return nullptr;
}

static bool entityWrite( const std::vector<const EntityManager::PackageRecord *> &records, const Entity *obj, EntityWriter &writer )
{
	if( !obj )
	{
		ctLogError << "Invalid parameter, obj must be a pointer to an allocated object" << ctLogEnd;
		return false;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Write( obj, writer );
		if( ret )
			return ret;
	}

	ctLogError << "Unrecognized entity, " << obj->EntityTypeString() << " is not registered with any package." << ctLogEnd;
	return false;
}

static bool entityRead( const std::vector<const EntityManager::PackageRecord *> &records, Entity *obj, EntityReader &reader )
{
	if( !obj )
	{
		ctLogError << "Invalid parameter, obj must be a pointer to an allocated object" << ctLogEnd;
		return false;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Read( obj, reader );
		if( ret )
			return ret;
	}

	ctLogError << "Unrecognized entity, " << obj->EntityTypeString() << " is not registered with any package." << ctLogEnd;
	return false;
}

static bool entityValidate( const std::vector<const EntityManager::PackageRecord *> &records, const Entity *obj, EntityValidator &validator )
{
	if( !obj )
	{
		ctLogError << "Invalid parameter, obj must be a pointer to an allocated object" << ctLogEnd;
		return false;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Validate( obj, validator );
		if( ret )
			return ret;
	}

	ctLogError << "Unrecognized entity, " << obj->EntityTypeString() << " is not registered with any package." << ctLogEnd;
	return false;
}

void EntityManager::InsertEntity( const entity_ref &ref, const std::shared_ptr<const Entity> &entity )
{
	ctle::readers_writer_lock::write_guard guard( this->EntitiesLock );

	this->Entities.emplace( ref, entity );
}

status EntityManager::Initialize( const std::string &path, const std::vector<const PackageRecord *> &records )
{
	if( !this->Path.empty() )
	{
		return status::already_initialized;
	}
	if( records.empty() )
	{
		return status::invalid_param; // must have at least one record
	}

#ifdef _MSC_VER

	// make sure it is a directory 
	DWORD file_attributes = GetFileAttributesA( path.c_str() );
	if( ( file_attributes == INVALID_FILE_ATTRIBUTES )
		|| ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY )
	{
		ctLogError << "Invalid path: " << path << ctLogEnd;
		return status::invalid_param; // invalid path
	}

#endif
	this->Path = path;

	// copy the package records
	this->Records = records;

	return status::ok;
}

status EntityManager::ReadTask( EntityManager *pThis, const entity_ref ref )
{
	const uint hash_size = 32;

	// skip if entity already is loaded
	if( pThis->IsEntityLoaded( ref ) )
	{
		return status::ok;
	}

	// create the file name and path from the hash
	const std::string fileName = to_string( hash( ref ) ) + ".dat";
	const std::string filePath = pThis->Path + "/" + fileName;

	std::vector<u8> allocation;
	if( !ctle::read_file( filePath, allocation ) )
	{
		return status::cant_read;
	}

	// cant be less in size than the size of the hash at the end
	if( allocation.size() < hash_size )
	{
		return status::corrupted;
	}

	u8 *buffer = allocation.data();
	u64 total_size = allocation.size();

	// calculate the sha256 hash on the data, and make sure it compares correctly with the hash
	hash digest = {};
	ctle::calculate_sha256_hash( digest, buffer, total_size );
	if( digest != hash( ref ) )
	{
		// sha hash does not compare correctly, file is corrupted
		return status::corrupted;
	}

	// set up a memory stream and deserializer
	MemoryReadStream rstream( buffer, total_size, false );
	EntityReader reader( rstream );

	// read file header and deserialize the entity
	bool result = {};
	EntityReader *sectionReader;
	std::tie( sectionReader, result ) = reader.BeginReadSection( pdsKeyMacro( EntityFile ), false );
	if( !result )
		return status::corrupted;
	std::string entityTypeString;
	result = sectionReader->Read<std::string>( pdsKeyMacro( EntityType ), entityTypeString );
	if( !result )
		return status::corrupted;
	std::shared_ptr<Entity> entity = entityNew( pThis->Records, entityTypeString.c_str() );
	if( !entity )
		return status::not_initialized;
	result = entityRead( pThis->Records, entity.get(), *sectionReader );
	if( !result )
		return status::corrupted;
	result = reader.EndReadSection( sectionReader );
	if( !result )
		return status::corrupted;

	// transfer into the Entities map 
	pThis->InsertEntity( ref, entity );

	// done
	return status::ok;
}

std::future<status> EntityManager::LoadEntityAsync( const entity_ref &ref )
{
	return std::async( ReadTask, this, ref );
}

status EntityManager::LoadEntity( const entity_ref &ref )
{
	auto futr = this->LoadEntityAsync( ref );
	futr.wait();
	return futr.get();
}

status EntityManager::UnloadNonReferencedEntities()
{
	ctle::readers_writer_lock::write_guard guard( this->EntitiesLock );

	auto it = this->Entities.begin();
	while( it != this->Entities.end() )
	{
		// if this entity is only held by us, remove it, else skip to next
		if( it->second.use_count() == 1 )
		{
			it = this->Entities.erase( it );
		}
		else
		{
			++it;
		}
	}

	return status::ok;
}


bool EntityManager::IsEntityLoaded( const entity_ref &ref )
{
	ctle::readers_writer_lock::read_guard guard( this->EntitiesLock );

	return this->Entities.find( ref ) != this->Entities.end();
}

std::shared_ptr<const Entity> EntityManager::GetLoadedEntity( const entity_ref &ref )
{
	ctle::readers_writer_lock::read_guard guard( this->EntitiesLock );

	const auto it = this->Entities.find( ref );
	if( it == this->Entities.end() )
		return nullptr;

	return it->second;
}

std::pair<entity_ref, status> EntityManager::WriteTask( EntityManager *pThis, std::shared_ptr<const Entity> entity )
{
	EntityValidator validator;
	MemoryWriteStream wstream;
	EntityWriter writer( wstream );

	// make sure the entity is valid
	if( !entityValidate( pThis->Records, entity.get(), validator ) )
		return std::pair<entity_ref, status>( {}, status::corrupted );
	if( validator.GetErrorCount() > 0 )
		return std::pair<entity_ref, status>( {}, status::invalid );

	// serialize to a stream
	EntityWriter *sectionWriter = writer.BeginWriteSection( pdsKeyMacro( EntityFile ) );
	if( !sectionWriter )
		return std::pair<entity_ref, status>( {}, status::undefined_error );
	sectionWriter->Write<std::string>( pdsKeyMacro( EntityType ), entity->EntityTypeString() );
	if( !entityWrite( pThis->Records, entity.get(), *sectionWriter ) )
		return std::pair<entity_ref, status>( {}, status::undefined_error );
	if( !writer.EndWriteSection( sectionWriter ) )
		return std::pair<entity_ref, status>( {}, status::undefined_error );

	// calculate the sha256 hash on the data
	hash digest = {};
	ctle::calculate_sha256_hash( digest, (u8 *)wstream.GetData(), wstream.GetSize() );

	// get file data
	const u8 *writeBuffer = (u8 *)wstream.GetData();
	const u64 totalBytesToWrite = wstream.GetSize();

	// create the file name and path from the hash
	const std::string fileName = to_string( digest ) + ".dat";
	const std::string filePath = pThis->Path + "/" + fileName;

	// if the file does not exist, create and write it
	if( !ctle::file_exists( filePath ) )
	{
		if( !ctle::write_file( filePath, writeBuffer, (size_t)totalBytesToWrite, true ) )
		{
			return std::pair<entity_ref, status>( {}, status::cant_write );
		}
	}

	// transfer into the Entities map 
	pThis->InsertEntity( entity_ref( digest ), entity );

	// done
	return std::pair<entity_ref, status>( entity_ref( digest ), status::ok );
}

std::future<std::pair<entity_ref, status>> EntityManager::AddEntityAsync( const std::shared_ptr<const Entity> &entity )
{
	return std::async( WriteTask, this, entity );
}

std::pair<entity_ref, status> EntityManager::AddEntity( const std::shared_ptr<const Entity> &entity )
{
	auto futr = this->AddEntityAsync( entity );
	futr.wait();
	return futr.get();
}

#include "_pds_undef_macros.inl"
}
// namespace pds
