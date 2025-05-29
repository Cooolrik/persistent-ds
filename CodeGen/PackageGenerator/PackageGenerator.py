# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

import os
import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, float_bit_range, vector_dimension_range
from ctlepy.formatted_output import formatted_output, set_default_license_header_values
from EntitiesHelpers import * 
from pathlib import Path

from .ItemHeader import CreateItemHeader
from .ItemMFHeader import CreateItemMFHeader
from .ItemImplementation import CreateItemImplementation

from ctypes import c_ulonglong 
from ctypes import c_ubyte

release_version = None

def SetupReleaseVersion( package: Package , version_str:str ):

	global release_version

	# if version is "Latest", we need to find the latest version of the package
	# latest is the verion which does not have a later version pointing at it
	if version_str == "Latest":
		version_str = None
		for ver in package.Versions:	
			# for each version, check if any other version points at it
			hasLater = False
			for laterVer in package.Versions:	
				if laterVer == ver:
					continue
				if laterVer.PreviousVersion == ver:
					hasLater = True
					break
			if not hasLater:
				if version_str != None:
					print('Error: The package has "Latest" set as selected default version, but there are more than one leaf versions.')
					exit(1)
				version_str = ver.Name
				break

	# find the version object in the package
	release_version = next( (ver for ver in package.Versions if ver.Name == version_str) , None )
	if release_version == None:
		print('Error: The package has "Latest" set as selected default version, but no leaf version was found.')
		exit(1)

	# log info
	print(f'PackageGenerator: Using version "{release_version.Name}" as the release version of the package.')

	# list the versions in order from release to first version
	current_version = release_version.PreviousVersion
	while current_version != None:
		print(f'PackageGenerator: Found earlier version "{current_version.Name}" in the package.')
		current_version = current_version.PreviousVersion

	# for each item in the release version, find the version of the item to use as the release version
	# this is either the release version of the item, or the latest previous version of the item
	for item in release_version.Items:
		implementing_item = item.GetImplementingItem()
		if implementing_item != None:
			implementing_item.IsReleaseVersion = True
			print(f'PackageGenerator: Using {implementing_item.Version.Name} version of {item.Name} as release item version.')


# create a header for the package, which has all needed references and definitions
def CreatePackageHeader( package ):

	file_name = f'{package.Name}.h'

	op = formatted_output()
	op.generate_license_header()
	with op.header_guard( file_name=file_name, project_name=package.Name ):
		op.ln('#include <ctle/optional_value.h>')
		op.ln('#include <ctle/idx_vector.h>')
		op.ln('#include <ctle/optional_vector.h>')
		op.ln('#include <ctle/optional_idx_vector.h>')
		op.ln('#include <ctle/log.h>')
		op.ln()
		op.ln('#include <pds/fwd.h>')
		op.ln('#include <pds/item_ref.h>')		
		op.ln('#include <pds/entity_ref.h>')		
		op.ln('#include <pds/Entity.h>')
		op.ln()
		with op.ns(package.Name):
			op.comment_ln('imports from pds')
			op.ln('')

			# typedef base integer types
			op.comment_ln('scalar types')
			for bit_size in int_bit_range:
				op.ln(f'using pds::i{bit_size};')
			for bit_size in int_bit_range:
				op.ln(f'using pds::u{bit_size};')
			for bit_size in float_bit_range:
				op.ln(f'using pds::f{bit_size};')				
			op.ln('')
			op.ln('using pds::status;')
			op.ln('')
			op.comment_ln('ids, hashes and strings')
			op.ln('using pds::uuid;')
			op.ln('using pds::hash;')
			op.ln('using pds::string;')
			op.ln('')

			op.comment_ln('container types')
			op.ln('using pds::vector;')
			op.ln('using pds::idx_vector;')
			op.ln('using pds::optional_idx_vector;')
			op.ln('using pds::optional_value;')
			op.ln('using pds::optional_vector;')
			op.ln('')

			# typedef vector types
			op.comment_ln('vector types')
			for bit_size in int_bit_range:
				for vec_dim in vector_dimension_range:
					op.ln(f'using pds::i{bit_size}vec{vec_dim};')
			op.ln('')
			for bit_size in int_bit_range:
				for vec_dim in vector_dimension_range:
					op.ln(f'using pds::u{bit_size}vec{vec_dim};')
			op.ln('')
			for bit_size in float_bit_range:
				for vec_dim in vector_dimension_range:
					op.ln(f'using pds::f{bit_size}vec{vec_dim};')
			op.ln('')
			
			# typedef matrix types
			op.comment_ln('matrix types')
			for bit_size in float_bit_range:
				for vec_dim in vector_dimension_range:
					op.ln(f'using pds::f{bit_size}mat{vec_dim};')
			op.ln('')

			# typedef quaternions
			op.comment_ln('quaternion types')
			for bit_size in float_bit_range:
				op.ln(f'using pds::f{bit_size}quat;')
			op.ln('')

			# typedef standard types
			op.comment_ln('standard types from glm')
			for vec_dim in vector_dimension_range:
				op.ln(f'using pds::ivec{vec_dim};')
			for vec_dim in vector_dimension_range:
				op.ln(f'using pds::uvec{vec_dim};')
			for vec_dim in vector_dimension_range:
				op.ln(f'using pds::vec{vec_dim};')
			for vec_dim in vector_dimension_range:
				op.ln(f'using pds::mat{vec_dim};')
			op.ln('')

			# inline entity_ref and item_ref
			op.ln(f'using pds::entity_ref;')
			op.ln(f'using pds::item_ref;')
			op.ln('')

			# enum of all data types
			op.comment_ln('value type index enums')
			op.ln('using pds::element_type_index;')
			op.ln('')

			# standard PDS data classes
			op.comment_ln('data classes')
			op.ln('using pds::IndexedVector;')
			op.ln('using pds::ItemTable;')
			op.ln('using pds::Varying;')
			op.ln('using pds::DirectedGraph;')
			op.ln('using pds::BidirectionalMap;')
			op.ln('')

			# type information on all types
			#op.comment_ln('type information templates')
			#op.ln('template <class _Ty> using data_type_information = pds::data_type_information<_Ty>;')
			#op.ln('template <class _Ty> using combined_type_information = pds::combined_type_information<_Ty>;')

			#op.ln('const pds::EntityManager::PackageRecord *GetPackageRecord();')

	op.write_lines_to_file(f'{package.Path}/{file_name}')


def CreatePackageSourceFile( package: Package ):

	op = formatted_output()
	op.generate_license_header()
	op.ln()
	op.comment_ln('Forward declarations and imports from pds')
	op.ln(f'#include "{package.Name}.h"')
	op.ln('#include <pds/EntityWriter.h>')
	op.ln('#include <pds/EntityReader.h>')
	op.ln('#include <pds/WriteStream.h>')
	op.ln('#include <pds/ReadStream.h>')
	op.ln()

	for version in package.Versions:
		op.comment_ln(f'include version {version.Name}')
		for item in version.Items:
			if not item.IsDeleted:
				op.ln(f'#include "{version.Name}/{version.Name}_{item.Name}.h"')
		op.ln('')

	for version in package.Versions:
		op.comment_ln(f'include version {version.Name} source files (only non-aliased)')
		# include inl files for all new items in version
		for item in version.Items:
			if not item.IsDeleted and not item.IsIdenticalToPreviousVersion:
				op.ln(f'#include "{version.Name}/{version.Name}_{item.Name}.inl"')
		op.ln('')

	op.write_lines_to_file(f'{package.Path}/{package.Name}.cpp')

	# lines.append('// Include the package handler for this package')
	# lines.append('')
	# lines.append(f'#include "{packageName}PackageHandler.inl"')

	# hlp.write_lines_to_file(f"{package.Path}/{packageName}.cpp",lines)



# static and constant hash table for entity lookup, (must be larger than the number of entities to add)
# Fowler–Noll–Vo FNV-1a hash function  https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
class EntityHashTable:
	
	# select the next prime number above the item count as the table size
	def calc_table_size( self , item_count ):
		# start with item_count*2 (but minimum 20), then count up to the next prime
		table_size = max( item_count*2 , 20 )
		while True:
			is_prime = True
			for num in range(2, int(table_size**0.5) + 1):
				if table_size % num == 0:
					is_prime = False
					break
			if is_prime:
				return table_size
			table_size += 1

	def hash_function( self , entity_name ):
		hash = c_ulonglong(0xcbf29ce484222325)
		for ch in entity_name:
			hash = c_ulonglong(hash.value ^ c_ubyte(ord(ch)).value)
			hash = c_ulonglong(hash.value * c_ulonglong(0x00000100000001B3).value)
		return hash.value

	def insert_into_table( self , package_name , version_name , entity_name ):
		# use hash function to generate a good starting point
		hash_pos = self.hash_function( package_name + "." + version_name + "." + entity_name ) % self.hash_table_size
		# find first empty slot
		while( self.hash_table[hash_pos] != None ):
			hash_pos = hash_pos+1
			if( hash_pos >= self.hash_table_size ):
				hash_pos = 0
		# fill it
		self.hash_table[hash_pos] = version_name + "_" + entity_name

	def __init__(self, package):
		item_count = 0
		for version in package.Versions:
			item_count = len(version.Items)
		
		self.hash_table_size = self.calc_table_size( item_count ) 
		self.hash_data_type_mult = 109
		self.hash_container_type_mult = 991
		self.hash_table = [None] * self.hash_table_size

		# fill up hash table, only fill with entities
		for version in package.Versions:
			for item in version.Items:
				if item.IsEntity and not item.IdenticalToPreviousVersion and not item.IsDeleted:
					self.insert_into_table( package.Name , version.Name , item.Name );

def CreatePackageHandler_inl( package: Package ):
	packageName = package.Name

	lines = []
	lines.extend( hlp.generate_header() )
	lines.append('')
	lines.append('#include <pds/pds.h>')
	lines.append('#include <pds/Varying.h>')
	lines.append('')

	for version in package.Versions:
		for item in version.Items:
			if not item.IsDeleted and not item.IsIdenticalToPreviousVersion and item.IsEntity:
				lines.append(f'#include "{version.Name}/{version.Name}_{item.Name}.h"')

	lines.append('')
	lines.append('#include <pds/_pds_macros.inl>')
	lines.append('')
	lines.append(f'namespace {packageName}')
	lines.append('{')
	lines.append('namespace entity_types')
	lines.append('{')
	
	lines.append('    // dynamic allocation functors for items')
	lines.append('    class _entityTypeClass')
	lines.append('        {')
	lines.append('        public:')
	lines.append('            virtual const char *EntityTypeString() const = 0;')
	lines.append('            virtual std::shared_ptr<pds::Entity> New() const = 0;')
	lines.append('            virtual void Clear( pds::Entity *obj ) const = 0;')
	lines.append('            virtual bool Equals( const pds::Entity *lval , const pds::Entity *rval ) const = 0;')
	lines.append('            virtual bool Write( const pds::Entity *obj, pds::EntityWriter &writer ) const = 0;')
	lines.append('            virtual bool Read( pds::Entity *obj, pds::EntityReader &reader ) const = 0;')
	lines.append('            virtual bool Validate( const pds::Entity *obj, pds::EntityValidator &validator ) const = 0;')
	lines.append('        };')
	lines.append('')

	# add all (unique) entities of all versions
	for version in package.Versions:
		for item in version.Items:
			if item.IsEntity and not item.IsIdenticalToPreviousVersion and not item.IsDeleted:
				namespacedItemName = f'{version.Name}::{item.Name}'
				lines.append(f'    // {namespacedItemName}' )
				lines.append(f'    static const class _et_{version.Name}_{item.Name}_EntityType : public _entityTypeClass' )
				lines.append(f'        {{' )
				lines.append(f'        public:' )
				lines.append(f'            virtual const char *EntityTypeString() const {{ return {namespacedItemName}::ItemTypeString; }}' )
				lines.append(f'            virtual std::shared_ptr<pds::Entity> New() const {{ return std::make_shared<{namespacedItemName}>(); }}')
				lines.append(f'            virtual void Clear( pds::Entity *obj ) const {{ {namespacedItemName}::MF::Clear( *(({namespacedItemName}*)obj) ); }}')
				lines.append(f'            virtual bool Equals( const pds::Entity *lval , const pds::Entity *rval ) const {{ return {namespacedItemName}::MF::Equals( (({namespacedItemName}*)lval) , (({namespacedItemName}*)rval) ); }}')
				lines.append(f'            virtual bool Write( const pds::Entity *obj, pds::EntityWriter &writer ) const {{ return {namespacedItemName}::MF::Write( *(({namespacedItemName}*)obj) , writer ); }}')
				lines.append(f'            virtual bool Read( pds::Entity *obj, pds::EntityReader &reader ) const {{ return {namespacedItemName}::MF::Read( *(({namespacedItemName}*)obj) , reader ); }}')
				lines.append(f'            virtual bool Validate( const pds::Entity *obj, pds::EntityValidator &validator ) const {{ return {namespacedItemName}::MF::Validate( *(({namespacedItemName}*)obj) , validator ); }}')
				lines.append(f'        }} _et_{version.Name}_{item.Name}_EntityTypeObject;' )
				lines.append('')

	# allocate and print hash table
	hash_table = EntityHashTable( package )

	# print it 
	lines.append('    // Hash table with the type entity handler objects')
	lines.append(f'    static const _entityTypeClass *_entityTypeClassHashTable[{hash_table.hash_table_size}] = ')
	lines.append('        {')
	for row_start in range(0,hash_table.hash_table_size,10):
		row_str = ''
		row_end = min(row_start+10,hash_table.hash_table_size)
		for idx in range(row_start,row_end):
			if hash_table.hash_table[idx] == None:
				row_str += 'nullptr,'
			else:
				row_str += f'&_et_{hash_table.hash_table[idx]}_EntityTypeObject,'
		lines.append('        ' + row_str + f' // items {row_start} to {row_end-1}' )
	lines.append('        };')
	lines.append('')
	lines.append('    // hash table lookup of entityType')
	lines.append('    static const _entityTypeClass *_findEntityTypeClass( const char *typeNameString )')
	lines.append('        {')
	lines.append('        // calculate hash value using Fowler-Noll-Vo FNV-1a hash function')
	lines.append('        u64 hash = 0xcbf29ce484222325;')
	lines.append('        for( const char *chP = typeNameString ; *chP != \'\\0\' ; ++chP )')
	lines.append('            {')
	lines.append('            hash ^= (u8)(*chP);')
	lines.append('            hash *= (u64)(0x00000100000001B3);')
	lines.append('            }')
	lines.append('')
	lines.append('        // look for entry in table. ')
	lines.append(f'        u64 hashValue = hash % {hash_table.hash_table_size};')
	lines.append('        while( _entityTypeClassHashTable[hashValue] != nullptr )')
	lines.append('            {')
	lines.append('            if( strcmp( _entityTypeClassHashTable[hashValue]->EntityTypeString() , typeNameString ) == 0 )')
	lines.append('                return _entityTypeClassHashTable[hashValue];')
	lines.append('            ++hashValue;')
	lines.append(f'            if(hashValue >= {hash_table.hash_table_size})')
	lines.append('                hashValue = 0;')
	lines.append('            }')
	lines.append('')
	lines.append('        // entity was not found (this should never happen unless testing)')
	lines.append('        ctLogError << "_findEntityTypeClass: Invalid entity parameter { " << typeNameString << " } " << ctLogEnd;')
	lines.append('        return nullptr;')
	lines.append('        }')
	lines.append('')
	
	lines.append('}')
	lines.append(f'// namespace entity_types')	
 
	lines.append("""
	static const class CreatePackageHandler : public pds::EntityManager::PackageRecord
		{
		public:
			virtual std::shared_ptr<pds::Entity> New( const char *entityTypeString ) const
				{
				if( !entityTypeString )
					{
					ctLogError << "Invalid parameter, data must be name of entity type" << ctLogEnd;
					return nullptr;
					}
				const entity_types::_entityTypeClass *ta = entity_types::_findEntityTypeClass( entityTypeString );
				if( !ta )
					return nullptr;
				return ta->New();
				}
		
			virtual bool Write( const pds::Entity *obj, pds::EntityWriter &writer ) const
				{
				if( !obj )
					{
					ctLogError << "Invalid parameter, data must be a pointer to allocated object" << ctLogEnd;
					return false;
					}
				const entity_types::_entityTypeClass *ta = entity_types::_findEntityTypeClass( obj->EntityTypeString() );
				if( !ta )
					return false;
				return ta->Write( obj , writer );
				}
		
			virtual bool Read( pds::Entity *obj, pds::EntityReader &reader ) const
				{
				if( !obj )
					{
					ctLogError << "Invalid parameter, data must be a pointer to allocated object" << ctLogEnd;
					return false;
					}
				const entity_types::_entityTypeClass *ta = entity_types::_findEntityTypeClass( obj->EntityTypeString() );
				if( !ta )
					return false;
				return ta->Read( obj , reader );
				}
		
			virtual bool Validate( const pds::Entity *obj, pds::EntityValidator &validator ) const
				{
				if( !obj )
					{
					ctLogError << "Invalid parameter, data must be a pointer to allocated object" << ctLogEnd;
					return false;
					}
				const entity_types::_entityTypeClass *ta = entity_types::_findEntityTypeClass( obj->EntityTypeString() );
				if( !ta )
					return false;
				return ta->Validate( obj , validator );
				}
				
		} _createPackageHandlerObject;

	const pds::EntityManager::PackageRecord *GetPackageRecord() { return &_createPackageHandlerObject; }
	""")

	# end of namespace
	lines.append('}')
	lines.append(f'// namespace {packageName}')
	lines.append('')
	lines.append('#include <pds/_pds_undef_macros.inl>')
	hlp.write_lines_to_file(f"{package.Path}/{packageName}PackageHandler.inl",lines)

def run(package: Package, 
		version_str:str = 'Latest',
		project_name:str = 'Persistent Data Structure framework', 
		copyright_holder = '2022 Ulrik Lindahl', 
		license_type:str = 'MIT', 
		license_link:str = 'https://github.com/Cooolrik/pds/blob/main/LICENSE', 
		):
	
	set_default_license_header_values( project_name, copyright_holder, license_type, license_link )

	# set up the selected version of the package
	SetupReleaseVersion( package, version_str )

	# create folders for the package and all versions
	os.makedirs(package.Path, exist_ok=True)
	for version in package.Versions:
		os.makedirs(package.Path + '/' + version.Name , exist_ok=True)

	CreatePackageHeader( package )
	CreatePackageSourceFile( package )
	#CreatePackageHandler_inl( package )
	
	# generate all items in all version folders
	for version in package.Versions:
		for item in version.Items:
			if item.IsDeleted:
				continue
			CreateItemHeader( item=item )
			CreateItemMFHeader( item )
			CreateItemImplementation( item )
	
	#FindAndCreateDefaultVersionReferencesAndHeaders( package, version_str )



# 	# create the files for the selected version
# 	for item in version.Items:
# 		if not item.IsDeleted:

# 			op = formatted_output()
# 			op.generate_license_header()
# 			with op.header_guard( file_name=item.Name, project_name=package.Name ):
# 				op.ln('hje')
# 			op.write_lines_to_file(f"{package.Path}/{item.Name}.h")

# 			#lines = []
			
# 			## point at the latest implemented version of the entity
# 			#implementVersionName = version.Name
# 			#if item.IdenticalToPreviousVersion:
# 			#	implementVersionName = item.PreviousVersion.Version.Name
# #
# 			#lines.extend( hlp.generate_header() )
# 			#lines.append('')
# 			#lines.append('#pragma once')
# 			#lines.append('')
# 			#lines.append(f'#include "{implementVersionName}/{implementVersionName}_{item.Name}.h"')
# 			#lines.append(f'namespace {package.Name}')
# 			#lines.append('\t{')
# 			#lines.append(f'\tusing {item.Name} = {implementVersionName}::{item.Name};' )
# 			#lines.append('\t}')
# #
# 			#hlp.write_lines_to_file(f"{package.Path}/{item.Name}.h",lines)	
