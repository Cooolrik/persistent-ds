# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

import copy
import os
from stat import S_IRUSR, S_IRGRP, S_IROTH, S_IWUSR
import importlib
from pathlib import Path
import shutil
import re

int_bit_range = [8,16,32,64]
float_bit_range = [32,64]
vector_dimension_range = [2,3,4] 
nonconst_const_range = ['','const ']

class BaseType:
	def __init__(self,name,variants):
		self.name = name
		self.variants = variants

class BaseTypeVariant:
	def __init__(self,implementing_type,item_type,num_items_per_object,override=False):
		self.implementing_type = implementing_type
		self.item_type = item_type
		self.num_items_per_object = num_items_per_object
		self.overrides_type = override

class ContainerType:
	def __init__(self,container_id,implementing_type,is_template = True):
		self.container_id = container_id
		self.implementing_type = implementing_type
		self.is_template = is_template

base_type_bool = BaseType('bool',[BaseTypeVariant('bool','bool',1)])
base_type_int =  BaseType('int',[BaseTypeVariant('i8','i8',1),BaseTypeVariant('i16','i16',1),BaseTypeVariant('i32','i32',1),BaseTypeVariant('i64','i64',1)])
base_type_uint =  BaseType('uint',[BaseTypeVariant('u8','u8',1),BaseTypeVariant('u16','u16',1),BaseTypeVariant('u32','u32',1),BaseTypeVariant('u64','u64',1)])
base_type_float =  BaseType('float',[BaseTypeVariant('f32','f32',1),BaseTypeVariant('f64','f64',1)])
base_type_vec2 =  BaseType('vec2',[BaseTypeVariant('f32vec2','f32',2),BaseTypeVariant('f64vec2','f64',2)]) 
base_type_vec3 =  BaseType('vec3',[BaseTypeVariant('f32vec3','f32',3),BaseTypeVariant('f64vec3','f64',3)])  
base_type_vec4 =  BaseType('vec4',[BaseTypeVariant('f32vec4','f32',4),BaseTypeVariant('f64vec4','f64',4)])  
base_type_ivec2 =  BaseType('ivec2',[BaseTypeVariant('i8vec2','i8',2),BaseTypeVariant('i16vec2','i16',2),BaseTypeVariant('i32vec2','i32',2),BaseTypeVariant('i64vec2','i64',2)])    
base_type_ivec3 =  BaseType('ivec3',[BaseTypeVariant('i8vec3','i8',3),BaseTypeVariant('i16vec3','i16',3),BaseTypeVariant('i32vec3','i32',3),BaseTypeVariant('i64vec3','i64',3)])    
base_type_ivec4 =  BaseType('ivec4',[BaseTypeVariant('i8vec4','i8',4),BaseTypeVariant('i16vec4','i16',4),BaseTypeVariant('i32vec4','i32',4),BaseTypeVariant('i64vec4','i64',4)])    
base_type_uvec2 =  BaseType('uvec2',[BaseTypeVariant('u8vec2','u8',2),BaseTypeVariant('u16vec2','u16',2),BaseTypeVariant('u32vec2','u32',2),BaseTypeVariant('u64vec2','u64',2)])    
base_type_uvec3 =  BaseType('uvec3',[BaseTypeVariant('u8vec3','u8',3),BaseTypeVariant('u16vec3','u16',3),BaseTypeVariant('u32vec3','u32',3),BaseTypeVariant('u64vec3','u64',3)])    
base_type_uvec4 =  BaseType('uvec4',[BaseTypeVariant('u8vec4','u8',4),BaseTypeVariant('u16vec4','u16',4),BaseTypeVariant('u32vec4','u32',4),BaseTypeVariant('u64vec4','u64',4)])    
base_type_mat2 =  BaseType('mat2',[BaseTypeVariant('f32mat2','f32',4),BaseTypeVariant('f64mat2','f64',4)])    
base_type_mat3 =  BaseType('mat3',[BaseTypeVariant('f32mat3','f32',9),BaseTypeVariant('f64mat3','f64',9)])    
base_type_mat4 =  BaseType('mat4',[BaseTypeVariant('f32mat4','f32',16),BaseTypeVariant('f64mat4','f64',16)])
base_type_quat =  BaseType('quat',[BaseTypeVariant('f32quat','f32',4),BaseTypeVariant('f64quat','f64',4)])
base_type_uuid =  BaseType('uuid',[BaseTypeVariant('uuid','uuid',1),BaseTypeVariant('item_ref','uuid',1,True)])
base_type_hash =  BaseType('hash',[BaseTypeVariant('hash','hash',1),BaseTypeVariant('entity_ref','hash',1,True)])
base_type_string = BaseType('string',[BaseTypeVariant('string','string',1)])

base_types = [base_type_bool,
			  base_type_int,
			  base_type_uint,
			  base_type_float,
			  base_type_vec2,
			  base_type_vec3,
			  base_type_vec4,
			  base_type_ivec2,
			  base_type_ivec3,
			  base_type_ivec4,
			  base_type_uvec2,
			  base_type_uvec3,
			  base_type_uvec4,
			  base_type_mat2,
			  base_type_mat3,
			  base_type_mat4,
			  base_type_quat,
			  base_type_uuid,
			  base_type_hash,
			  base_type_string]

container_types = [ContainerType(0x00,'none',False),
				   ContainerType(0x01,'optional_value'),
				   ContainerType(0x10,'vector'),
				   ContainerType(0x11,'optional_vector'),
				   ContainerType(0x20,'idx_vector'),
				   ContainerType(0x21,'optional_idx_vector')]

# find a base type based on name, and return base_type and base_type_variant info
def get_base_type_variant( name ):
	for typ in base_types:
		for var in typ.variants:
			if name == var.implementing_type:
				return typ, var
	return None,None

# print all lines using all items in list, with all base types and all variants (including optional variants), as well as all vector versions of base types
def generate_lines_for_all_basetype_combos( line_list ):
	lines = []
	for basetype in base_types:
		for var in basetype.variants:
			for cont in container_types:
				for line in line_list:
					if( cont.is_template ):
						base_type_container_combo = f'{cont.implementing_type}<{var.implementing_type}>'
					else:
						base_type_container_combo = var.implementing_type
					lines.append( 
						line.format( 
							base_type_name = basetype.name , 
							implementing_type = var.implementing_type , 
							container_type = cont.implementing_type , 
							item_type = var.item_type , 
							num_items_per_object = var.num_items_per_object , 
							base_type_combo = base_type_container_combo 
							) 
						)
	return lines

# call function with all base types and all variants (including optional variants), as well as all vector versions of base types
def function_for_all_basetype_combos( line_function ):
	lines = []
	for basetype in base_types:
		for var in basetype.variants:
			for cont in container_types:
				if( cont.is_template ):
					base_type_container_combo = f'{cont.implementing_type}<{var.implementing_type}>'
				else:
					base_type_container_combo = var.implementing_type
				lines.extend( 
					line_function( 
						base_type_name = basetype.name , 
						implementing_type = var.implementing_type , 
						container_type = cont.implementing_type , 
						item_type = var.item_type , 
						num_items_per_object = var.num_items_per_object , 
						base_type_combo = base_type_container_combo 
						) 
					)
	return lines

# reads a file and output lines
def inline_file( path ):
	inlined_file = open(path, 'r')
	lines = inlined_file.readlines()
	strip_lines = []
	for str in lines:
		strip_lines.append( str.rstrip() )
	inlined_file.close()
	return strip_lines

def camel_to_snake(name):
    name = re.sub(r'([a-zA-Z])([0-9])', r'\1_\2', name)  # Separate letters and numbers
    name = re.sub(r'([a-z])([A-Z])', r'\1_\2', name) # Separate camel case with underscore
    return name.lower() # to lower case all

def generate_header():
	lines = []
	lines.append('// WARNING! DO NOT EDIT THIS FILE! This file is generated.')
	lines.append('')
	lines.append('// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl')
	lines.append('// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE')
	return lines

def begin_header_file( filename:str ) -> list:
	lines = []
	lines.extend( generate_header() )
	lines.append(f'#pragma once')
	def_name = filename.replace('.','_').upper()
	lines.append(f'#ifndef __PDS__{def_name}__')
	lines.append(f'#define __PDS__{def_name}__')
	return lines

def end_header_file( filename:str ) -> list:
	lines = []
	def_name = filename.replace('.','_').upper()
	lines.append(f'#endif//__PDS__{def_name}__')
	return lines

def generate_push_and_disable_warnings( vswarns , gccwarns , comment = 'disable warnings in code we cannot control' ):
	lines = []
	lines.append(f'// {comment}')
	lines.append('#ifdef _MSC_VER')
	lines.append('#pragma warning( push )')
	for w in vswarns:
		lines.append(f'#pragma warning( disable : {w} )')
	lines.append('#elif defined(__GNUC__)')
	lines.append('#pragma GCC diagnostic push')
	for w in gccwarns:
		lines.append(f'#pragma GCC diagnostic ignored "{w}"')	
	lines.append('#endif')
	return lines

def generate_pop_warnings( comment = 're-enable warnings again' ):
	lines = []
	lines.append(f'// {comment}')
	lines.append('#ifdef _MSC_VER')
	lines.append('#pragma warning( pop )')
	lines.append('#elif defined(__GNUC__)')
	lines.append('#pragma GCC diagnostic pop')
	lines.append('#endif')
	return lines

def import_ctle_code_gen( ctle_path:str = '../build/_deps/ctle-src' ):
	if not (Path.cwd() / 'code_gen' / 'ctlepy').exists():
		
		if ctle_path == '':
			ctle_path = Path.cwd() / '../build/_deps/ctle-src'
		else:
			ctle_path = Path(ctle_path)
			if not ctle_path.is_absolute():
				ctle_path = Path.cwd() / ctle_path
		
		ctlepy_path = ctle_path / 'code_gen' / 'ctlepy'
		
		if ctlepy_path.exists():
			print( f'Copying {ctlepy_path} to local ctlepy copy.')
			shutil.copytree(ctlepy_path, 'ctlepy', dirs_exist_ok=True)
		else:
			print( f'Error: the ctle code gen path: {ctlepy_path} does not exist. Cannot continue' )
			exit(-1)
	

def run_module( name , *args ):
	print('Running: ' + name )
	importlib.import_module( name ).run( *args )
	print('')

def write_lines_to_file( path , lines ):
	# make into one long string
	new_text = ''
	for line in lines:
		new_text += line + '\n'

	# if the file already exists, check if we have an update
	if os.path.exists(path):
		# try reading in the current file if one exists
		with open(path,'r') as f:
			existing_text = f.read()
			f.close()

		#if no difference was found, return
		if new_text == existing_text:
			print( '\tSkipping: ' + path + ', (it is identical)...')
			return

		# if a difference was found, remove the old file
		os.chmod(path, S_IWUSR)
		os.remove( path ) 

	# we should write to the new file
	print( '\tWriting: ' + path + '...')
	with open(path,'w') as f:
		f.write(new_text)
		f.close()

	# change mode of file to readonly
	os.chmod(path, S_IRUSR|S_IRGRP|S_IROTH)