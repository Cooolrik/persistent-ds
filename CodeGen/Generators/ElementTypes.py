# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

import CodeGeneratorHelpers as hlp
from CodeGeneratorHelpers import int_bit_range, float_bit_range, vector_dimension_range, nonconst_const_range

def fwd_h():
	lines = []
	lines.extend( hlp.begin_header_file( "fwd.h" ) )
	lines.append('')
	lines.append('// fwd.h - Forward declaration of types used in pds')
	lines.append('')
	lines.append('#include <limits.h>')
	lines.append('#include <float.h>')
	lines.append('#include <string>')
	lines.append('#include <vector>')
	lines.append('#include <memory>')	
	lines.append('')
	lines.append('#include <ctle/fwd.h>')
	lines.append('')

	lines.append('namespace pds')
	lines.append('{')
	lines.append('')

	# typedef base integer types
	lines.append(f"// scalar types")
	for bit_size in int_bit_range:
		lines.append(f"typedef std::int{bit_size}_t i{bit_size};")
	for bit_size in int_bit_range:
		lines.append(f"typedef std::uint{bit_size}_t u{bit_size};")
	lines.append('typedef float f32;')
	lines.append('typedef double f64;')
	lines.append('')
	lines.append(f"using std::string;")
	lines.append(f"using std::unique_ptr;")	
	lines.append('')
	lines.append(f"using ctle::status;")
	lines.append(f"template<class _ValTy> using status_return = ctle::status_return<ctle::status, _ValTy>;")	
	lines.append(f"using ctle::uuid;")
	lines.append(f"using hash = ctle::digest<256>;")
	lines.append('')

	# typedef vector types
	lines.append(f"// integer vector types")
	for signed_type in ['i','u']:
		for bit_size in int_bit_range:
			for vec_dim in vector_dimension_range:
				lines.append(f"typedef ctle::n_tup<{signed_type}{bit_size},{vec_dim}> {signed_type}{bit_size}vec{vec_dim};")
		lines.append('')
	lines.append(f"// float vector types")
	for bit_size in float_bit_range:
		for vec_dim in vector_dimension_range:
			lines.append(f"typedef ctle::n_tup<f{bit_size},{vec_dim}> f{bit_size}vec{vec_dim};")
	lines.append('')
	lines.append(f"// float matrix types")
	for bit_size in float_bit_range:
		for vec_dim in vector_dimension_range:
			lines.append(f"typedef ctle::mn_tup<f{bit_size},{vec_dim},{vec_dim}> f{bit_size}mat{vec_dim};")
	lines.append('')

	# typedef quaternions
	lines.append(f"// quaternion types")
	for bit_size in float_bit_range:
		lines.append(f"class f{bit_size}quat;")
	lines.append('')

	# typedef quaternions
	lines.append(f"// item and entity references")
	lines.append('class item_ref;')
	lines.append('class entity_ref;')
	lines.append('')

	# typedef standard precision types (32 bit ints and floats)
	lines.append(f"// shorthand for commonly used types")
	lines.append('typedef u32 uint;')
	for vec_dim in vector_dimension_range:
		lines.append(f"typedef i32vec{vec_dim} ivec{vec_dim};")
	for vec_dim in vector_dimension_range:
		lines.append(f"typedef u32vec{vec_dim} uvec{vec_dim};")
	for vec_dim in vector_dimension_range:
		lines.append(f"typedef f32vec{vec_dim} vec{vec_dim};")
	for vec_dim in vector_dimension_range:
		lines.append(f"typedef f32mat{vec_dim} mat{vec_dim};")
	lines.append('')

	# container types
	lines.append('// container types')
	lines.append('template <class _Ty> using vector = std::vector<_Ty>;')
	lines.append('template <class _Ty> using idx_vector = ctle::idx_vector<_Ty,vector<u32>>;')
	lines.append('template <class _Ty> using optional_idx_vector = ctle::optional_idx_vector<_Ty,vector<u32>>;')
	lines.append('template <class _Ty> using optional_value = ctle::optional_value<_Ty>;')
	lines.append('template <class _Ty> using optional_vector = ctle::optional_vector<_Ty>;')
	lines.append('')	

	# type information on all types
	lines.append('// @brief element_type_information stores information on the element types in pds')
	lines.append('template <class _Ty> struct element_type_information;')
	lines.append('')
	lines.append('// @brief value_type_information stores information on the value types in pds, which is a combination of element types with containers')
	lines.append('template <class _Ty> struct value_type_information;')
	lines.append('')
	lines.append('// @brief container_type_index stores the container type index of the container types in pds')
	lines.append('enum class container_type_index : uint;')
	lines.append('')
	lines.append('// @brief element_type_index stores the element type index in pds')
	lines.append('enum class element_type_index : uint;')
	lines.append('')
	lines.append('// @brief validation_error_flag lists validation error flags in pds')
	lines.append('enum class validation_error_flag : u64;')
	lines.append('')

	# fwd declarations of classes
	lines.append('class Entity;')
	lines.append('class EntityWriter;')
	lines.append('class EntityReader;')
	lines.append('class WriteStream;')
	lines.append('class ReadStream;')
	lines.append('')
	lines.append('// @brief IndexedVector is the template class for all indexed vectors in pds')
	lines.append('template <')
	lines.append('\tclass _Ty, /* the type of the value in the vector */')
	lines.append('\tclass _IdxTy = u32, /* the type of the index in the vector */')
	lines.append('\tclass _Base = ctle::idx_vector<_Ty,std::vector<_IdxTy>> /* the base class of the vector, use to override implementation and allocations etc. */')
	lines.append('> class IndexedVector;')
	lines.append('')

	# end of pds namespace
	lines.append('}')
	lines.append('// namespace pds')

	# end of file
	lines.append('')
	lines.extend( hlp.end_header_file( "fwd.h" ) )
	hlp.write_lines_to_file("../Include/pds/fwd.h",lines)

def print_type_information_header( type , value , value_count ):
	lines = []
	lines.append(f'template<> struct element_type_information<{type}>')
	lines.append( '{')
	lines.append(f'\ttypedef {value} value_type; // the value type of {type} ( {value} )')
	lines.append(f'\tstatic constexpr size_t value_count = {value_count}; // the number of values in {type} ( {value_count} )')
	lines.append(f'\tstatic constexpr const char * value_name = "{value}"; // name of the value in {type} ( "{value}" ) ')
	lines.append(f'\tstatic constexpr const char * type_name = "{type}"; // name of the type ( "{type}" ) ')
	lines.append(f'\tstatic constexpr element_type_index type_index = element_type_index::dt_{type}; // the element type index of {type} ( dt_{type} )')
	lines.append(f'\tstatic const {type} zero; // zero value of {type}')
	lines.append(f'\tstatic const {type} inf; // limit inferior (minimum possible value) of {type}')
	if type != 'string':
		lines.append(f'\tstatic const {type} sup; // limit superior (maximum possible value) of {type}')
	lines.append( '};')
	lines.append('')
	return lines

def element_types_h():
	lines = []
	lines.extend( hlp.begin_header_file( "element_types.h" ) )
	lines.append('')
	lines.append('// element_types.h - Declaration of element types used in pds.')
	lines.append('// Element types are the basic building blocks of the data structures, and can be serialized.')
	lines.append('')
	lines.append('#include <ctle/ntup.h>')
	lines.append('')
	lines.append('#include "entity_ref.h"')
	lines.append('#include "item_ref.h"')
	lines.append('')

	lines.append('namespace pds')
	lines.append('{')
	lines.append('')

	# const min/max values of the standard types
	lines.append('// zero/inf(min)/sup(max) values for the element types')
	lines.append('constexpr bool bool_zero = false;')
	lines.append('constexpr bool bool_inf = false;')
	lines.append('constexpr bool bool_sup = true;')
	for bit_size in int_bit_range:
		lines.append(f"constexpr i{bit_size} i{bit_size}_zero = 0;")
		lines.append(f"constexpr i{bit_size} i{bit_size}_inf = INT{bit_size}_MIN;")
		lines.append(f"constexpr i{bit_size} i{bit_size}_sup = INT{bit_size}_MAX;")
	for bit_size in int_bit_range:
		lines.append(f"constexpr u{bit_size} u{bit_size}_zero = 0;")
		lines.append(f"constexpr u{bit_size} u{bit_size}_inf = 0;")
		lines.append(f"constexpr u{bit_size} u{bit_size}_sup = UINT{bit_size}_MAX;")
	lines.append('')
	lines.append('constexpr f32 f32_zero = 0.0f;')
	lines.append('constexpr f32 f32_inf = -FLT_MAX;')
	lines.append('constexpr f32 f32_sup = FLT_MAX;')
	lines.append('constexpr f64 f64_zero = 0.0;')
	lines.append('constexpr f64 f64_inf = -DBL_MAX;')
	lines.append('constexpr f64 f64_sup = DBL_MAX;')
	lines.append('')

	lines.append('const string string_zero;')
	lines.append('const string string_inf;')
	lines.append('')
	lines.append('constexpr uuid uuid_zero = uuid::zero();')
	lines.append('constexpr uuid uuid_inf = uuid::inf();')
	lines.append('constexpr uuid uuid_sup = uuid::sup();')
	lines.append('')
	lines.append('constexpr hash hash_zero = hash::zero();')
	lines.append('constexpr hash hash_inf = hash::inf();')
	lines.append('constexpr hash hash_sup = hash::sup();')
	lines.append('')

	lines.append(f"// quaternion types")
	for bit_size in float_bit_range:
		lines.append(f"class f{bit_size}quat : public f{bit_size}vec4")
		lines.append('{')
		lines.append('public:')
		lines.append(f'\tf{bit_size}quat() : f{bit_size}vec4() {{}}')
		lines.append(f'\tf{bit_size}quat( const f{bit_size}quat &other ) : f{bit_size}vec4( other ) {{}}')
		lines.append(f'\tf{bit_size}quat( f{bit_size} _a, f{bit_size} _b, f{bit_size} _c, f{bit_size} _d ) : f{bit_size}vec4(_a,_b,_c,_d) {{}}')
		lines.append(f'\tf{bit_size}quat &operator=( const f{bit_size}quat &other ) noexcept {{ x = other.x; y = other.y; z = other.z; w = other.w; return *this; }}')
		lines.append('};')
		lines.append('')

	# enum of all data types
	lines.append('// all element type indices')
	lines.append('enum class element_type_index : uint')
	lines.append('{')
	for basetype_inx in range(len(hlp.base_types)):
		basetype = hlp.base_types[basetype_inx]
		for variant_inx in range(len(basetype.variants)):
			variant_name = basetype.variants[variant_inx].implementing_type
			variant_id = ( (basetype_inx+1) << 4) + (variant_inx + 1)
			lines.append(f'\tdt_{variant_name} = {hex(variant_id)},')
	lines.append('};')
	lines.append('')

	# scalar type info
	lines.extend(print_type_information_header("bool","bool",1))
	for bit_size in int_bit_range:
		lines.extend(print_type_information_header(f"i{bit_size}",f"i{bit_size}",1))
	for bit_size in int_bit_range:
		lines.extend(print_type_information_header(f"u{bit_size}",f"u{bit_size}",1))
	lines.extend(print_type_information_header("f32","f32",1))
	lines.extend(print_type_information_header("f64","f64",1))

	# vector type info
	for bit_size in int_bit_range:
		for vec_dim in vector_dimension_range:
			lines.extend(print_type_information_header(f"i{bit_size}vec{vec_dim}",f"i{bit_size}",vec_dim))
	for bit_size in int_bit_range:
		for vec_dim in vector_dimension_range:
			lines.extend(print_type_information_header(f"u{bit_size}vec{vec_dim}",f"u{bit_size}",vec_dim))	
	for bit_size in float_bit_range:			
		for vec_dim in vector_dimension_range:
			lines.extend(print_type_information_header(f"f{bit_size}vec{vec_dim}",f"f{bit_size}",vec_dim))	

	# matrix type info
	for bit_size in float_bit_range:
		for vec_dim in vector_dimension_range:
			lines.extend(print_type_information_header(f"f{bit_size}mat{vec_dim}",f"f{bit_size}",vec_dim*vec_dim))	

	# quaternions info
	lines.extend(print_type_information_header('f32quat','f32',4))
	lines.extend(print_type_information_header('f64quat','f64',4))

	# uuid info
	lines.extend(print_type_information_header('uuid','uuid',1))
	lines.extend(print_type_information_header('item_ref','item_ref',1))

	# hash info
	lines.extend(print_type_information_header('hash','hash',1))
	lines.extend(print_type_information_header('entity_ref','entity_ref',1))

	# string info
	lines.extend(print_type_information_header('string','string',1))

	# end of pds namespace
	lines.append('}')
	lines.append('// namespace pds')

	lines.append('#ifdef PDS_IMPLEMENTATION')
	lines.append('#include "element_types.inl"')
	lines.append('#endif//PDS_IMPLEMENTATION')
	
	# end of file
	lines.append('')
	lines.extend( hlp.end_header_file( "element_types.h" ) )	
	hlp.write_lines_to_file("../Include/pds/element_types.h",lines)


def print_type_information_source( type , value , value_count ):
	lines = []
	
	zero = inf = sup = ''
	for i in range(value_count):
		zero += f'{value}_zero'
		inf += f'{value}_inf'
		sup += f'{value}_sup'
		if i < value_count-1:
			zero += ','
			inf += ','
			sup += ','

	lines.append(f'\tconst {type} element_type_information<{type}>::zero = {type}({zero}); // zero value of {type}')
	lines.append(f'\tconst {type} element_type_information<{type}>::inf = {type}({inf}); // limit inferior (minimum bound) of {type}')
	if type != 'string':
		lines.append(f'\tconst {type} element_type_information<{type}>::sup = {type}({sup}); // limit superior (maximum bound) of {type}')
	lines.append('')
	return lines

def print_matrix_type_information_source( type , subtype , value , value_count ):
	lines = []
	
	zero = inf = sup = ''
	for t in range(value_count):
		zero += f'{subtype}('		
		inf += f'{subtype}('		
		sup += f'{subtype}('		
		for i in range(value_count):
			zero += f'{value}_zero'
			inf += f'{value}_inf'
			sup += f'{value}_sup'
			if i < value_count-1:
				zero += ','
				inf += ','
				sup += ','
		zero += ')'
		inf += ')'
		sup += ')'
		if t < value_count-1:
			zero += ','
			inf += ','
			sup += ','

	lines.append(f'\tconst {type} element_type_information<{type}>::zero = {type}({zero}); // zero value of {type}')
	lines.append(f'\tconst {type} element_type_information<{type}>::inf = {type}({inf}); // limit inferior (minimum bound) of {type}')
	lines.append(f'\tconst {type} element_type_information<{type}>::sup = {type}({sup}); // limit superior (maximum bound) of {type}')
	lines.append('')
	return lines

def element_types_inl():
	lines = []
	lines.extend( hlp.begin_header_file('element_types.inl') )
	lines.append('')
	lines.append('// element_types.inl - Implementation of element_types constants in in pds.')
	lines.append('// Don''t include this file directly, it is included by element_types.h when implementing pds.')
	lines.append('')
	lines.append('namespace pds')
	lines.append('{')

	# scalar type info
	lines.extend(print_type_information_source("bool","bool",1))
	for bit_size in int_bit_range:
		lines.extend(print_type_information_source(f"i{bit_size}",f"i{bit_size}",1))
	for bit_size in int_bit_range:
		lines.extend(print_type_information_source(f"u{bit_size}",f"u{bit_size}",1))
	for bit_size in float_bit_range:		
		lines.extend(print_type_information_source(f"f{bit_size}",f"f{bit_size}",1))

	# vector type info
	for bit_size in int_bit_range:
		for vec_dim in vector_dimension_range:
			lines.extend(print_type_information_source(f"i{bit_size}vec{vec_dim}",f"i{bit_size}",vec_dim))
	for bit_size in int_bit_range:
		for vec_dim in vector_dimension_range:
			lines.extend(print_type_information_source(f"u{bit_size}vec{vec_dim}",f"u{bit_size}",vec_dim))	
	for bit_size in float_bit_range:
		for vec_dim in vector_dimension_range:
			lines.extend(print_type_information_source(f"f{bit_size}vec{vec_dim}",f"f{bit_size}",vec_dim))	

	# matrix type info
	for bit_size in float_bit_range:
		for vec_dim in vector_dimension_range:
			lines.extend(print_matrix_type_information_source(f"f{bit_size}mat{vec_dim}",f"f{bit_size}vec{vec_dim}",f"f{bit_size}",vec_dim))	

	# quaternions info
	for bit_size in float_bit_range:
		lines.extend(print_type_information_source(f'f{bit_size}quat',f'f{bit_size}',4))

	# other types that are atomic
	same_type_range = ['uuid','entity_ref','hash','item_ref','string']
	for type in same_type_range:
		lines.extend(print_type_information_source(type,type,1))

	lines.append('}')
	lines.append('// namespace pds')

	lines.extend( hlp.end_header_file('element_types.inl') )
	hlp.write_lines_to_file("../Include/pds/element_types.inl",lines)

def element_value_ptrs_h():
	lines = []
	lines.extend( hlp.begin_header_file("element_value_ptrs.h") )
	lines.append('')
	lines.append('#include "element_types.h"')
	lines.append('')

	lines.append('namespace pds')
	lines.append('{')

	# type pointer functions (return pointer to first item in each type)
	lines.append(f"// item pointer functions, returns a pointer to the first item of each type")
	lines.append('')
	for bit_size in int_bit_range:
		for const_type in nonconst_const_range:
			lines.append(f"inline {const_type}i{bit_size} *value_ptr( {const_type}i{bit_size} &value ) {{ return &value; }}")
	lines.append('')
	for bit_size in int_bit_range:
		for const_type in nonconst_const_range:
			lines.append(f"inline {const_type}u{bit_size} *value_ptr( {const_type}u{bit_size} &value ) {{ return &value; }}")
	lines.append('')
	for bit_size in float_bit_range:
		for const_type in nonconst_const_range:
			lines.append(f"inline {const_type}f{bit_size} *value_ptr( {const_type}f{bit_size} &value ) {{ return &value; }}")
	lines.append('')
	for bit_size in int_bit_range:
		for vec_dim in vector_dimension_range:
			for const_type in nonconst_const_range:
				lines.append(f"inline {const_type}i{bit_size} *value_ptr( {const_type}i{bit_size}vec{vec_dim} &value ) {{ return value.data(); }}")
	lines.append('')
	for bit_size in int_bit_range:
		for vec_dim in vector_dimension_range:
			for const_type in nonconst_const_range:
				lines.append(f"inline {const_type}u{bit_size} *value_ptr( {const_type}u{bit_size}vec{vec_dim} &value ) {{ return value.data(); }}")
	lines.append('')

	# vectors
	for bit_size in float_bit_range:	
		for vec_dim in vector_dimension_range:
			for const_type in nonconst_const_range:
				lines.append(f"inline {const_type}f{bit_size} *value_ptr( {const_type}f{bit_size}vec{vec_dim} &value ) {{ return value.data(); }}")
	lines.append('')

	# matrices
	for bit_size in float_bit_range:
		for vec_dim in vector_dimension_range:
			for const_type in nonconst_const_range:
				lines.append(f"inline {const_type}f{bit_size} *value_ptr( {const_type}f{bit_size}mat{vec_dim} &value ) {{ return value.data(); }}")
	lines.append('')

	# quaternions
	for bit_size in float_bit_range:	
		for const_type in nonconst_const_range:
			lines.append(f"inline {const_type}f{bit_size} *value_ptr( {const_type}f{bit_size}quat &value ) {{ return value.data(); }}")
	lines.append('')

	# other types that have no inner item pointer
	same_type_range = ['uuid','hash','string']
	for type in same_type_range:
		for const_type in nonconst_const_range:
			lines.append(f"inline {const_type}{type} *value_ptr( {const_type}{type} &value ) {{ return &value; }}")

	# end of namespace
	lines.append('}')
	lines.append('// namespace pds')
	lines.append('')
	
	lines.extend(hlp.end_header_file("element_value_ptrs.h"))
	hlp.write_lines_to_file("../Include/pds/element_value_ptrs.h",lines)

def run():
	fwd_h()
	element_types_h()
	element_types_inl()
	element_value_ptrs_h()
