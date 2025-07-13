// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__READER_TEMPLATES_H__
#define __PDS__READER_TEMPLATES_H__


#include <ctle/log.h>

#include "fileops_common.h"
#include "element_value_ptrs.h"
#include "ReadStream.h"

// value_type: the serialization_type_index enum to read the block as
// object_type: the C++ object that stores the data (can be a basic type), such as u32, or glm::vec3
// item_type: the actual basic type that stores the data, int the case of glm::vec3, it is a float
// item_count: the count of the the basic type, again in the case of glm::vec3, the count is 3

namespace pds
{
#include "_pds_macros.inl"

enum class reader_status
{
	fail, // general fail, (or value is empty, but empty is not allowed for this value)
	success_empty, // success, value is empty
	success // success, has value
};

// read the header of a large block
// returns the stream position of the expected end of the block, to validate the read position
// a stream position of 0 is not possible, and indicates error
inline u64 begin_read_large_block( ReadStream &sstream, serialization_type_index VT, const char *key, const u8 key_size_in_bytes )
{
	ctSanityCheck( key_size_in_bytes <= EntityMaxKeyLength ); // max key length

	// read and make sure we have the correct value type
	const u8 value_type = sstream.Read<u8>();
	if( value_type != (u8)VT )
	{
		// not the expected type
		ctLogError << "The type in the input stream:" << (u32)value_type << " does not match expected type: " << (u32)VT << ctLogEnd;
		return 0;
	}

	// check the size, and calculate expected end position
	const u64 block_size = sstream.Read<u64>();
	const u64 expected_end_pos = sstream.GetPosition() + block_size;
	if( expected_end_pos > sstream.GetSize() )
	{
		// not the expected type
		ctLogError << "The block size:" << block_size << " points beyond the end of the stream size" << ctLogEnd;
		return 0;
	}

	// read in the key length
	const u8 read_key_size_in_bytes = sstream.Read<u8>();
	if( read_key_size_in_bytes != key_size_in_bytes )
	{
		// not the expected type
		std::string expected_key_name( key, key_size_in_bytes );
		ctLogError << "The size of the input key:" << (u32)read_key_size_in_bytes << " does not match expected size: " << (u32)key_size_in_bytes << " for key: \"" << expected_key_name << "\"" << ctLogEnd;
		return 0;
	}

	// read in the key data
	char read_key[EntityMaxKeyLength];
	sstream.Read( (i8 *)read_key, (u64)key_size_in_bytes );
	if( memcmp( key, read_key, (u64)key_size_in_bytes ) != 0 )
	{
		std::string expected_key_name( key, key_size_in_bytes );
		std::string read_key_name( read_key, key_size_in_bytes );
		ctLogError << "Unexpected key name in the stream. Expected name: " << expected_key_name << " read name: " << read_key_name << ctLogEnd;
		return 0;
	}

	return expected_end_pos;
}

// ends the block, write the size of the block
inline bool end_read_large_block( ReadStream &sstream, u64 expected_end_pos )
{
	const u64 end_pos = sstream.GetPosition();
	return ( end_pos == expected_end_pos ); // make sure we have read in the full block
}

// template method that Reads a small block of a specific serialization_type_index VT to the stream. Since most value types 
// can have different bit depths, the second parameter I is the actual type of the data stored. The data can have more than one values of type I, the count is stored in IC.
template<serialization_type_index VT, class T> inline reader_status read_single_item( ReadStream &sstream, const char *key, const u8 key_size_in_bytes, const bool empty_value_is_allowed, T *dest_data )
{
	static_assert( ( VT >= serialization_type_index::vt_bool ) && ( VT <= serialization_type_index::vt_hash ), "Invalid type for generic template of read_single_item" );

	const u64 value_size = sizeof( typename element_type_information<T>::value_type );
	const u64 value_count = element_type_information<T>::value_count;

	// record start position, for validation
	const u64 start_pos = sstream.GetPosition();

	// read u8 value_type, check the value
	const u8 value_type = sstream.Read<u8>();
	if( value_type != (u8)VT )
	{
		// not the expected type
		ctLogError << "The type in the input stream:" << (u32)value_type << " does not match expected type: " << (u32)VT << ctLogEnd;
		return reader_status::fail;
	}

	// calc the expected possible sizes. if empty value, the data size must be 0, else it is the expected size based on the item type (I) and count (IC)
	const u64 dest_data_size_in_bytes = value_size * value_count;
	const u64 expected_block_size_if_empty = key_size_in_bytes;
	const u64 expected_block_size = dest_data_size_in_bytes + expected_block_size_if_empty;

	ctSanityCheck( key_size_in_bytes <= EntityMaxKeyLength );
	ctSanityCheck( expected_block_size < 256 ); // must fit in a byte

	// read in size of the small block, if the size does not match the expected block size, check if empty value is ok (is_optional_value == true), and if not raise error
	// any size other than expected_block_size is regarded as empty, and we will check that size if empty is actually allowed
	const u64 block_size = sstream.Read<u8>();
	const bool is_empty_value = ( block_size != expected_block_size ) ? true : false;
	if( is_empty_value )
	{
		if( empty_value_is_allowed )
		{
			// if empty is allowed, make sure that we have the block size of an empty block
			if( block_size != expected_block_size_if_empty )
			{
				ctLogError << "The size of the block in the input stream:" << block_size << " does not match expected possible sizes: " << expected_block_size_if_empty << " (if empty value) or " << expected_block_size << " (if non-empty) " << ctLogEnd;
				return reader_status::fail;
			}
		}
		else
		{
			// empty is not allowed, so regardless of the size, it is invalid, error out
			ctLogError << "The size of the block in the input stream:" << block_size << " does not match expected size (empty is not allowed): " << expected_block_size << ctLogEnd;
			return reader_status::fail;
		}
	}

	// read in the value(s)
	if( !is_empty_value )
	{
		const u64 read_count = sstream.Read( value_ptr( *dest_data ), value_count );
		if( read_count != value_count )
		{
			ctLogError << "Could not read all expected values from the input stream. Expected count: " << value_count << " read count: " << read_count << ctLogEnd;
			return reader_status::fail;
		}
	}

	// read in the key data
	char read_key[EntityMaxKeyLength];
	const u64 read_key_length = sstream.Read( (u8 *)read_key, (u64)key_size_in_bytes );
	if( read_key_length != (u64)key_size_in_bytes
		|| memcmp( key, read_key, (u64)key_size_in_bytes ) != 0 )
	{
		std::string expected_key_name( key, key_size_in_bytes );
		std::string read_key_name( read_key, key_size_in_bytes ); // cap string at lenght of expected data
		ctLogError << "Unexpected key name in the stream. Expected name: " << expected_key_name << " read name: " << read_key_name << ctLogEnd;
		return reader_status::fail;
	}

	// get the position beyond the end of the block, and validate position
	const u64 expected_end_pos = ( is_empty_value ) ? ( start_pos + 2 + expected_block_size_if_empty ) : ( start_pos + 2 + expected_block_size );
	const u64 end_pos = sstream.GetPosition();
	ctSanityCheck( end_pos == expected_end_pos );
	if( end_pos != expected_end_pos )
	{
		ctLogError << "Invaild position in the read stream. Expected position: " << expected_end_pos << " current position: " << end_pos << ctLogEnd;
		return reader_status::fail;
	}

	// return success, either with value or empty
	if( is_empty_value )
		return reader_status::success_empty;
	return reader_status::success;
};

// special implementation of read_small_block for bool values, which reads a u8 and converts to bool
template<> inline reader_status read_single_item<serialization_type_index::vt_bool, bool>( ReadStream &sstream, const char *key, const u8 key_size_in_bytes, const bool empty_value_is_allowed, bool *dest_data )
{
	u8 u8val;
	reader_status status = read_single_item<serialization_type_index::vt_bool, u8>( sstream, key, key_size_in_bytes, empty_value_is_allowed, &u8val );
	if( status != reader_status::fail )
	{
		( *dest_data ) = (bool)u8val;
	}
	return status;
};

// template method that Reads a small block of a specific serialization_type_index VT to the stream. Since most value types 
// can have different bit depths, the second parameter I is the actual type of the data stored. The data can have more than one values of type I, the count is stored in IC.
template<> inline reader_status read_single_item<serialization_type_index::vt_string, string>( ReadStream &sstream, const char *key, const u8 key_size_in_bytes, const bool empty_value_is_allowed, string *dest_data )
{
	static_assert( sizeof( u64 ) == sizeof( size_t ), "Unsupported size_t, current code requires it to be 8 bytes in size, equal to u64" );

	ctSanityCheck( dest_data );

	// read block header
	const u64 expected_end_position = begin_read_large_block( sstream, serialization_type_index::vt_string, key, key_size_in_bytes );
	if( expected_end_position == 0 )
	{
		ctLogError << "begin_read_large_block() failed unexpectedly" << ctLogEnd;
		return reader_status::fail;
	}

	// if we are at the end of the block, we have an empty string
	if( sstream.GetPosition() == expected_end_position )
	{
		// marked as empty, check that this is allowed
		if( empty_value_is_allowed )
		{
			// empty value is allowed, early out if the block end checks out
			if( !end_read_large_block( sstream, expected_end_position ) )
			{
				ctLogError << "End position of data " << sstream.GetPosition() << " does not equal the expected end position which is " << expected_end_position << ctLogEnd;
				return reader_status::fail;
			}

			// all good
			return reader_status::success_empty;
		}
		else
		{
			// empty is not allowed
			ctLogError << "The read stream value is empty, which is not allowed for value \"" << key << "\"" << ctLogEnd;
			return reader_status::fail;
		}
	}

	// non-empty, read in the string size
	const u64 string_size = sstream.Read<u64>();

	// make sure the item count is plausible before allocating the vector
	const u64 expected_string_size = ( expected_end_position - sstream.GetPosition() );
	if( string_size > expected_string_size )
	{
		ctLogError << "The string size in the stream is invalid, it is beyond the size of the value block" << ctLogEnd;
		return reader_status::fail;
	}

	// resize the destination string
	dest_data->resize( string_size );

	// read in the string data
	if( string_size > 0 )
	{
		i8 *p_data = (i8 *)&( dest_data->front() );
		const u64 read_item_count = sstream.Read( p_data, string_size );
		if( read_item_count != string_size )
		{
			ctLogError << "The stream could not read the whole string" << ctLogEnd;
			return reader_status::fail;
		}
	}

	// make sure we are at the expected end pos
	if( !end_read_large_block( sstream, expected_end_position ) )
	{
		ctLogError << "End position of data " << sstream.GetPosition() << " does not equal the expected end position which is " << expected_end_position << ctLogEnd;
		return reader_status::fail;
	}

	return reader_status::success;
}

inline reader_status end_read_empty_large_block( ReadStream &sstream, const char *key, const bool empty_value_is_allowed, const u64 expected_end_position )
{
	// check that empty value this is allowed
	if( empty_value_is_allowed )
	{
		// empty value is allowed, early out if the block end checks out
		if( !end_read_large_block( sstream, expected_end_position ) )
		{
			ctLogError << "End position of data " << sstream.GetPosition() << " does not equal the expected end position which is " << expected_end_position << ctLogEnd;
			return reader_status::fail;
		}

		// all good
		return reader_status::success_empty;
	}
	else
	{
		// empty is not allowed
		ctLogError << "The read stream value is empty, which is not allowed for value \"" << key << "\"" << ctLogEnd;
		return reader_status::fail;
	}
}

// reads an array header and value size from the stream, and decodes into flags, then reads the index if one exists. 
inline bool read_array_metadata_and_index( ReadStream &sstream, size_t &out_per_item_size, size_t &out_item_count, const u64 block_end_position, vector<u32> *dest_index )
{
	static_assert( sizeof( u64 ) <= sizeof( size_t ), "Unsupported size_t, current code requires it to be at least 8 bytes in size, equal to u64" );

	const u64 start_position = sstream.GetPosition();
	u64 expected_end_position = start_position + sizeof( u16 ) + sizeof( u64 );

	const u16 array_flags = sstream.Read<u16>();
	out_per_item_size = (size_t)( array_flags & 0xff );
	const bool has_index = ( array_flags & 0x100 ) != 0;
	const bool index_is_64bit = ( array_flags & 0x200 ) != 0;

	// we don't support 64 bit index (yet)
	if( index_is_64bit )
	{
		ctLogError << "The block has a 64 bit index, which is not supported" << ctLogEnd;
		return false;
	}

	// read in the item count
	out_item_count = (size_t)sstream.Read<u64>();

	// if we have an index, read it
	if( has_index )
	{
		// make sure we DO expect an index
		if( !dest_index )
		{
			ctLogError << "Invalid array type: The stream type has an index, but the destination object does not." << ctLogEnd;
			return false;
		}

		// read in the size of the index
		ctSanityCheck( block_end_position >= sstream.GetPosition() );
		const u64 index_count = sstream.Read<u64>();
		const u64 maximum_possible_index_count = ( block_end_position - sstream.GetPosition() ) / sizeof( u32 );
		if( index_count > maximum_possible_index_count )
		{
			ctLogError << "The index item count in the stream is invalid, it is beyond the size of the block" << ctLogEnd;
			return false;
		}

		// resize the dest vector
		dest_index->resize( index_count );

		// read in the data
		u32 *p_index_data = dest_index->data();
		sstream.Read( p_index_data, index_count );

		// modify the expected end position
		expected_end_position += sizeof( u64 ) + ( index_count * sizeof( i32 ) );
	}
	else
	{
		// make sure we do NOT expect an index
		if( dest_index )
		{
			ctLogError << "Invalid array type: The stream type has does not have an index, but the destination object does." << ctLogEnd;
			return false;
		}
	}

	if( expected_end_position != sstream.GetPosition() )
	{
		ctLogError << "Failed to read full array header from block." << ctLogEnd;
		return false;
	}

	return true;
}

template<serialization_type_index VT, class T> inline reader_status read_array( ReadStream &sstream, const char *key, const u8 key_size_in_bytes, const bool empty_value_is_allowed, vector<T> *dest_items, vector<u32> *dest_index )
{
	static_assert( ( VT >= serialization_type_index::vt_array_bool ) && ( VT <= serialization_type_index::vt_array_hash ), "Invalid type for generic read_array template" );
	static_assert( sizeof( u64 ) >= sizeof( size_t ), "Unsupported size_t, current code requires it to be at max 8 bytes in size, equal to u64" );
	const size_t value_size = sizeof( typename element_type_information<T>::value_type );

	ctSanityCheck( dest_items );

	// read block header. if we are already at the end, the block is empty, end the block and make sure empty is allowed
	const u64 block_end_position = begin_read_large_block( sstream, VT, key, key_size_in_bytes );
	if( block_end_position == 0 )
	{
		ctLogError << "begin_read_large_block() failed unexpectedly" << ctLogEnd;
		return reader_status::fail;
	}
	else if( block_end_position == sstream.GetPosition() )
	{
		return end_read_empty_large_block( sstream, key, empty_value_is_allowed, block_end_position );
	}

	// read item size & count and index if it exists, or make sure we do not expect an index
	size_t per_item_size = 0;
	size_t item_count = 0;
	if( !read_array_metadata_and_index( sstream, per_item_size, item_count, block_end_position, dest_index ) )
	{
		return reader_status::fail;
	}

	// make sure we have the right item size
	if( value_size != per_item_size )
	{
		ctLogError << "The size of the items in the stream does not match the expected size" << ctLogEnd;
		return reader_status::fail;
	}

	// make sure the item count is plausible before allocating the vector
	const u64 maximum_possible_item_count = ( block_end_position - sstream.GetPosition() ) / value_size;
	if( item_count > maximum_possible_item_count )
	{
		ctLogError << "The array item count in the stream is invalid, it is beyond the size of the block" << ctLogEnd;
		return reader_status::fail;
	}

	// resize the destination vector
	const u64 type_count = item_count / element_type_information<T>::value_count;
	dest_items->resize( type_count );

	// read in the data
	T *p_data = dest_items->data();
	const u64 read_item_count = sstream.Read( value_ptr( *p_data ), item_count );
	if( read_item_count != item_count )
	{
		ctLogError << "The stream could not read all the items for the array" << ctLogEnd;
		return reader_status::fail;
	}

	// make sure we are at the expected end pos
	if( !end_read_large_block( sstream, block_end_position ) )
	{
		ctLogError << "End position of data " << sstream.GetPosition() << " does not equal the expected end position which is " << block_end_position << ctLogEnd;
		return reader_status::fail;
	}

	return reader_status::success;
}

// read_array implementation for bool arrays (which need specific packing)
template <> inline reader_status read_array<serialization_type_index::vt_array_bool, bool>( ReadStream &sstream, const char *key, const u8 key_size_in_bytes, const bool empty_value_is_allowed, vector<bool> *dest_items, vector<u32> *dest_index )
{
	ctSanityCheck( dest_items );

	// read block header. if we are already at the end, the block is empty, end the block and make sure empty is allowed
	const u64 block_end_position = begin_read_large_block( sstream, serialization_type_index::vt_array_bool, key, key_size_in_bytes );
	if( block_end_position == 0 )
	{
		ctLogError << "begin_read_large_block() failed unexpectedly" << ctLogEnd;
		return reader_status::fail;
	}
	else if( block_end_position == sstream.GetPosition() )
	{
		return end_read_empty_large_block( sstream, key, empty_value_is_allowed, block_end_position );
	}

	// read item size & count and index if it exists, or make sure we do not expect an index
	size_t per_item_size = 0;
	size_t bool_count = 0;
	if( !read_array_metadata_and_index( sstream, per_item_size, bool_count, block_end_position, dest_index ) )
	{
		return reader_status::fail;
	}

	// calculate the number of packed items.
	// round up, should the last u8 be not fully filled
	const u64 number_of_packed_u8s = ( bool_count + 7 ) / 8;

	// make sure the item count is plausible before allocating the vector
	const u64 maximum_possible_item_count = ( block_end_position - sstream.GetPosition() );
	if( number_of_packed_u8s > maximum_possible_item_count )
	{
		ctLogError << "The array item count in the stream is invalid, it is beyond the size of the block" << ctLogEnd;
		return reader_status::fail;
	}

	// resize the destination vector
	dest_items->resize( bool_count );

	// read in a u8 vector with the packed values
	std::vector<u8> packed_vec( number_of_packed_u8s );
	sstream.Read( packed_vec.data(), number_of_packed_u8s );

	// unpack the bool vector from the u8 vector
	for( u64 bool_index = 0; bool_index < bool_count; ++bool_index )
	{
		const size_t packed_index = bool_index >> 3; // bool_index / 8
		const size_t packed_subindex = bool_index & 0x7; // bool_index % 8
		( *dest_items )[bool_index] = ( ( packed_vec[packed_index] ) & ( 1 << packed_subindex ) ) != 0;
	}

	// make sure we are at the expected end pos
	if( !end_read_large_block( sstream, block_end_position ) )
	{
		ctLogError << "End position of data " << sstream.GetPosition() << " does not equal the expected end position which is " << block_end_position << ctLogEnd;
		return reader_status::fail;
	}

	return reader_status::success;
}

template<> inline reader_status read_array<serialization_type_index::vt_array_string, string>( ReadStream &sstream, const char *key, const u8 key_size_in_bytes, const bool empty_value_is_allowed, vector<string> *dest_items, vector<u32> *dest_index )
{
	static_assert( sizeof( u64 ) == sizeof( size_t ), "Unsupported size_t, current code requires it to be 8 bytes in size, equal to u64" );

	ctSanityCheck( dest_items );

	// read block header. if we are already at the end, the block is empty, end the block and make sure empty is allowed
	const u64 block_end_position = begin_read_large_block( sstream, serialization_type_index::vt_array_string, key, key_size_in_bytes );
	if( block_end_position == 0 )
	{
		ctLogError << "begin_read_large_block() failed unexpectedly" << ctLogEnd;
		return reader_status::fail;
	}
	else if( block_end_position == sstream.GetPosition() )
	{
		return end_read_empty_large_block( sstream, key, empty_value_is_allowed, block_end_position );
	}

	// read item size & count and index if it exists, or make sure we do not expect an index
	size_t per_item_size = 0;
	size_t string_count = 0;
	if( !read_array_metadata_and_index( sstream, per_item_size, string_count, block_end_position, dest_index ) )
	{
		return reader_status::fail;
	}

	// make sure the item count is plausible before allocating the vector
	// (the size is assuming only empty strings, so only the size of the string size (sizeof(u64)) per string)
	const u64 maximum_possible_item_count = ( block_end_position - sstream.GetPosition() ) / sizeof( u64 );
	if( string_count > maximum_possible_item_count )
	{
		ctLogError << "The array string count in the stream is invalid, it is beyond the size of the block" << ctLogEnd;
		return reader_status::fail;
	}

	// resize the destination vector
	dest_items->resize( string_count );

	// read in each string separately
	for( u64 string_index = 0; string_index < string_count; ++string_index )
	{
		string &dest_string = ( *dest_items )[string_index];

		const u64 string_size = sstream.Read<u64>();

		// make sure the string is not outsize of possible size
		const u64 maximum_possible_string_size = ( block_end_position - sstream.GetPosition() );
		if( string_size > maximum_possible_string_size )
		{
			ctLogError << "A string size in a string array in the stream is invalid, it is beyond the size of the block" << ctLogEnd;
			return reader_status::fail;
		}

		// setup the destination string, and read in the data
		dest_string.resize( string_size );
		if( string_size > 0 )
		{
			i8 *p_data = (i8 *)&( dest_string.front() );
			const u64 read_item_count = sstream.Read( p_data, string_size );
			if( read_item_count != string_size )
			{
				ctLogError << "The stream could not read one of the strings" << ctLogEnd;
				return reader_status::fail;
			}
		}
	}

	// make sure we are at the expected end pos
	if( !end_read_large_block( sstream, block_end_position ) )
	{
		ctLogError << "End position of data " << sstream.GetPosition() << " does not equal the expected end position which is " << block_end_position << ctLogEnd;
		return reader_status::fail;
	}

	return reader_status::success;
}

#include "_pds_undef_macros.inl"
}
// namespace pds

#endif//__PDS__READER_TEMPLATES_H__
