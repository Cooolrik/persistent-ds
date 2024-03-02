// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

namespace pds
{

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

// reflection and serialization value types
enum class ValueType
{
	// -----------------------------------------------------------------------------------------------------------------------
	// --- Base value types up to 0x3f use the smaller encoding chunk, and are capped at less than 256 bytes in size

	// --- Base value types, valid range: 0x00 - 0x3f 
	VT_Bool = 0x01, // boolean value, can only be 1 byte in size
	VT_Int = 0x02, // signed integer value, can be 1, 2, 4, and 8 or in size (8, 16, 32 or 64 bits)
	VT_UInt = 0x03, // unsigned integer value, can be 1, 2, 4, or 8 bytes in size (8, 16, 32 or 64 bits)
	VT_Float = 0x04, // floating point value, can be 4 or 8 bytes in size (float or double)
	VT_Vec2 = 0x05, // a 2-component floating point vector, can be 8 or 16 bytes in size (float or double per component)
	VT_Vec3 = 0x06, // a 3-component floating point vector, can be 12 or 24 bytes in size (float or double per component)
	VT_Vec4 = 0x07, // a 4-component floating point vector, can be 16 or 32 bytes in size (float or double per component)
	VT_IVec2 = 0x08, // a 2-component integer vector, can be 2, 4, 8 or 16 bytes in size (8, 16, 32 or 64 bits per component)
	VT_IVec3 = 0x09, // a 3-component integer vector, can be 3, 6, 12 or 24 bytes in size (8, 16, 32 or 64 bits per component) 
	VT_IVec4 = 0x0a, // a 4-component integer vector, can be 4, 8, 16 or 32 bytes in size (8, 16, 32 or 64 bits per component) 
	VT_UVec2 = 0x0b, // a 2-component unsigned integer vector, can be 2, 4, 8 or 16 bytes in size (8, 16, 32 or 64 bits per component) 
	VT_UVec3 = 0x0c, // a 3-component unsigned integer vector, can be 3, 6, 12 or 24 bytes in size (8, 16, 32 or 64 bits per component)
	VT_UVec4 = 0x0d, // a 4-component unsigned integer vector, can be 4, 8, 16 or 32 bytes in size (8, 16, 32 or 64 bits per component)
	VT_Mat2 = 0x0e, // a 2x2 matrix, can be 16 or 32 bytes in size (float or double per component)
	VT_Mat3 = 0x0f, // a 3x3 matrix, can be 36 or 72 bytes in size (float or double per component)
	VT_Mat4 = 0x10, // a 4x4 matrix, can be 64 or 128 bytes in size (float or double per component)
	VT_Quat = 0x11, // a quaternion, can be 16 or 32 bytes in size (float or double per component)
	VT_Uuid = 0x12, // a UUID, universally unique identifier, 16 bytes in size
	VT_Hash = 0x13, // a hash value, 32 bytes in size

	// -----------------------------------------------------------------------------------------------------------------------
	// --- All types 0x40 and up use the larger encoding chunk, and can be up to 2^64 bytes in size

	// --- Array of base value types, valid range: 0x40 - 0x7f
	VT_Array_Bool = 0x41,  // array of VT_Bool 
	VT_Array_Int = 0x42,   // array of VT_Int 
	VT_Array_UInt = 0x43,  // array of VT_UInt 
	VT_Array_Float = 0x44, // array of VT_Float
	VT_Array_Vec2 = 0x45,  // array of VT_Vec2 
	VT_Array_Vec3 = 0x46,  // array of VT_Vec3 
	VT_Array_Vec4 = 0x47,  // array of VT_Vec4 
	VT_Array_IVec2 = 0x48, // array of VT_IVec2
	VT_Array_IVec3 = 0x49, // array of VT_IVec3
	VT_Array_IVec4 = 0x4a, // array of VT_IVec4
	VT_Array_UVec2 = 0x4b, // array of VT_UVec2
	VT_Array_UVec3 = 0x4c, // array of VT_UVec3
	VT_Array_UVec4 = 0x4d, // array of VT_UVec4
	VT_Array_Mat2 = 0x4e,  // array of VT_Mat2 
	VT_Array_Mat3 = 0x4f,  // array of VT_Mat3 
	VT_Array_Mat4 = 0x50,  // array of VT_Mat4 
	VT_Array_Quat = 0x51,  // array of VT_Quat 
	VT_Array_Uuid = 0x52,  // array of VT_Uuid
	VT_Array_Hash = 0x53,  // array of VT_Hash

	// --- Specific types: 0xd0 - 0xff
	VT_Subsection = 0xd0, // a named subsection, containins named values and nested subsections. 
	VT_Array_Subsection = 0xd1, // array of (unnamed) subsections
	VT_String = 0xe0, // a UTF-8 encoded string
	VT_Array_String = 0xe1, // array of strings
};

// enumeration of container types
enum class container_type_index
{
	ct_none = 0x0,
	ct_optional_value = 0x1,
	ct_vector = 0x10,
	ct_optional_vector = 0x11,
	ct_idx_vector = 0x20,
	ct_optional_idx_vector = 0x21,
};

}
// namespace pds
