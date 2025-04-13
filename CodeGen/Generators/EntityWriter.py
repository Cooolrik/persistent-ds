# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

import CodeGeneratorHelpers as hlp

def EntityWriter_h():
	lines = []
	lines.extend( hlp.begin_header_file('EntityWriter.h') )
	lines.append('')
	lines.append('#pragma once')
	lines.append('')
	lines.append('#include "fwd.h"')
	lines.append('')
	lines.append('namespace pds')
	lines.append('{')
	lines.append('')
	lines.append('class EntityWriter')
	lines.append('{')
	lines.append('private:')
	lines.append('\tWriteStream &dstream;')
	lines.append('\tconst u64 start_position;')
	lines.append('')
	lines.append('\tstd::unique_ptr<EntityWriter> active_subsection;')
	lines.append('')
	lines.append('\tsize_t active_array_size = 0;')
	lines.append('\tsize_t active_array_index = size_t(~0);')
	lines.append('\tu64 active_array_index_start_position = 0;')
	lines.append('')
	lines.append('public:')
	lines.append('\tEntityWriter( WriteStream &_dstream );')
	lines.append('\t~EntityWriter();')
	lines.append('')
	lines.append('\t// Build a section. ')
	lines.append('\tstatus_return<EntityWriter*> BeginWriteSection( const char *key, const u8 key_length );')
	lines.append('\tstatus EndWriteSection( const EntityWriter *section_writer );')
	lines.append('\tstatus WriteNullSection( const char *key, const u8 key_length );')
	lines.append('')
	lines.append('\t// Build a sections array. ')
	lines.append('\tstatus_return<EntityWriter*> BeginWriteSectionsArray( const char *key, const u8 key_length, const size_t array_size, const std::vector<i32> *index = nullptr );')
	lines.append('\tstatus BeginWriteSectionInArray( const EntityWriter *sections_array_writer , const size_t section_index );')
	lines.append('\tstatus EndWriteSectionInArray( const EntityWriter *sections_array_writer , const size_t section_index );')
	lines.append('\tstatus EndWriteSectionsArray( const EntityWriter *sections_array_writer );')
	lines.append('\tstatus WriteNullSectionsArray( const char *key, const u8 key_length );')
	lines.append('')
	lines.append('\t// The Write function template, specifically implemented for all supported value types.')
	lines.append('\ttemplate <class T> status Write( const char *key, const u8 key_length, const T &value );')
	lines.append('};')
	lines.append('')
	lines.append('}')
	lines.append('// namespace pds')
	lines.append('')
	lines.append('#ifdef PDS_IMPLEMENTATION')
	lines.append('#include "EntityWriter.inl"')
	lines.append('#endif//PDS_IMPLEMENTATION')
	lines.extend(hlp.end_header_file("EntityWriter.h"))
	hlp.write_lines_to_file("../Include/pds/EntityWriter.h",lines)

def EntityWriter_inl():
	lines = []
	lines.extend( hlp.generate_header() )
	lines.append('')
	lines.append('#include "writer_templates.h"')
	lines.append('')
	lines.append('namespace pds')
	lines.append('{')
	 
	lines.extend( hlp.inline_file('./InlinedCode/EntityWriter_inl.inl') )
	lines.append('')

	# print the base types
	for basetype in hlp.base_types:
		
		# all variants
		for type_impl in basetype.variants:
			implementing_type = str(type_impl.implementing_type)
			item_type = str(type_impl.item_type)
			num_items_per_object = str(type_impl.num_items_per_object)

			if type_impl.overrides_type:
				lines.append(f'// {implementing_type}: using {item_type} to store')
				lines.append(f'template <> status EntityWriter::Write<{implementing_type}>( const char *key, const u8 key_length, const {implementing_type} &src_variable )')
				lines.append(f'{{')
				lines.append(f'\tconst {item_type} tmp_variable = src_variable;')
				lines.append(f'\treturn this->Write<{item_type}>( key, key_length, tmp_variable );')
				lines.append(f'}}')
				lines.append(f'')
			
				lines.append(f'// {implementing_type}: using optional_value<{item_type}> to store' )
				lines.append(f'template <> status EntityWriter::Write<optional_value<{implementing_type}>>( const char *key, const u8 key_length, const optional_value<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\toptional_value<{item_type}> tmp_variable;')
				lines.append(f'\tif( src_variable.has_value() )')
				lines.append(f'\t\ttmp_variable.set( src_variable.value() );')
				lines.append(f'\treturn this->Write<optional_value<{item_type}>>( key, key_length, tmp_variable );')
				lines.append(f'}}')
				lines.append(f'')
			
				lines.append(f'// {implementing_type}: using std::vector<{item_type}> to store' )
				lines.append(f'template <> status EntityWriter::Write<std::vector<{implementing_type}>>( const char *key, const u8 key_length, const std::vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\tstd::vector<{item_type}> tmp_variable;')
				lines.append(f'\ttmp_variable.reserve( src_variable.size() );')
				lines.append(f'\tstd::copy( src_variable.begin(), src_variable.end(), std::back_inserter(tmp_variable) );')
				lines.append(f'\treturn this->Write<std::vector<{item_type}>>( key, key_length, tmp_variable );')
				lines.append(f'}}')
				lines.append(f'')
			
				lines.append(f'//  {implementing_type}: optional_vector<{item_type}> to store' )
				lines.append(f'template <> status EntityWriter::Write<optional_vector<{implementing_type}>>( const char *key, const u8 key_length, const optional_vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\toptional_vector<{item_type}> tmp_variable;')
				lines.append(f'\tif( src_variable.has_value() )')
				lines.append(f'\t{{')
				lines.append(f'\t\ttmp_variable.set();')
				lines.append(f'\t\ttmp_variable.values().reserve( src_variable.values().size() );')
				lines.append(f'\t\tstd::copy( src_variable.values().begin(), src_variable.values().end(), std::back_inserter(tmp_variable.values()) );')
				lines.append(f'\t}}')
				lines.append(f'	return this->Write<optional_vector<{item_type}>>( key, key_length, tmp_variable );')
				lines.append(f'}}')
				lines.append(f'')
				
				lines.append(f'// {implementing_type}: using idx_vector<{item_type}> to store' )
				lines.append(f'template <> status EntityWriter::Write<idx_vector<{implementing_type}>>( const char *key, const u8 key_length, const idx_vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\tidx_vector<{item_type}> tmp_variable;')
				lines.append(f'\ttmp_variable.index().reserve( src_variable.index().size() );')
				lines.append(f'\ttmp_variable.values().reserve( src_variable.values().size() );')
				lines.append(f'\tstd::copy( src_variable.index().begin(), src_variable.index().end(), std::back_inserter(tmp_variable.index()) );')
				lines.append(f'\tstd::copy( src_variable.values().begin(), src_variable.values().end(), std::back_inserter(tmp_variable.values()) );')
				lines.append(f'\treturn this->Write<idx_vector<{item_type}>>( key, key_length, tmp_variable );')
				lines.append(f'}}')
				lines.append(f'')

				lines.append(f'//  {implementing_type}: optional_idx_vector<{item_type}> to store' )
				lines.append(f'template <> status EntityWriter::Write<optional_idx_vector<{implementing_type}>>( const char *key, const u8 key_length, const optional_idx_vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\toptional_idx_vector<{item_type}> tmp_variable;')
				lines.append(f'\tif( src_variable.has_value() )')
				lines.append(f'\t{{')
				lines.append(f'\t\ttmp_variable.set();')
				lines.append(f'\t\ttmp_variable.index().reserve( src_variable.index().size() );')
				lines.append(f'\t\ttmp_variable.values().reserve( src_variable.values().size() );')
				lines.append(f'\t\tstd::copy( src_variable.index().begin(), src_variable.index().end(), std::back_inserter(tmp_variable.index()) );')
				lines.append(f'\t\tstd::copy( src_variable.values().begin(), src_variable.values().end(), std::back_inserter(tmp_variable.values()) );')
				lines.append(f'\t}}')
				lines.append(f'\treturn this->Write<optional_idx_vector<{item_type}>>( key, key_length, tmp_variable );')
				lines.append(f'}}')
				lines.append(f'')
				
			else:
				type_name = 'vt_' + basetype.name
				array_type_name = 'vt_array_' + basetype.name
				
				lines.append(f'// {type_name}: {implementing_type}')
				lines.append(f'template <> status EntityWriter::Write<{implementing_type}>( const char *key, const u8 key_length, const {implementing_type} &src_variable )')
				lines.append(f'{{')
				lines.append(f'\treturn write_single_value<serialization_type_index::{type_name},{implementing_type}>( this->dstream, key, key_length, &src_variable );')
				lines.append(f'}}')
				lines.append(f'')
				
				lines.append(f'// {type_name}: optional_value<{implementing_type}>' )
				lines.append(f'template <> status EntityWriter::Write<optional_value<{implementing_type}>>( const char *key, const u8 key_length, const optional_value<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\tconst {implementing_type} *p_src_variable = (src_variable.has_value()) ? &(src_variable.value()) : nullptr;')
				lines.append(f'\treturn write_single_value<serialization_type_index::{type_name},{implementing_type}>( this->dstream, key, key_length, p_src_variable );')
				lines.append(f'}}')
				lines.append(f'')
				
				lines.append(f'//  {array_type_name}: std::vector<{implementing_type}>' )
				lines.append(f'template <> status EntityWriter::Write<std::vector<{implementing_type}>>( const char *key, const u8 key_length, const std::vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\treturn write_array<serialization_type_index::{array_type_name},{implementing_type}>(this->dstream, key, key_length, &src_variable , nullptr );')
				lines.append(f'}}')
				lines.append(f'')
				
				lines.append(f'//  {array_type_name}: optional_vector<{implementing_type}>' )
				lines.append(f'template <> status EntityWriter::Write<optional_vector<{implementing_type}>>( const char *key, const u8 key_length, const optional_vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\tconst std::vector<{implementing_type}> *p_src_variable = (src_variable.has_value()) ? &(src_variable.values()) : nullptr;')
				lines.append(f'\treturn write_array<serialization_type_index::{array_type_name},{implementing_type}>(this->dstream, key, key_length, p_src_variable , nullptr );')
				lines.append(f'}}')
				lines.append(f'')
				
				lines.append(f'//  {array_type_name}: idx_vector<{implementing_type}>' )
				lines.append(f'template <> status EntityWriter::Write<idx_vector<{implementing_type}>>( const char *key, const u8 key_length, const idx_vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\treturn write_array<serialization_type_index::{array_type_name},{implementing_type}>(this->dstream, key, key_length, &(src_variable.values()) , &(src_variable.index()) );')
				lines.append(f'}}')
				lines.append(f'')
				
				lines.append(f'//  {array_type_name}: optional_idx_vector<{implementing_type}>' )
				lines.append(f'template <> status EntityWriter::Write<optional_idx_vector<{implementing_type}>>( const char *key, const u8 key_length, const optional_idx_vector<{implementing_type}> &src_variable )')
				lines.append(f'{{')
				lines.append(f'\tconst std::vector<{implementing_type}> *p_src_values = (src_variable.has_value()) ? &(src_variable.values()) : nullptr;')
				lines.append(f'\tconst std::vector<i32> *p_src_index = (src_variable.has_value()) ? &(src_variable.index()) : nullptr;')
				lines.append(f'\treturn write_array<serialization_type_index::{array_type_name},{implementing_type}>(this->dstream, key, key_length, p_src_values , p_src_index );')
				lines.append(f'}}')
				lines.append(f'')
				
	## other types which convert to existing types
	#types = [ ['item_ref','uuid'], ['entity_ref','hash'] ]
	#for type in types:
	#	type_name = type[0]
	#	implementing_type = type[1]

	lines.append('}')
	lines.append('// namespace pds')
	hlp.write_lines_to_file("../Include/pds/EntityWriter.inl",lines)

def run():
	EntityWriter_h()
	EntityWriter_inl()