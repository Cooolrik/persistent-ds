from EntitiesHelpers import * 
import os
import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, vector_dimension_range, nonconst_const_range
from ctlepy.formatted_output import formatted_output
from pathlib import Path

from ctypes import c_ulonglong 
from ctypes import c_ubyte

def CreateItemMFClass(op: formatted_output, item: Item) -> None:
	package = item.Package
	version = item.Version

	op.ln(f'class {item.Name}::MF')
	with op.blk( no_indent=True, add_semicolon=True ):
		op.ln('public:')
		with op.tab():
			op.ln(f'static status Clear( {item.Name} &obj );')
			op.ln(f'static status DeepCopy( {item.Name} &dest, const {item.Name} *source );')
			op.ln(f'static bool Equals( const {item.Name} *lvar, const {item.Name} *rvar );')
			op.ln('')
			op.ln(f'static status Write( const {item.Name} &obj, pds::EntityWriter &writer );')
			op.ln(f'static status Read( {item.Name} &obj, pds::EntityReader &reader );')
			op.ln('')
			op.ln(f'static status Validate( const {item.Name} &obj, pds::EntityValidator &validator );')
			op.ln('')
			if item.IsEntity:
				op.ln(f'static const {item.Name} *EntitySafeCast( const pds::Entity *srcEnt );')
				op.ln(f'static std::shared_ptr<const {item.Name}> EntitySafeCast( std::shared_ptr<const pds::Entity> srcEnt );')
				op.ln('')
			if item.IsModifiedFromPreviousVersion:
				op.ln(f'static status ToPrevious( {item.PreviousVersion.Version.Name}::{item.Name} &dest , const {item.Name} &source );')
				op.ln(f'static status FromPrevious( {item.Name} &dest , const {item.PreviousVersion.Version.Name}::{item.Name} &source );')
				op.ln('')
	op.ln('')


def CreateItemMFHeader(item: Item):
	package = item.Package
	version = item.Version

	# set up the file path and name
	file_path = Path(package.Path) / version.Name / f'{version.Name}_{item.Name}_MF.h'
	file_name = file_path.name

	# only implement MF if a new version
	if item.IsDeleted or item.IsIdenticalToPreviousVersion:
		return

	op = formatted_output()
	op.generate_license_header()
	with op.header_guard( file_name=file_name, project_name=package.Name ):
		op.comment_ln(f'class: {item.Name}')
		op.comment_ln(f'version: {version.Name}')
		op.ln('')

		op.ln(f'#include "{item.GetImplementingHeaderFilePath()}"')

		# list dependences that needs to be included in the header
		for dep in item.Dependencies:
			if dep.IncludeInHeader:
				if dep.PDSType:
					op.ln(f'#include <pds/mf/{dep.Name}_MF.h>')
				else:
					op.ln(f'#include "{version.Name}_{dep.Name}_MF.h"')			
					
		op.ln('')
		with op.ns(package.Name, add_empty_line=False):
			if item.IsReleaseVersion:
				op.ln('')
				CreateItemMFClass(op, item)
			else:
				with op.ns(version.Name):
					op.ln('')
					CreateItemMFClass(op, item)

	op.write_lines_to_file(str(file_path))
