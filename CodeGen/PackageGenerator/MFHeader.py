from EntitiesHelpers import * 
import os
import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, float_type_range, vector_dimension_range, nonconst_const_range
from ctle_code_gen.formatted_output import formatted_output

from ctypes import c_ulonglong 
from ctypes import c_ubyte

def CreateMFHeader(item: Item):
	packageName = item.Package.Name
	versionName = item.Version.Name

	o = formatted_output()
	o.ln( o.generate_license_header( project_name='pds', copyright_holder='2024 Ulrik Lindahl', license_link='https://github.com/Cooolrik/pds' ) )
	o.ln('')
	o.ln('#pragma once')
	o.ln('')
	o.ln(f'#include "{versionName}_{item.Name}.h"')
	o.ln('')

	# only implement MF if a new version
	if not item.IdenticalToPreviousVersion:

		# list dependences that needs to be included in the header
		for dep in item.Dependencies:
			if dep.IncludeInHeader:
				if dep.PDSType:
					o.ln(f'#include <pds/{dep.Name}_MF.h>')
				else:
					o.ln(f'#include "{versionName}_{dep.Name}_MF.h"')			

		o.ln(f'namespace {packageName}')
		o.ln('{')
		o.ln(f'namespace {versionName}')
		o.ln('{')
		o.ln('')

		o.ln(f'class {item.Name}::MF')
		with o.blk():
			o.ln('public:')
			with o.tab():
				o.ln(f'static status Clear( {item.Name} &obj );')
				o.ln(f'static status DeepCopy( {item.Name} &dest, const {item.Name} *source );')
				o.ln(f'static bool Equals( const {item.Name} *lvar, const {item.Name} *rvar );')
				o.ln('')
				o.ln(f'static status Write( const {item.Name} &obj, pds::EntityWriter &writer );')
				o.ln(f'static status Read( {item.Name} &obj, pds::EntityReader &reader );')
				o.ln('')
				o.ln(f'static status Validate( const {item.Name} &obj, pds::EntityValidator &validator );')
				o.ln('')
				if item.IsEntity:
					o.ln(f'static const {item.Name} *EntitySafeCast( const pds::Entity *srcEnt );')
					o.ln(f'static std::shared_ptr<const {item.Name}> EntitySafeCast( std::shared_ptr<const pds::Entity> srcEnt );')
					o.ln('')
				if item.IsModifiedFromPreviousVersion:
					o.ln(f'static status ToPrevious( {item.PreviousVersion.Version.Name}::{item.Name} &dest , const {item.Name} &source );')
					o.ln(f'static status FromPrevious( {item.Name} &dest , const {item.PreviousVersion.Version.Name}::{item.Name} &source );')
					o.ln('')
		o.ln('')
		
		## ctors and copy operator code
		#o.ln(f'    inline {item.Name}::{item.Name}( const {item.Name} &rval )')
		#o.ln('        {')
		#o.ln('        MF::DeepCopy( *this , &rval );')
		#o.ln('        }')
		#o.ln('')
		#o.ln(f'    inline {item.Name} &{item.Name}::operator=( const {item.Name} &rval )')
		#o.ln('        {')
		#o.ln('        MF::DeepCopy( *this , &rval );')
		#o.ln('        return *this;')
		#o.ln('        }')
		#o.ln('')
		#o.ln(f'    inline bool {item.Name}::operator==( const {item.Name} &rval ) const')
		#o.ln('        {')
		#o.ln('        return MF::Equals( this, &rval );')
		#o.ln('        }')
		#o.ln('')
		#o.ln(f'    inline bool {item.Name}::operator!=( const {item.Name} &rval ) const')
		#o.ln('        {')
		#o.ln('        return !(MF::Equals( this, &rval ));')
		#o.ln('        }')
		#o.ln('')

		o.ln('}')
		o.ln(f'// namespace {versionName}')
		o.ln('}')		
		o.ln(f'// namespace {packageName}')
  
	o.write_lines_to_file(f'{item.Package.Path}/{versionName}/{versionName}_{item.Name}_MF.h')
