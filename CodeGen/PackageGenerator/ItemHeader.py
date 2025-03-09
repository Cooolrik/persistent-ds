from EntitiesHelpers import * 
import os
import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, vector_dimension_range, nonconst_const_range
from ctle_code_gen.formatted_output import formatted_output

from ctypes import c_ulonglong 
from ctypes import c_ubyte

def CreateItemHeader(item: Item):
	packageName = item.Package.Name
	versionName = item.Version.Name

	lines = []
	lines.extend( hlp.generate_header() )
	lines.append('')
	lines.append('#pragma once')
	lines.append('')
	lines.append(f'#include "../pdsImportsAndDefines.h"')

	# is this an alias of a previous version?
	if item.IdenticalToPreviousVersion:
		previousVersionName = item.PreviousVersion.Version.Name

		# if this is just an alias, define a using and reference back to the actual entity
		lines.append(f'#include "../{previousVersionName}/{previousVersionName}_{item.Name}.h"')
		lines.append('')
		lines.append(f'namespace {packageName}')
		lines.append('{')
		lines.append(f'namespace {versionName}')
		lines.append('{')
		lines.append(f'// {item.Name} is identical to the previous version {previousVersionName}')
		lines.append(f'using {item.Name} = {previousVersionName}::{item.Name};')
		lines.append('}')
		lines.append(f'// namespace {versionName}')
		lines.append('}')		
		lines.append(f'// namespace {packageName}')
		
	else:
		# not an alias, defined the whole class
		if item.IsModifiedFromPreviousVersion:
			previousVersionName = item.PreviousVersion.Version.Name
			lines.append(f'#include "../{previousVersionName}/{previousVersionName}_{item.Name}.h"')
		
		# list dependences that needs to be included in the header
		for dep in item.Dependencies:
			if dep.IncludeInHeader:
				if dep.PDSType:
					lines.append(f'#include <pds/{dep.Name}.h>')
				else:
					lines.append(f'#include "{versionName}_{dep.Name}.h"')				

		lines.append('')
		lines.append(f'namespace {packageName}')
		lines.append('{')
		lines.append(f'namespace {versionName}')
		lines.append('{')

		# list dependences that only needs a forward reference in the header
		for dep in item.Dependencies:
			if not dep.IncludeInHeader:
				lines.append(f'    class {dep.Name};')

		if item.IsEntity:
			lines.append(f'    class {item.Name} : public pds::Entity')
		else:
			lines.append(f'    class {item.Name}')
		lines.append('        {')
		lines.append('        public:')

		# list typedefs of templates
		if len(item.Templates) > 0:
			for typ in item.Templates:
				lines.append(typ.Declaration)
			lines.append('')

		lines.append('            class MF;')
		lines.append('            friend MF;')
		lines.append('')
		lines.append(f'            static constexpr const char *ItemTypeString = "{packageName}.{versionName}.{item.Name}";')
		lines.append('')
		
		if item.IsEntity:
			lines.append(f'            virtual const char *EntityTypeString() const;')
			lines.append('')

		lines.append(f'            {item.Name}() = default;')
		lines.append(f'            {item.Name}( const {item.Name} &rval );')
		lines.append(f'            {item.Name} &operator=( const {item.Name} &rval );')
		lines.append(f'            {item.Name}( {item.Name} &&rval ) = default;')
		lines.append(f'            {item.Name} &operator=( {item.Name} &&rval ) = default;')
		if item.IsEntity:
			lines.append(f'            virtual ~{item.Name}() = default;')
		else:
			lines.append(f'            ~{item.Name}() = default;')
		lines.append('')

		lines.append('            // value compare operators')
		lines.append(f'            bool operator==( const {item.Name} &rval ) const;')
		lines.append(f'            bool operator!=( const {item.Name} &rval ) const;')
		lines.append('')

		lines.append('        protected:')
		
		# list variables in item
		for var in item.Variables:
			if var.IsSimpleBaseType:
				lines.append(f'            {var.TypeString} v_{var.Name} = {{}};')
			else:
				lines.append(f'            {var.TypeString} v_{var.Name};')

		lines.append('')
		lines.append('        public:')

		# create accessor ref for variables, const and non-const versions
		for var in item.Variables:
			lines.append(f'            // accessor for referencing variable {var.Name}')
			lines.append(f'            const {var.TypeString} & {var.Name}() const {{ return this->v_{var.Name}; }}')
			lines.append(f'            {var.TypeString} & {var.Name}() {{ return this->v_{var.Name}; }}')
			lines.append('')

		lines.append('        };')

		lines.append('}')
		lines.append(f'// namespace {versionName}')
		lines.append('}')		
		lines.append(f'// namespace {packageName}')
  
	hlp.write_lines_to_file(f"{item.Package.Path}/{versionName}/{versionName}_{item.Name}.h",lines)
