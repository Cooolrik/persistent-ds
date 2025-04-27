// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__FILEOPS_COMMON_H__
#define __PDS__FILEOPS_COMMON_H__

// fileops_common.h - has values shared by read and write functions 

namespace pds
{

// The maximum length of a subchunk key in pds. Keys are recommended to be as short as possible, for space and performance.
constexpr const u8 EntityMaxKeyLength = 40;

// A Note on how types are either stored in small encoding chunks, or large encoding chunks in the binary files:
// 
// Small encoding chunks, for any valuetype < 0x40:
// * Small values of fixed maximum length, where the payload + length of the key is < 256 bytes
// * Used mainly for single items of base types: bools, ints, floats, vec2s, vec3s, vec4s, UUIDs
// * Layout:
//		u8 Type; // types 0x00 - 0x3f
//		u8 SizeInBytes; // can be used to skip value if not recognized
//		u8 Value[]; // <- defined size, based on Type (or with extra info inside, if variable size)
//		u8 KeyData[]; // the key of the value (EntityMaxKeyLength is the max length of any key)
// 
// Large encoding chunks, for any valuetype >= 0x40 
// * Uses a u64 to define size, so basically any useful size
// * Used for:
//		- Nested chunks
//		- Arrays of values
//		- Strings (UTF-8 encoded)
//		- Data that would not fit in 256 bytes
// * Layout:
//		u8 Type; // types 0x40 - 0xff
//		u64 SizeInBytes; // to skip over rest of this block, size after this value  
//		u8 KeySizeInBytes; // the size of the key of the value (EntityMaxKeyLength is the max length of any key)
//		u8 KeyData[]; // the key of the value 
//		u8 Value[]; // <- defined size, equal to the rest of SizeInBytes after the key data ( sizeof(KeySizeInBytes)=1 + KeySizeInBytes bytes) 

// encoding value for serialized types
enum class serialization_type_index : u8
{
	// -----------------------------------------------------------------------------------------------------------------------
	// --- Base value types up to 0x3f use the smaller encoding chunk, and are capped at less than 256 bytes in size

	// --- base value types, valid range: 0x00 - 0x3f 
	vt_bool = 0x01, // boolean value, can only be 1 byte in size
	vt_int = 0x02, // signed integer value, can be 1, 2, 4, and 8 or in size (8, 16, 32 or 64 bits)
	vt_uint = 0x03, // unsigned integer value, can be 1, 2, 4, or 8 bytes in size (8, 16, 32 or 64 bits)
	vt_float = 0x04, // floating point value, can be 4 or 8 bytes in size (float or double)
	vt_vec2 = 0x05, // a 2-component floating point vector, can be 8 or 16 bytes in size (float or double per component)
	vt_vec3 = 0x06, // a 3-component floating point vector, can be 12 or 24 bytes in size (float or double per component)
	vt_vec4 = 0x07, // a 4-component floating point vector, can be 16 or 32 bytes in size (float or double per component)
	vt_ivec2 = 0x08, // a 2-component integer vector, can be 2, 4, 8 or 16 bytes in size (8, 16, 32 or 64 bits per component)
	vt_ivec3 = 0x09, // a 3-component integer vector, can be 3, 6, 12 or 24 bytes in size (8, 16, 32 or 64 bits per component) 
	vt_ivec4 = 0x0a, // a 4-component integer vector, can be 4, 8, 16 or 32 bytes in size (8, 16, 32 or 64 bits per component) 
	vt_uvec2 = 0x0b, // a 2-component unsigned integer vector, can be 2, 4, 8 or 16 bytes in size (8, 16, 32 or 64 bits per component) 
	vt_uvec3 = 0x0c, // a 3-component unsigned integer vector, can be 3, 6, 12 or 24 bytes in size (8, 16, 32 or 64 bits per component)
	vt_uvec4 = 0x0d, // a 4-component unsigned integer vector, can be 4, 8, 16 or 32 bytes in size (8, 16, 32 or 64 bits per component)
	vt_mat2 = 0x0e, // a 2x2 matrix, can be 16 or 32 bytes in size (4-component, float or double per component)
	vt_mat3 = 0x0f, // a 3x3 matrix, can be 36 or 72 bytes in size (9-component, float or double per component)
	vt_mat4 = 0x10, // a 4x4 matrix, can be 64 or 128 bytes in size (16-component, float or double per component)
	vt_quat = 0x11, // a quaternion, can be 16 or 32 bytes in size (4-component, float or double per component)
	vt_uuid = 0x12, // a uuid, universally unique identifier, 16 bytes in size
	vt_hash = 0x13, // a hash value, 32 bytes in size

	// -----------------------------------------------------------------------------------------------------------------------
	// --- all types 0x40 and up use the larger encoding chunk, and can be up to 2^64 bytes in size

	// --- array of base value types, valid range: 0x40 - 0x7f
	vt_array_bool = 0x41,  // array of vt_bool 
	vt_array_int = 0x42,   // array of vt_int 
	vt_array_uint = 0x43,  // array of vt_uint 
	vt_array_float = 0x44, // array of vt_float
	vt_array_vec2 = 0x45,  // array of vt_vec2 
	vt_array_vec3 = 0x46,  // array of vt_vec3 
	vt_array_vec4 = 0x47,  // array of vt_vec4 
	vt_array_ivec2 = 0x48, // array of vt_ivec2
	vt_array_ivec3 = 0x49, // array of vt_ivec3
	vt_array_ivec4 = 0x4a, // array of vt_ivec4
	vt_array_uvec2 = 0x4b, // array of vt_uvec2
	vt_array_uvec3 = 0x4c, // array of vt_uvec3
	vt_array_uvec4 = 0x4d, // array of vt_uvec4
	vt_array_mat2 = 0x4e,  // array of vt_mat2 
	vt_array_mat3 = 0x4f,  // array of vt_mat3 
	vt_array_mat4 = 0x50,  // array of vt_mat4 
	vt_array_quat = 0x51,  // array of vt_quat 
	vt_array_uuid = 0x52,  // array of vt_uuid
	vt_array_hash = 0x53,  // array of vt_hash

	// --- specific types: 0xd0 - 0xff
	vt_subsection = 0xd0, // a named subsection, containins named values and nested subsections. 
	vt_array_subsection = 0xd1, // array of (unnamed) subsections
	vt_string = 0xe0, // a utf-8 encoded string
	vt_array_string = 0xe1, // array of strings
};

}
// namespace pds

#endif//__PDS__FILEOPS_COMMON_H__