from EntitiesHelpers import * 
import os
import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, vector_dimension_range, nonconst_const_range
from ctlepy.formatted_output import formatted_output
from pathlib import Path

from ctypes import c_ulonglong 
from ctypes import c_ubyte

def CreateItemClass(op: formatted_output, item: Item) -> None:
	package = item.Package
	version = item.Version
	
	# list dependences that only needs a forward reference in the header
	for dep in item.Dependencies:
		if not dep.IncludeInHeader:
			op.ln(f'    class {dep.Name};')
	op.ln()
	op.comment_ln(f'{item.Name} class')

	op.ln(f'class {item.Name} {": public pds::Entity" if item.IsEntity else ""}')
	with op.blk(no_indent=True, add_semicolon=True):

		# initalization of the class
		op.ln('public:')
		with op.tab():
			op.ln('class MF; friend MF; // forward declaration and friend definition of the management function class')
			op.ln(f'static constexpr const char *ItemTypeString = "{package.Name}.{version.Name}.{item.Name}"; // item type and version string')
			if item.IsEntity:
				op.ln(f'virtual const char *EntityTypeString() const; // entity type and version string, to fetch the type from the entity interface')
			op.ln()

			# list typedefs of templates
			if len(item.Templates) > 0:
				op.comment_ln('typedefs for templates')
				for typ in item.Templates:
					op.ln(typ.Declaration)
				op.ln()

			op.comment_ln('ctor/dtor')
			op.ln(f'{item.Name}();')
			op.ln(f'{item.Name}( const {item.Name} & );')
			op.ln(f'{item.Name} &operator=( const {item.Name} & );')
			op.ln(f'{item.Name}( {item.Name} && );')
			op.ln(f'{item.Name} &operator=( {item.Name} && );')
			op.ln(f'{"virtual " if item.IsEntity else ""}~{item.Name}();')
			op.ln()

			op.comment_ln('value compare operators')
			op.ln(f'bool operator==( const {item.Name} &rval ) const;')
			op.ln(f'bool operator!=( const {item.Name} &rval ) const;')
			op.ln()

			if item.IsEntity:
				op.comment_ln('safe cast to entity')
				op.ln(f'static const {item.Name} *EntitySafeCast( const pds::Entity *srcEnt );')
				op.ln(f'static std::shared_ptr<const {item.Name}> EntitySafeCast( std::shared_ptr<const pds::Entity> srcEnt );')
				op.ln('')

		# variable declarations
		op.ln('private:')
		with op.tab():
			for var in item.Variables:
				if var.IsSimpleBaseType:
					op.ln(f'{var.TypeString} v_{var.Name} = {{}};')
				else:
					op.ln(f'{var.TypeString} v_{var.Name};')
			op.ln()

		# variable accessors, const and non-const versions
		op.ln('public:')
		with op.tab():		
			for var in item.Variables:
				op.ln(f'// accessor for referencing variable {var.Name}')
				op.ln(f'const {var.TypeString} & {var.Name}() const {{ return this->v_{var.Name}; }}')
				op.ln(f'{var.TypeString} & {var.Name}() {{ return this->v_{var.Name}; }}')
				op.ln('')


def CreateItemHeader(item: Item) -> None:
	package = item.Package
	version = item.Version
	version_prefix = version.Name + '_'

	# set up the file path and name
	file_path = Path(package.Path) / version.Name / f'{version.Name}_{item.Name}.h'
	file_name = file_path.name

	# check if this is only an alias of a previous version, or the release version, and if so, use the previous version's class
	if item.IsReleaseVersion or item.IsIdenticalToPreviousVersion:
		if item.IsReleaseVersion:
			implementing_class_name:str = f'{package.Name}::{item.Name}'
		else:
			implementing_class_name:str = f'{package.Name}::{item.PreviousVersion.Version.Name}::{item.Name}'

		# generate the header file with a forward declaration of the class
		op = formatted_output()
		op.generate_license_header()
		with op.header_guard( file_name=file_name, project_name=package.Name ):
			op.comment_ln(f'class: {item.Name}')
			op.comment_ln(f'version: {version.Name}')
			op.ln('')
			op.ln(f'#include "{item.GetImplementingHeaderFilePath()}"')
			op.ln('')
			with op.ns(package.Name, add_empty_line=False):
				with op.ns(version.Name):
					op.ln(f'using {item.Name} = {implementing_class_name};')
		op.write_lines_to_file(str(file_path))

		# if this is an alias of a previous item, we are done
		if item.IsIdenticalToPreviousVersion:
			return
		
		# if this is the release version, we also need to create the header file with the class definition
		file_path = Path(package.Path) / f'{item.Name}.h'
		file_name = file_path.name
		version_prefix = ''

	# this is not an alias, so we need to create the header file with the class definition
	op = formatted_output()
	op.generate_license_header()
	with op.header_guard( file_name=file_name, project_name=package.Name ):
		op.comment_ln(f'class: {item.Name}')
		op.comment_ln(f'version: {version.Name}')
		op.ln('')

		# include forward declaration of the classes and pds types
		op.ln(f'#include "{item.GetPathToRoot()}{package.Name}.h"')
		#if item.IsModifiedFromPreviousVersion:
		#	op.ln(f'#include "{item.GetPathToPreviousVersion()}"')

		# list dependences that needs to be included in the header
		for dep in item.Dependencies:
			if dep.IncludeInHeader:
				if dep.PDSType:
					op.ln(f'#include <pds/{dep.Name}.h>')
				else:
					op.ln(f'#include "{version_prefix}{dep.Name}.h"')

		op.ln()
		with op.ns(package.Name, add_empty_line=False):
			if item.IsReleaseVersion:
				CreateItemClass(op, item)
			else:
				with op.ns(version.Name):
					CreateItemClass(op, item)
	op.write_lines_to_file(str(file_path))
