from EntitiesHelpers import * 
import os
import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, vector_dimension_range, nonconst_const_range
from ctlepy.formatted_output import formatted_output
from pathlib import Path

from ctypes import c_ulonglong 
from ctypes import c_ubyte


def ImplementClearCall(op: formatted_output, item:Item, var) -> None:
	op.comment_ln(f'clear variable "{var.Name}"')

	# clear all values, base values and Entities
	if var.Optional:
		op.ln(f'obj.v_{var.Name}.reset();')
	else:
		base_type,base_variant = hlp.get_base_type_variant(var.Type)
		if base_type is not None:
			# we have a base type, add the write code directly
			op.ln(f'obj.v_{var.Name} = {{}};')
		else:
			# clear through the MF::Clear method of the type
			op.ln(f'ctStatusCall({var.Type}::MF::Clear( obj.v_{var.Name} ));')
	op.ln()

def ImplementDeepCopyCall(op: formatted_output, item:Item, var) -> None:
	op.comment_ln(f'copy variable "{var.Name}"')

	# deep copy all values
	if var.IsBaseType:
		# we have a base type, add the copy code directly
		op.ln(f'dest.v_{var.Name} = source->v_{var.Name};')
	else:
		# this is an item type
		if var.Optional:
			op.ln(f'if( source->v_{var.Name}.has_value() )')
			with op.blk():
				op.ln(f'dest.v_{var.Name}.set();')
				op.ln(f'ctStatusCall( {var.Type}::MF::DeepCopy( dest.v_{var.Name}.value() , &(source->v_{var.Name}.value()) ));')
			op.ln(f'else')
			with op.blk():
				op.ln(f'dest.v_{var.Name}.reset();')			
		else:
			op.ln(f'ctStatusCall( {var.Type}::MF::DeepCopy( dest.v_{var.Name} , &(source->v_{var.Name} ) ));')

def ImplementEqualsCall(item,var):
	lines = []

	lines.append(f'        // check variable "{var.Name}"')

	# do we have a base type or item?
	if var.IsBaseType:
		# we have a base type, do the compare directly
		lines.append(f'        if( lvar->v_{var.Name} != rvar->v_{var.Name} )')
		lines.append(f'            return false;')
	else:
		# not a base type, so an item. check item
		if var.Optional:
			lines.append(f'        if( !{item.Name}::{var.Type}::MF::Equals(')
			lines.append(f'            lvar->v_{var.Name}.has_value() ? &lvar->v_{var.Name}.value() : nullptr,  ')
			lines.append(f'            rvar->v_{var.Name}.has_value() ? &rvar->v_{var.Name}.value() : nullptr')
			lines.append(f'            ) )')
			lines.append('            return false;')
		else:
			lines.append(f'        if( !{item.Name}::{var.Type}::MF::Equals( &lvar->v_{var.Name} , &rvar->v_{var.Name} ) )')
			lines.append('            return false;')

	lines.append('')

	return lines

def ImplementWriterCall(item,var):
	lines = []

	# do we have a base type or item?
	if var.IsBaseType:
		# we have a base type, add the write code directly
		lines.append(f'        // write variable "{var.Name}"')
		lines.append(f'        success = writer.Write<{var.TypeString}>( pdsKeyMacro({var.Name}) , obj.v_{var.Name} );')
		lines.append(f'        if( !success )')
		lines.append(f'            return status::cant_write;')
		lines.append('')
	else:
		# not a base type, so an item. add a block
		lines.append(f'        // write section "{var.Name}"')
		lines.append(f'        success = (section_writer = writer.BeginWriteSection( pdsKeyMacro({var.Name}) ));')
		lines.append('        if( !success )')
		lines.append('            return status::cant_write;')
		if var.Optional:
			lines.append(f'        if( obj.v_{var.Name}.has_value() )')
			lines.append('            {')
			lines.append(f'            ctStatusCall( {item.Name}::{var.Type}::MF::Write( obj.v_{var.Name}.value(), *section_writer ) );')
			lines.append('            }')
		else:
			lines.append(f'        ctStatusCall( {item.Name}::{var.Type}::MF::Write( obj.v_{var.Name}, *section_writer ) );')
		lines.append('        writer.EndWriteSection( section_writer );')
		lines.append('        section_writer = nullptr;')
		lines.append('')

	return lines

def ImplementReaderCall(item,var):
	lines = []

	if var.Optional:
		value_can_be_null = "true"
	else:
		value_can_be_null = "false"

	# do we have a base type or item?
	if var.IsBaseType:
		# we have a base type, add the read code directly
		lines.append(f'        // read variable "{var.Name}"')
		lines.append(f'        success = reader.Read<{var.TypeString}>( pdsKeyMacro({var.Name}) , obj.v_{var.Name} );')
		lines.append(f'        if( !success )')
		lines.append(f'            return status::cant_read;')
		lines.append('')
	else:
		# not a base type, so an item. add a block
		lines.append(f'        // read section "{var.Name}"')
		lines.append(f'        std::tie(section_reader,success) = reader.BeginReadSection( pdsKeyMacro({var.Name}) , {value_can_be_null} );')
		lines.append('        if( !success )')
		lines.append('            return status::cant_read;')
		lines.append('        if( section_reader )')
		lines.append('            {')
		if var.Optional:
			lines.append(f'            obj.v_{var.Name}.set();')
			lines.append(f'            ctStatusCall( {item.Name}::{var.Type}::MF::Read( obj.v_{var.Name}.value(), *section_reader ) );')
		else:
			lines.append(f'            ctStatusCall( {item.Name}::{var.Type}::MF::Read( obj.v_{var.Name}, *section_reader ) );')
		lines.append('            reader.EndReadSection( section_reader );')
		lines.append('            section_reader = nullptr;')
		lines.append('            }')
		if var.Optional:
			lines.append('        else')
			lines.append(f'            obj.v_{var.Name}.reset();')
		lines.append('')

	return lines

def ImplementVariableValidatorCall(item,var):
	lines = []

	# validate all values, base values and Entities
	base_type,base_variant = hlp.get_base_type_variant(var.Type)
	if base_type is None:
		lines.append(f'        // validate variable "{var.Name}"')
		if var.Optional:
			lines.append(f'        if( obj.v_{var.Name}.has_value() )')
			lines.append('            {')
			lines.append(f'            ctStatusCall( {var.Type}::MF::Validate( obj.v_{var.Name}.value() , validator ) );')
			lines.append('            }')
		else:
			lines.append(f'        ctStatusCall( {var.Type}::MF::Validate( obj.v_{var.Name} , validator ) );')
		lines.append('')

	return lines

def ImplementToPreviousCall(item:Item , mapping:Mapping):
	lines = []	

	# if code inject, do that and return
	if type(mapping) is CustomCodeMapping:
		lines.append(mapping.ToPrevious)
		return lines

	# if it is a deleted variable, just return empty
	if type(mapping) is DeletedVariable:
		return []

	# not custom code, so there is exactly one variable
	variableName = mapping.Variables[0]
	
	# find variable in item
	variable = next( (var for var in item.Variables if var.Name == variableName) , None )
	if variable == None:
		return []

	# validate all values, base values and Entities
	base_type,base_variant = hlp.get_base_type_variant(variable.Type)

	if type(mapping) is NewVariable: # if this is a new variable, not much we can do converting back
		return []

	if type(mapping) is RenamedVariable: # renamed or same variable, copy to the previous name in the dest
		if base_type is None:
			lines.append(f'        ctStatusCall( {variable.Type}::MF::Copy( dest.{mapping.PreviousName}() , obj.v_{variable.Name} ) );')
		else:
			lines.append(f'        dest.{mapping.PreviousName}() = obj.v_{variable.Name};')

	return lines

def ImplementFromPreviousCall(item:Item , mapping:Mapping):
	lines = []	

	# if code inject, do that and return
	if type(mapping) is CustomCodeMapping:
		lines.append(mapping.FromPrevious)
		return lines

	# if it is a deleted variable, just return empty
	if type(mapping) is DeletedVariable:
		return []

	# not custom code, so there is exactly one variable
	variableName = mapping.Variables[0]
	
	# find variable in item
	variable = next( (var for var in item.Variables if var.Name == variableName) , None )
	if variable == None:
		return []

	# validate all values, base values and Entities
	base_type,base_variant = hlp.get_base_type_variant(variable.Type)

	if type(mapping) is NewVariable: # if this is a new variable, clear it
		return ImplementClearCall(item,variable)

	if type(mapping) is RenamedVariable: # renamed or same variable, copy to the previous name in the dest
		if base_type is None:
			lines.append(f'        ctStatusCall( {variable.Type}::MF::Copy( obj.v_{variable.Name} , src.{mapping.PreviousName}() ) );')
		else:
			lines.append(f'        obj.v_{variable.Name} = src.{mapping.PreviousName}();')

	return lines

def CreateItemClassImpl(op: formatted_output, item: Item) -> None:
	package = item.Package
	version = item.Version

	if item.IsEntity:
		op.ln(f'const char *{item.Name}::EntityTypeString() const {{ return {item.Name}::ItemTypeString; }}')
		op.ln()

	# check if there are entities in the variable list, which means we need to add entity writers/readers/validators
	vars_have_item = False
	for var in item.Variables:
		base_type,base_variant = hlp.get_base_type_variant(var.Type)
		if base_type is None:
			vars_have_item = True
			break
	
	# clear code
	op.ln(f'status {item.Name}::MF::Clear( {item.Name} &obj )')
	with op.blk():
		op.comment_ln('direct clear calls on variables and Entities')
		op.ln('')
		for var in item.Variables:
			ImplementClearCall(op,item,var)
		op.ln('return status::ok;')
	op.ln('')

	# deep copy code
	op.ln(f'status {item.Name}::MF::DeepCopy( {item.Name} &dest, const {item.Name} *source )')
	with op.blk():
		op.comment_ln('just call Clear if source is nullptr')
		op.ln('if( !source )')
		with op.blk():
			op.ln('ctStatusCall( MF::Clear( dest ) );')
			op.ln('return status::ok;')
		op.ln('')
		for var in item.Variables:
			op.ln(ImplementDeepCopyCall(op,item,var))
		op.ln('return status::ok;')
	op.ln()

	# # equals code
	# lines.append(f'    bool {item.Name}::MF::Equals( const {item.Name} *lvar, const {item.Name} *rvar )')
	# lines.append('        {')
	# lines.append('        // early out if pointers are equal')
	# lines.append('        if( lvar == rvar )')
	# lines.append('            return true;')
	# lines.append('')
	# lines.append('        // early out if one of the pointers is nullptr')
	# lines.append('        if( !lvar || !rvar )')
	# lines.append('            return false;')
	# lines.append('')
	# for var in item.Variables:
	# 	lines.extend(ImplementEqualsCall(item,var))
	# lines.append('        return true;')
	# lines.append('        }')
	# lines.append('')

	# # writer code
	# lines.append(f'    status {item.Name}::MF::Write( const {item.Name} &obj, pds::EntityWriter &writer )')
	# lines.append('        {')
	# lines.append('        bool success = true;')
	# if vars_have_item:
	# 	lines.append('        pds::EntityWriter *section_writer = nullptr;')
	# lines.append('')
	# for var in item.Variables:
	# 	lines.extend(ImplementWriterCall(item,var))
	# lines.append('        return status::ok;')
	# lines.append('        }')
	# lines.append('')
	
	# # reader code
	# lines.append(f'    status {item.Name}::MF::Read( {item.Name} &obj, pds::EntityReader &reader )')
	# lines.append('        {')
	# lines.append('        bool success = true;')
	# if vars_have_item:
	# 	lines.append('        pds::EntityReader *section_reader = nullptr;')
	# lines.append('')
	# for var in item.Variables:
	# 	lines.extend(ImplementReaderCall(item,var))
	# lines.append('        return status::ok;')
	# lines.append('        }')
	# lines.append('')

	# # setup validation lines first, and see if there are any lines generated
	# validation_lines = []
	# for var in item.Variables:
	# 	validation_lines.extend(ImplementVariableValidatorCall(item,var))
	# for validation in item.Validations:
	# 	validation_lines.extend( validation.GenerateValidationCode(item,'        ') )
	# 	validation_lines.append('')

	# # if we have validation lines, setup the support code else use empty call
	# if len(validation_lines) > 0:
	# 	# validator code
	# 	lines.append(f'    status {item.Name}::MF::Validate( const {item.Name} &obj, pds::EntityValidator &validator )')
	# 	lines.append('        {')
	# 	lines.append('')
	# 	lines.extend( validation_lines )
	# 	lines.append('')
	# else:
	# 	lines.append(f'    status {item.Name}::MF::Validate( const {item.Name} &/*obj*/, pds::EntityValidator &/*validator*/ )')
	# 	lines.append('        {')
	# 	lines.append('        // no validation defined in this class, just return ok')

	# lines.append('        return status::ok;')
	# lines.append('        }')
	# lines.append('')

	# # entity code
	# if item.IsEntity:
	# 	lines.append(f'    const {item.Name} *{item.Name}::MF::EntitySafeCast( const pds::Entity *srcEnt )')
	# 	lines.append('        {')
	# 	lines.append(f'        if( srcEnt && std::string(srcEnt->EntityTypeString()) == {item.Name}::ItemTypeString )')
	# 	lines.append('            {')
	# 	lines.append(f'            return (const {item.Name} *)(srcEnt);')
	# 	lines.append('            }')
	# 	lines.append('        return nullptr;')
	# 	lines.append('        }')
	# 	lines.append('')
	# 	lines.append(f'    std::shared_ptr<const {item.Name}> {item.Name}::MF::EntitySafeCast( std::shared_ptr<const pds::Entity> srcEnt )')
	# 	lines.append('        {')
	# 	lines.append(f'        if( srcEnt && std::string(srcEnt->EntityTypeString()) == {item.Name}::ItemTypeString )')
	# 	lines.append('            {')
	# 	lines.append(f'            return std::static_pointer_cast<const {item.Name}>(srcEnt);')
	# 	lines.append('            }')
	# 	lines.append('        return nullptr;')
	# 	lines.append('        }')
	# 	lines.append('')

	# # modified item code
	# if item.IsModifiedFromPreviousVersion:
	# 	lines.append(f'    status {item.Name}::MF::ToPrevious( {item.PreviousVersion.Version.Name}::{item.Name} &dest , const {item.Name} &obj )')
	# 	lines.append('        {')
	# 	lines.append('')			
	# 	for mapping in item.Mappings:
	# 		lines.extend(ImplementToPreviousCall(item,mapping))	
	# 	lines.append('')			
	# 	lines.append('        return status::ok;')
	# 	lines.append('        }')
	# 	lines.append('')
	# 	lines.append(f'    status {item.Name}::MF::FromPrevious( {item.Name} &obj , const {item.PreviousVersion.Version.Name}::{item.Name} &src )')
	# 	lines.append('        {')
	# 	lines.append('')			
	# 	for mapping in item.Mappings:
	# 		lines.extend(ImplementFromPreviousCall(item,mapping))	
	# 	lines.append('')			
	# 	lines.append('        return status::ok;')
	# 	lines.append('        }')



def CreateItemImplementation(item):
	package = item.Package
	version = item.Version
	version_prefix = version.Name + '_'

	# set up the file path and name
	file_path = Path(package.Path) / version.Name / f'{version.Name}_{item.Name}.inl'
	file_name = file_path.name

	# if this is an aliased entity, dont generate an inl file
	if item.IsIdenticalToPreviousVersion:
		return

	op = formatted_output()
	op.generate_license_header()

	op.comment_ln(f'class: {item.Name}')
	op.comment_ln(f'version: {version.Name}')
	op.ln()

	op.ln(f'#include "{item.GetImplementingHeaderFilePath()}"')
	op.ln(f'#include "{version.Name}_{item.Name}_MF.h"')

	# include dependences that were forward referenced in the header
	for dep in item.Dependencies:
		if not dep.IncludeInHeader:
			if dep.PDSType:
				op.ln(f'#include <pds/{dep.Name}_MF.h>')
			else:
				op.ln(f'#include "{version.Name}_{dep.Name}.h"')
	op.ln()

	with op.ns(package.Name, add_empty_line=False):
		op.ln()
		op.ln('#include <pds/_pds_macros.inl>')
		if item.IsReleaseVersion:
			op.ln('')
			CreateItemClassImpl(op, item)
		else:
			with op.ns(version.Name):
				op.ln('')
				CreateItemClassImpl(op, item)
		op.ln('#include <pds/_pds_undef_macros.inl>')
		op.ln()

	op.write_lines_to_file(str(file_path))
