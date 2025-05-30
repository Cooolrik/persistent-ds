from EntitiesHelpers import * 
import os
import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, vector_dimension_range, nonconst_const_range
from ctlepy.formatted_output import formatted_output
from pathlib import Path

from ctypes import c_ulonglong 
from ctypes import c_ubyte


def ImplementClearCall(op: formatted_output, item:Item, var) -> None:
	# clear all values, base values and Entities
	op.comment_ln(f'clear variable "{var.Name}"')
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

def ImplementDeepCopyCall(op: formatted_output, item:Item, var) -> None:
	# deep copy all values
	op.comment_ln(f'copy variable "{var.Name}"')
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

def ImplementEqualsCall(op: formatted_output, item:Item, var) -> None:
	op.comment_ln(f'check variable "{var.Name}"')
	if var.IsBaseType:
		# we have a base type, do the compare directly
		op.ln(f'if( lvar->v_{var.Name} != rvar->v_{var.Name} )')
		with op.blk():
			op.ln(f'return false;')
	else:
		# not a base type, so an item. check item
		if var.Optional:
			op.ln(f'if( !{item.Name}::{var.Type}::MF::Equals(')
			with op.tab():
				op.ln(f'(lvar->v_{var.Name}.has_value()) ? (&lvar->v_{var.Name}.value()) : (nullptr),')
				op.ln(f'(rvar->v_{var.Name}.has_value()) ? (&rvar->v_{var.Name}.value()) : (nullptr)')
			op.ln(f') )')
			with op.blk():
				op.ln('return false;')
		else:
			op.ln(f'if( !{item.Name}::{var.Type}::MF::Equals( &lvar->v_{var.Name} , &rvar->v_{var.Name} ) )')
			with op.blk():
				op.ln('return false;')

def ImplementWriterCall(op: formatted_output, item:Item, var):
	if var.IsBaseType:
		# we have a base type, add the write code directly
		op.comment_ln(f'write variable "{var.Name}"')
		op.ln(f'ctStatusCall( writer.Write<{var.TypeString}>( pdsKeyMacro({var.StorageName}) , obj.v_{var.Name} ) );')
	else:
		# not a base type, so an item. add a block
		op.comment_ln(f'write section "{var.Name}"')
		op.ln(f'ctStatusReturnCall( section_writer, writer.BeginWriteSection( pdsKeyMacro({var.StorageName}) ) );')
		if var.Optional:
			op.ln(f'if( obj.v_{var.Name}.has_value() )')
			with op.blk():
				op.ln(f'ctStatusCall( {item.Name}::{var.Type}::MF::Write( obj.v_{var.Name}.value(), *section_writer ) );')
		else:
			op.ln(f'ctStatusCall( {item.Name}::{var.Type}::MF::Write( obj.v_{var.Name}, *section_writer ) );')
		op.ln('ctStatusCall( writer.EndWriteSection( section_writer ) );')

def ImplementReaderCall(op: formatted_output, item:Item, var):
	if var.IsBaseType:
		# we have a base type, add the read code directly
		op.comment_ln(f'read variable "{var.Name}"')
		op.ln(f'ctStatusCall( reader.Read<{var.TypeString}>( pdsKeyMacro({var.StorageName}) , obj.v_{var.Name} ) );')
	else:
		# not a base type, so an item. add a block
		op.comment_ln(f'read section "{var.Name}"')
		op.ln(f'ctStatusReturnCall( section_reader, reader.BeginReadSection( pdsKeyMacro({var.StorageName}) , {"true" if var.Optional else "false"} ) );')
		op.ln('if( section_reader )')
		with op.blk():
			if var.Optional:
				op.ln(f'obj.v_{var.Name}.set();')
				op.ln(f'ctStatusCall( {item.Name}::{var.Type}::MF::Read( obj.v_{var.Name}.value(), *section_reader ) );')
			else:
				op.ln(f'ctStatusCall( {item.Name}::{var.Type}::MF::Read( obj.v_{var.Name}, *section_reader ) );')
			op.ln(f'reader.EndReadSection( section_reader );')
		if var.Optional:
			op.ln(f'else')
			with op.blk():
				op.ln(f'obj.v_{var.Name}.reset();')

def ImplementVariableValidatorCall(op: formatted_output, item:Item, var) -> bool:
	base_type,base_variant = hlp.get_base_type_variant(var.Type)
	if op is None:
		return base_type is None # report if the function will generate code or not
	
	if base_type is None:
		op.comment_ln(f'validate variable "{var.Name}"')
		if var.Optional:
			op.ln(f'if( obj.v_{var.Name}.has_value() )')
			with op.blk():
				op.ln(f'ctStatusCall( {var.Type}::MF::Validate( obj.v_{var.Name}.value() , validator ) );')
		else:
			op.ln(f'ctStatusCall( {var.Type}::MF::Validate( obj.v_{var.Name} , validator ) );')
	else:
		op.comment_ln(f'variable "{var.Name}" has no validation defined')

def ImplementToPreviousCall(op:formatted_output, item:Item, mapping:Mapping) -> None:
	# if code inject, do that and return
	if type(mapping) is CustomCodeMapping:
		op.comment_ln(f'custom mapping of {item.Name}')
		op.ln(mapping.ToPrevious)
		return
	
	# if it is a deleted variable, just return empty
	if type(mapping) is DeletedVariable:
		op.comment_ln(f'{item.Name} is deleted in current version, so no value to copy')
		return

	
	# not custom code, so there is exactly one variable
	variableName = mapping.Variables[0]
	
	# find variable in item
	variable = next( (var for var in item.Variables if var.Name == variableName) , None )
	if variable == None:
		return

	# if this is a new variable, not much we can do converting back
	if type(mapping) is NewVariable: 
		op.comment_ln(f'{variable.Name} is a new variable in the current version, so cant copy to previous')
		return
	
	# validate all values, base values and Entities
	base_type,base_variant = hlp.get_base_type_variant(variable.Type)
	if issubclass(type(mapping),RenamedVariable): # renamed or same variable, copy to the previous name in the previous version (dest)
		op.comment_ln(f'copy current "{variable.Name}" to previous "{mapping.PreviousName}"')
		if base_type is None:
			op.ln(f'ctStatusCall( {variable.Type}::MF::Copy( dest.{mapping.PreviousName}() , obj.v_{variable.Name} ) );')
		else:
			op.ln(f'dest.{mapping.PreviousName}() = obj.v_{variable.Name};')

def ImplementFromPreviousCall(op:formatted_output, item:Item , mapping:Mapping) -> None:
	# if code inject, do that and return
	if type(mapping) is CustomCodeMapping:
		op.comment_ln(f'custom mapping of {item.Name}')
		op.ln(mapping.FromPrevious)
		return

	# if it is a deleted variable, just return empty
	if type(mapping) is DeletedVariable:
		op.comment_ln(f'{item.Name} is deleted in this version, so do nothing')
		return

	# not custom code, so there is exactly one variable
	variableName = mapping.Variables[0]
	
	# find variable in item
	variable = next( (var for var in item.Variables if var.Name == variableName) , None )
	if variable == None:
		return

	# if this is a new variable, clear it
	if type(mapping) is NewVariable: 
		op.comment_ln(f'{variable.Name} is new for this version, so clear value')
		ImplementClearCall(op, item, variable)
		return
	
	# validate all values, base values and Entities
	base_type,base_variant = hlp.get_base_type_variant(variable.Type)
	if issubclass(type(mapping),RenamedVariable): # renamed or same variable, copy to the previous name in the dest
		op.comment_ln(f'copy previous "{mapping.PreviousName}" to current "{variable.Name}"')
		if base_type is None:
			op.ln(f'ctStatusCall( {variable.Type}::MF::Copy( obj.v_{variable.Name} , src.{mapping.PreviousName}() ) );')
		else:
			op.ln(f'obj.v_{variable.Name} = src.{mapping.PreviousName}();')

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
	
	# ctors and copy operator code
	op.ln(f'{item.Name}::{item.Name}() = default;')
	op.ln()
	op.ln(f'{item.Name}::{item.Name}( const {item.Name} &rval )')
	with op.blk():
		op.ln('MF::DeepCopy( *this , &rval );')
	op.ln()
	op.ln(f'{item.Name} &{item.Name}::operator=( const {item.Name} &rval )')
	with op.blk():
		op.ln('MF::DeepCopy( *this , &rval );')
		op.ln('return *this;')
	op.ln()
	op.ln(f'{item.Name}::~{item.Name}() = default;')
	op.ln()
	op.ln(f'bool {item.Name}::operator==( const {item.Name} &rval ) const')
	with op.blk():
		op.ln('return MF::Equals( this, &rval );')
	op.ln()
	op.ln(f'bool {item.Name}::operator!=( const {item.Name} &rval ) const')
	with op.blk():
		op.ln('return !(MF::Equals( this, &rval ));')
	op.ln()

	# clear code
	op.ln(f'status {item.Name}::MF::Clear( {item.Name} &obj )')
	with op.blk():
		op.comment_ln('direct clear calls on variables and Entities')
		op.ln('')
		for var in item.Variables:
			ImplementClearCall(op,item,var)
			op.ln()
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
			ImplementDeepCopyCall(op,item,var)
			op.ln()
		op.ln('return status::ok;')
	op.ln()

	# deep copy code
	op.ln(f'bool {item.Name}::MF::Equals( const {item.Name} *lvar, const {item.Name} *rvar )')
	with op.blk():
		op.comment_ln('early out if pointers are equal')
		op.ln('if( lvar == rvar )')
		with op.blk():
			op.ln('return true;')
		op.ln('')
		op.comment_ln('early out if one of the pointers is nullptr')
		op.ln('if( !lvar || !rvar )')
		with op.blk():
			op.ln('return false;')
		op.ln('')
		for var in item.Variables:
			ImplementEqualsCall(op,item,var)
			op.ln()
		op.ln('return true;')
	op.ln()

	# writer code
	op.ln(f'status {item.Name}::MF::Write( const {item.Name} &obj, pds::EntityWriter &writer )')
	with op.blk():
		if vars_have_item:
			op.ln('pds::EntityWriter *section_writer;')
		op.ln()
		for var in item.Variables:
			ImplementWriterCall(op,item,var)
			op.ln()
		op.ln('return status::ok;')
	op.ln()
	
	# reader code
	op.ln(f'status {item.Name}::MF::Read( {item.Name} &obj, pds::EntityReader &reader )')
	with op.blk():
		if vars_have_item:
			op.ln('pds::EntityReader *section_reader = nullptr;')
		op.ln()
		for var in item.Variables:
			ImplementReaderCall(op,item,var)
			op.ln()
		op.ln('return status::ok;')
	op.ln()

	# check if validation will generate code
	will_generate_validation_code = False
	for var in item.Variables:
		if ImplementVariableValidatorCall(None,item,var):
			will_generate_validation_code = True
			break

	# if we have validation lines, setup the support code else use empty call
	if will_generate_validation_code:
		# validator code
		op.ln(f'status {item.Name}::MF::Validate( const {item.Name} &obj, pds::EntityValidator &validator )')
		with op.blk():
			for var in item.Variables:
				ImplementVariableValidatorCall(op,item,var)
				op.ln()
			op.ln('return status::ok;')
	else:
		op.ln(f'status {item.Name}::MF::Validate( const {item.Name} &/*obj*/, pds::EntityValidator &/*validator*/ )')
		with op.blk():
			op.comment_ln('no validation defined in this class, just return ok')
			op.ln('return status::ok;')
	op.ln()

	# entity code
	if item.IsEntity:
		op.ln(f'const {item.Name} *{item.Name}::MF::EntitySafeCast( const pds::Entity *srcEnt )')
		with op.blk():
			op.ln(f'if( srcEnt && std::string(srcEnt->EntityTypeString()) == {item.Name}::ItemTypeString )')
			with op.blk():
				op.ln(f'return (const {item.Name} *)(srcEnt);')
			op.ln('return nullptr;')
		op.ln()
		op.ln(f'std::shared_ptr<const {item.Name}> {item.Name}::MF::EntitySafeCast( std::shared_ptr<const pds::Entity> srcEnt )')
		with op.blk():
			op.ln(f'if( srcEnt && std::string(srcEnt->EntityTypeString()) == {item.Name}::ItemTypeString )')
			with op.blk():
				op.ln(f'return std::static_pointer_cast<const {item.Name}>(srcEnt);')
			op.ln('return nullptr;')
		op.ln()

	# modified item code
	if item.IsModifiedFromPreviousVersion:
		op.ln(f'status {item.Name}::MF::ToPrevious( {item.PreviousVersion.Version.Name}::{item.Name} &dest , const {item.Name} &obj )')
		with op.blk():
			for mapping in item.Mappings:
				ImplementToPreviousCall(op,item,mapping)
				op.ln()
			op.ln('return status::ok;')
		op.ln()
		op.ln(f'status {item.Name}::MF::FromPrevious( {item.Name} &obj , const {item.PreviousVersion.Version.Name}::{item.Name} &src )')
		with op.blk():
			for mapping in item.Mappings:
				ImplementFromPreviousCall(op,item,mapping)
				op.ln();
			op.ln('return status::ok;')

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
