// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include <ctle/file_funcs.h>
#include <ctle/hasher.h>
#include <ctle/log.h>

#include "Entity.h"

namespace pds
{
#include "_pds_macros.inl"

// calculate the hash value of the data, using the selected algo
static status_return<hash> calculateHash( const u8 *data, size_t count )
{
	hash ret;

#ifdef PDS_USE_SHA256
	// use sha256, cryptographically secure, but slower
	ctle::hasher_sha256 hasher;
#else
	// use hasher_2x_xxh128_dcb7be9cd0fcf505, which concatenates two 128 bit xxhash hashes into a 256bit hash, with a salt of 'dcb7be9cd0fcf505' on the second hash
	// caveat, not cryptographically secure, but much faster
	ctle::hasher_2x_xxh128_dcb7be9cd0fcf505 hasher;
#endif

	ctStatusCall(hasher.update( data, count ));
	ctStatusReturnCall( ret, hasher.finish() );

	return ret;
}

static status_return<std::shared_ptr<Entity>> entityNew( const std::vector<const EntityManager::PackageRecord *> &records, const char *entityTypeString )
{
	ctValidate( entityTypeString, status::invalid_param ) << "Invalid parameter, entityTypeString must be a pointer to a string" << ctValidateEnd;
	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->New( entityTypeString );
		if( ret.status() == status::not_found ) // not_found == not the correct package, continue looking
			continue;
		return ret; // correct package, return ok with pointer, or error 
	}

	ctLogError << "Unrecognized entity, '" << entityTypeString << "' is not registered with any package." << ctLogEnd;
	return status::not_found;
}

static status entityWrite( const std::vector<const EntityManager::PackageRecord *> &records, const Entity *obj, EntityWriter &writer )
{
	ctValidate( obj, status::invalid_param ) << "obj must be a pointer to an allocated object" << ctValidateEnd;

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Write( obj, writer );
		if( ret == status::not_found ) // not_found == not the correct package, continue looking
			continue;
		return ret; // correct package, return ok or error
	}

	ctLogError << "Unrecognized entity, '" << obj->EntityTypeString() << "' is not registered with any package." << ctLogEnd;
	return status::not_found;
}

static status entityRead( const std::vector<const EntityManager::PackageRecord *> &records, Entity *obj, EntityReader &reader )
{
	ctValidate( obj, status::invalid_param ) << "obj must be a pointer to an allocated object" << ctValidateEnd;

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Read( obj, reader );
		if( ret == status::not_found ) // not_found == not the correct package, continue looking
			continue;
		return ret; // correct package, return ok or error
	}

	ctLogError << "Unrecognized entity, '" << obj->EntityTypeString() << "' is not registered with any package." << ctLogEnd;
	return status::not_found;
}

static status entityValidate( const std::vector<const EntityManager::PackageRecord *> &records, const Entity *obj, EntityValidator &validator )
{
	ctValidate( obj, status::invalid_param ) << "obj must be a pointer to an allocated object" << ctValidateEnd;

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Validate( obj, validator );
		if( ret == status::not_found ) // not_found == not the correct package, continue looking
			continue;
		return ret; // correct package, return ok or error
	}

	ctLogError << "Unrecognized entity, '" << obj->EntityTypeString() << "' is not registered with any package." << ctLogEnd;
	return status::not_found;
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
	ctStatusAutoReturnCall( digest, calculateHash( buffer, total_size) );
	if( digest != hash( ref ) )
	{
		// sha hash does not compare correctly, file is corrupted
		return status::corrupted;
	}

	// set up a memory stream and deserializer
	ReadStream rstream( buffer, total_size );
	EntityReader reader( rstream );

	// read file header and deserialize the entity
	std::string entityTypeString;
	ctStatusAutoReturnCall( sectionReader, reader.BeginReadSection( pdsKeyMacro( EntityFile ), false ) )
	ctStatusCall( sectionReader->Read<std::string>( pdsKeyMacro( EntityType ), entityTypeString ) )
	ctStatusAutoReturnCall( entity, entityNew( pThis->Records, entityTypeString.c_str() ) );
	ctStatusCall( entityRead( pThis->Records, entity.get(), *sectionReader ) );
	ctStatusCall( reader.EndReadSection( sectionReader ) );

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

status_return<entity_ref> EntityManager::WriteTask( EntityManager *pThis, std::shared_ptr<const Entity> entity )
{
	EntityValidator validator;
	WriteStream wstream;
	EntityWriter writer( wstream );

	// make sure the entity is valid
	ctStatusCall(entityValidate( pThis->Records, entity.get(), validator ) );
	ctValidate( validator.GetErrorCount() == 0, status::invalid ) << "Validation failed on Entity before writing" << ctValidateEnd;

	// serialize to a stream
	ctStatusAutoReturnCall( sectionWriter, writer.BeginWriteSection( pdsKeyMacro( EntityFile ) ) );
	ctStatusCall( sectionWriter->Write<std::string>( pdsKeyMacro( EntityType ), entity->EntityTypeString() ) );
	ctStatusCall( entityWrite( pThis->Records, entity.get(), *sectionWriter ) );
	ctStatusCall( writer.EndWriteSection( sectionWriter ) );

	// get file data
	const u8 *writeBuffer = (u8 *)wstream.GetData();
	const u64 totalBytesToWrite = wstream.GetSize();

	// calculate the hash on the data
	ctStatusAutoReturnCall( digest, calculateHash( writeBuffer, totalBytesToWrite ) );
	
	// create the file name and path from the hash
	const std::string fileName = to_string( digest ) + ".dat";
	const std::string filePath = pThis->Path + "/" + fileName;

	// if the file does not exist, create and write it
	if( !ctle::file_exists( filePath ) )
		ctStatusCall( ctle::write_file( filePath, writeBuffer, (size_t)totalBytesToWrite, true ) );

	// transfer into the Entities map 
	pThis->InsertEntity( entity_ref( digest ), entity );

	// done
	return entity_ref( digest );
}

std::future<status_return<entity_ref>> EntityManager::AddEntityAsync( const std::shared_ptr<const Entity> &entity )
{
	return std::async( WriteTask, this, entity );
}

status_return<entity_ref> EntityManager::AddEntity( const std::shared_ptr<const Entity> &entity )
{
	auto futr = this->AddEntityAsync( entity );
	futr.wait();
	return futr.get();
}

#include "_pds_undef_macros.inl"
}
// namespace pds
