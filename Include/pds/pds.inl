// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#define PDS_MAIN_BUILD_FILE

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Rpc.h>
#endif

#include "SHA256.h"
#include "DynamicTypes.h"
#include "ValueTypes.h"
#include "Varying.h"

#include "MemoryWriteStream.h"
#include "MemoryReadStream.h"

#include "EntityWriter.h"
#include "EntityReader.h"
#include "EntityValidator.h"

#include "BidirectionalMap.h"
#include "DirectedGraph.h"
#include "IndexedVector.h"
#include "ItemTable.h"

#include "ElementValuePointers.h"

#include "ElementTypes.inl"
#include "EntityWriter.inl"
#include "EntityReader.inl"
#include "DynamicTypes.inl"

#include <iostream>
#include <fstream>

#include <ctle/file_funcs.h>

using std::pair;
using std::make_pair;

constexpr size_t sha256_hash_size = 32;

namespace pds
{
item_ref item_ref::make_ref()
{
	return item_ref::from_uuid( uuid::generate() );
}

std::string value_to_hex_string( hash value )
{
	static_assert( sizeof( hash ) == 32, "Error: hash is assumed to be of size 32." );
	// note: no need to swap order of bytes. 
	// The hash is always ordered the same, regardless of the hardware (basically a big-endian 256 bit value)
	return bytes_to_hex_string( &value, 32 );
}

template <> hash hex_string_to_value<hash>( const char *hex_string )
{
	static_assert( sizeof( hash ) == 32, "Error: hash is assumed to be of size 32." );
	hash value;
	hex_string_to_bytes( &value, hex_string, 32 );
	// note: no need to swap order of bytes. 
	// The hash is always ordered the same, regardless of the hardware (basically a big-endian 256 bit value)
	return value;
}

static std::shared_ptr<Entity> entityNew( const std::vector<const EntityHandler::PackageRecord *> &records, const char *entityTypeString )
{
	if( !entityTypeString )
	{
		pdsErrorLog << "Invalid parameter, entityTypeString must be a pointer to a string" << pdsErrorLogEnd;
		return nullptr;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->New( entityTypeString );
		if( ret )
			return ret;
	}

	pdsErrorLog << "Unrecognized entity, cannot allocate entity of type: " << entityTypeString << " is not registered with any package." << pdsErrorLogEnd;
	return nullptr;
}

static bool entityWrite( const std::vector<const EntityHandler::PackageRecord *> &records, const Entity *obj, EntityWriter &writer )
{
	if( !obj )
	{
		pdsErrorLog << "Invalid parameter, obj must be a pointer to an allocated object" << pdsErrorLogEnd;
		return false;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Write( obj, writer );
		if( ret )
			return ret;
	}

	pdsErrorLog << "Unrecognized entity, " << obj->EntityTypeString() << " is not registered with any package." << pdsErrorLogEnd;
	return false;
}

static bool entityRead( const std::vector<const EntityHandler::PackageRecord *> &records, Entity *obj, EntityReader &reader )
{
	if( !obj )
	{
		pdsErrorLog << "Invalid parameter, obj must be a pointer to an allocated object" << pdsErrorLogEnd;
		return false;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Read( obj, reader );
		if( ret )
			return ret;
	}

	pdsErrorLog << "Unrecognized entity, " << obj->EntityTypeString() << " is not registered with any package." << pdsErrorLogEnd;
	return false;
}

static bool entityValidate( const std::vector<const EntityHandler::PackageRecord *> &records, const Entity *obj, EntityValidator &validator )
{
	if( !obj )
	{
		pdsErrorLog << "Invalid parameter, obj must be a pointer to an allocated object" << pdsErrorLogEnd;
		return false;
	}

	for( size_t i = 0; i < records.size(); ++i )
	{
		auto ret = records[i]->Validate( obj, validator );
		if( ret )
			return ret;
	}

	pdsErrorLog << "Unrecognized entity, " << obj->EntityTypeString() << " is not registered with any package." << pdsErrorLogEnd;
	return false;
}

void EntityHandler::InsertEntity( const entity_ref &ref, const std::shared_ptr<const Entity> &entity )
{
	ctle::readers_writer_lock::write_guard guard( this->EntitiesLock );

	this->Entities.emplace( ref, entity );
}

Status EntityHandler::Initialize( const std::string &path, const std::vector<const PackageRecord *> &records )
{
	if( !this->Path.empty() )
	{
		return Status::EAlreadyInitialized;
	}
	if( records.empty() )
	{
		return Status::EParam; // must have at least one record
	}

#ifdef _MSC_VER

	// make sure it is a directory 
	DWORD file_attributes = GetFileAttributesA( path.c_str() );
	if( ( file_attributes == INVALID_FILE_ATTRIBUTES )
		|| ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY )
	{
		pdsErrorLog << "Invalid path: " << path << pdsErrorLogEnd;
		return Status::EParam; // invalid path
	}

#endif
	this->Path = path;

	// copy the package records
	this->Records = records;

	return Status::Ok;
}

Status EntityHandler::ReadTask( EntityHandler *pThis, const entity_ref ref )
{
	const uint hash_size = 32;

	// skip if entity already is loaded
	if( pThis->IsEntityLoaded( ref ) )
	{
		return Status::Ok;
	}

	// create the file name and path from the hash
	const std::string fileName = value_to_hex_string( hash( ref ) ) + ".dat";
	const std::string filePath = pThis->Path + "/" + fileName;

	std::vector<u8> allocation;
	if( !ctle::read_file( filePath, allocation ) )
	{
		return Status::ECantRead;
	}

	// cant be less in size than the size of the hash at the end
	if( allocation.size() < hash_size )
	{
		return Status::ECorrupted;
	}

	u8 *buffer = allocation.data();
	u64 total_size = allocation.size();

	// calculate the sha256 hash on the data, and make sure it compares correctly with the hash
	hash digest = {};
	SHA256::CalculateHash( digest, buffer, total_size );
	if( digest != hash( ref ) )
	{
		// sha hash does not compare correctly, file is corrupted
		return Status::ECorrupted;
	}

	// set up a memory stream and deserializer
	MemoryReadStream rstream( buffer, total_size, false );
	EntityReader reader( rstream );

	// read file header and deserialize the entity
	bool result = {};
	EntityReader *sectionReader;
	std::tie( sectionReader, result ) = reader.BeginReadSection( pdsKeyMacro( "EntityFile" ), false );
	if( !result )
		return Status::ECorrupted;
	std::string entityTypeString;
	result = sectionReader->Read<std::string>( pdsKeyMacro( "EntityType" ), entityTypeString );
	if( !result )
		return Status::ECorrupted;
	std::shared_ptr<Entity> entity = entityNew( pThis->Records, entityTypeString.c_str() );
	if( !entity )
		return Status::ENotInitialized;
	result = entityRead( pThis->Records, entity.get(), *sectionReader );
	if( !result )
		return Status::ECorrupted;
	result = reader.EndReadSection( sectionReader );
	if( !result )
		return Status::ECorrupted;

	// transfer into the Entities map 
	pThis->InsertEntity( ref, entity );

	// done
	return Status::Ok;
}

std::future<Status> EntityHandler::LoadEntityAsync( const entity_ref &ref )
{
	return std::async( ReadTask, this, ref );
}

Status EntityHandler::LoadEntity( const entity_ref &ref )
{
	auto futr = this->LoadEntityAsync( ref );
	futr.wait();
	return futr.get();
}

Status EntityHandler::UnloadNonReferencedEntities()
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

	return Status::Ok;
}


bool EntityHandler::IsEntityLoaded( const entity_ref &ref )
{
	ctle::readers_writer_lock::read_guard guard( this->EntitiesLock );

	return this->Entities.find( ref ) != this->Entities.end();
}

std::shared_ptr<const Entity> EntityHandler::GetLoadedEntity( const entity_ref &ref )
{
	ctle::readers_writer_lock::read_guard guard( this->EntitiesLock );

	const auto it = this->Entities.find( ref );
	if( it == this->Entities.end() )
		return nullptr;

	return it->second;
}

std::pair<entity_ref, Status> EntityHandler::WriteTask( EntityHandler *pThis, std::shared_ptr<const Entity> entity )
{
	EntityValidator validator;
	MemoryWriteStream wstream;
	EntityWriter writer( wstream );

	// make sure the entity is valid
	if( !entityValidate( pThis->Records, entity.get(), validator ) )
		return std::pair<entity_ref, Status>( {}, Status::ECorrupted );
	if( validator.GetErrorCount() > 0 )
		return std::pair<entity_ref, Status>( {}, Status::EInvalid );

	// serialize to a stream
	EntityWriter *sectionWriter = writer.BeginWriteSection( pdsKeyMacro( "EntityFile" ) );
	if( !sectionWriter )
		return std::pair<entity_ref, Status>( {}, Status::EUndefined );
	sectionWriter->Write<std::string>( pdsKeyMacro( "EntityType" ), entity->EntityTypeString() );
	if( !entityWrite( pThis->Records, entity.get(), *sectionWriter ) )
		return std::pair<entity_ref, Status>( {}, Status::EUndefined );
	if( !writer.EndWriteSection( sectionWriter ) )
		return std::pair<entity_ref, Status>( {}, Status::EUndefined );

	// calculate the sha256 hash on the data
	hash digest = {};
	SHA256::CalculateHash( digest, (u8 *)wstream.GetData(), wstream.GetSize() );

	// get file data
	const u8 *writeBuffer = (u8 *)wstream.GetData();
	const u64 totalBytesToWrite = wstream.GetSize();

	// create the file name and path from the hash
	const std::string fileName = value_to_hex_string( digest ) + ".dat";
	const std::string filePath = pThis->Path + "/" + fileName;

	// if the file does not exist, create and write it
	if( !ctle::file_exists( filePath ) )
	{
		if( !ctle::write_file( filePath, writeBuffer, (size_t)totalBytesToWrite, true ) )
		{
			return std::pair<entity_ref, Status>( {}, Status::ECantWrite );
		}
	}

	// transfer into the Entities map 
	pThis->InsertEntity( entity_ref( digest ), entity );

	// done
	return std::pair<entity_ref, Status>( entity_ref( digest ), Status::Ok );
}

std::future<std::pair<entity_ref, Status>> EntityHandler::AddEntityAsync( const std::shared_ptr<const Entity> &entity )
{
	return std::async( WriteTask, this, entity );
}

std::pair<entity_ref, Status> EntityHandler::AddEntity( const std::shared_ptr<const Entity> &entity )
{
	auto futr = this->AddEntityAsync( entity );
	futr.wait();
	return futr.get();
}

}

