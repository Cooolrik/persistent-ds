# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE


import CodeGeneratorHelpers as hlp

def CombinedTypes_h():
	lines = []
	lines.extend( hlp.begin_header_file('value_types.h') )
	lines.append('')
	lines.append('#include <vector>')
	lines.append('')
	lines.append('#include "element_types.h"')
	lines.append('#include "container_types.h"')
	lines.append('')
	lines.append('// Lists information of value types: a combination of data type and container, ')
	lines.append('// even when the container is "none" and the data type is used directly.')
	lines.append('// (Value types are the types used for the values which are used in Items/Entites).')
	lines.append('')
	lines.append('namespace pds')
	lines.append('{')
	lines.append('')
	lines.append('// template to clear a value type')
	lines.append('template <class _Ty> void clear_value_type( _Ty& value_type );')
	lines.append('')

	def generate_type_information( base_type_name , implementing_type , container_type , item_type , num_items_per_object , base_type_combo ):
		lines = []
		lines.append(f'// {base_type_combo}' )
		lines.append(f'template<> struct value_type_information<{base_type_combo}>' )
		lines.append(f'{{' )
		lines.append(f'\ttypedef {implementing_type} data_type;' )
		lines.append(f'\tstatic constexpr element_type_index type_index = element_type_index::dt_{implementing_type}; // the element type index of {base_type_combo} ( dt_{implementing_type} )')
		lines.append(f'\tstatic constexpr container_type_index container_index = container_type_index::ct_{container_type}; // the container type index of {base_type_combo} ( dt_{container_type} )')
		lines.append(f'}};' )
		lines.append(f'')
		return lines
	lines.extend( hlp.function_for_all_basetype_combos( generate_type_information ))

	# end of namespaces
	lines.append('}')
	lines.append('')
	lines.append('#ifdef PDS_IMPLEMENTATION')
	lines.append('#include "value_types.inl"')
	lines.append('#endif//PDS_IMPLEMENTATION')

	lines.extend(hlp.end_header_file("value_types.h"))
	hlp.write_lines_to_file("../Include/pds/value_types.h",lines)

def CombinedTypes_inl():
	lines = []
	lines.extend( hlp.generate_header() )
	lines.append('')
	lines.append('namespace pds')
	lines.append('{')
	lines.append('')

	def generate_clear_value_type( base_type_name , implementing_type , container_type , item_type , num_items_per_object , base_type_combo ):
		lines = []
		if( container_type == 'none' ):
			lines.append(f'template <> void clear_value_type<{base_type_combo}>( {base_type_combo} &type ) {{ type = element_type_information<{base_type_combo}>::zero; }}' )
		elif( container_type == 'vector' or container_type == 'idx_vector' ):
			lines.append(f'template <> void clear_value_type<{base_type_combo}>( {base_type_combo} &type ) {{ type.clear(); }}' )
		else:
			lines.append(f'template <> void clear_value_type<{base_type_combo}>( {base_type_combo} &type ) {{ type.reset(); }}' )
		return lines
	lines.extend( hlp.function_for_all_basetype_combos( generate_clear_value_type ))

	# end of namespace
	lines.append('}')
	hlp.write_lines_to_file("../Include/pds/value_types.inl",lines)
	
def run():
	CombinedTypes_h()
	CombinedTypes_inl()
