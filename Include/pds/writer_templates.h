// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once
#ifndef __PDS__WRITER_TEMPLATES_H__
#define __PDS__WRITER_TEMPLATES_H__

#include <ctle/log.h>

#include "fileops_common.h"
#include "element_value_ptrs.h"
#include "WriteStream.h"

namespace pds
{
#include "_pds_macros.inl"

// begin/end writing a large block of serialization_type_index VT to the stream.
status begin_write_large_block( WriteStream &dstream, serialization_type_index VT, const char *key, const u8 key_size_in_bytes );
status end_write_large_block( WriteStream &dstream, u64 start_pos );

// Write a single block of serialization_type_index VT to the stream. Most values are fixed, small sizes, but string values can be any size 
template<serialization_type_index VT, class T> status write_single_value( WriteStream &dstream, const char *key, const u8 key_size_in_bytes, const T *data );

// Write an array to stream.
template<serialization_type_index VT, class T> status write_array( WriteStream &dstream, const char *key, const u8 key_size_in_bytes, const std::vector<T> *items, const std::vector<i32> *index );

// called to begin a large block
inline status begin_write_large_block( WriteStream &dstream, const serialization_type_index VT, const char *key, const u8 key_size_in_bytes )
{
	const u8 value_type = (u8)VT;
	const u64 start_pos = dstream.GetPosition();
	ctSanityCheck( key_size_in_bytes <= EntityMaxKeyLength );

	// sizeof(value_type)=1 + sizeof(block_size)=8 + sizeof(key_size_in_bytes)=1 + key_size_in_bytes;
	const u64 expected_end_pos = start_pos + key_size_in_bytes + 10;

	// write block header 
	// write empty stand in value for now (INT64_MAX on purpose), which is definitely 
	// wrong, as to trigger any test if the value is not overwritten with the correct value
	dstream.Write( value_type );
	dstream.Write( (u64)INT64_MAX );
	dstream.Write( key_size_in_bytes );
	dstream.Write( (i8 *)key, key_size_in_bytes );

	const u64 end_pos = dstream.GetPosition();

	// validate the write 
	ctValidate(end_pos == expected_end_pos, status::cant_write)
		<< "Failed to write the beginning of a large block. End position of data " << end_pos
		<< " does not equal the expected end position which is " << expected_end_pos
		<< "." << ctValidateEnd;

	// succeeded
	return status::ok;
}

// ends the block
// writes the size of the block in the header of the block
inline status end_write_large_block( WriteStream &dstream, const u64 start_pos )
{
	const u64 end_pos = dstream.GetPosition();
	ctValidate( end_pos > start_pos, status::cant_write )
		<< "Failed to write the end of a large block. End position of data " << end_pos
		<< " is not greater than the start position which is " << start_pos
		<< "." << ctValidateEnd;

	const u64 block_size = end_pos - start_pos - 9; // total block size - ( sizeof( serialization_type_index )=1 + sizeof( block_size_variable )=8 )
	dstream.SetPosition( start_pos + 1 ); // skip over the serialization_type_index
	dstream.Write( block_size );
	dstream.SetPosition( end_pos ); // move back the where we were
	return ( end_pos > start_pos ) ? (status::ok) : (status::cant_write); // only thing we really can check
}

// template method that writes a small block of a specific serialization_type_index VT to the stream. 
template<serialization_type_index VT, class T> inline status write_single_value( WriteStream &dstream, const char *key, const u8 key_length, const T *data )
{
	static_assert( ( VT >= serialization_type_index::vt_bool ) && ( VT <= serialization_type_index::vt_hash ), "Invalid type for general write_single_value template" );

	const bool has_data = data != nullptr;
	
	const u8 value_type = (u8)VT;
	const size_t value_size = sizeof( typename element_type_information<T>::value_type );
	const size_t value_count = element_type_information<T>::value_count;
	const size_t data_size_in_bytes = ( has_data ) ? ( value_size * value_count ) : 0; // if data == nullptr, the block is empty
	const size_t block_size = data_size_in_bytes + key_length;
	ctSanityCheck( key_length <= EntityMaxKeyLength ); // max key length
	ctSanityCheck( block_size < 256 ); // must fit in a byte
	const u8 u8_block_size = (u8)( block_size );
	const u64 start_pos = dstream.GetPosition();
	const u64 expected_end_pos = start_pos + 2 + block_size;

	// write data block 
	dstream.Write( value_type );
	dstream.Write( u8_block_size );
	if( data_size_in_bytes > 0 )
	{
		const typename element_type_information<T>::value_type *pvalue = value_ptr( ( *data ) );
		dstream.Write( pvalue, value_count );
	}
	dstream.Write( (i8 *)key, key_length );

	const u64 end_pos = dstream.GetPosition();

	// validate the write 
	ctValidate(end_pos == expected_end_pos, status::cant_write)
		<< "Failed to write value to stream. End position of data " << end_pos
		<< " does not equal the expected end position which is " << expected_end_pos
		<< "." << ctValidateEnd;

	// succeeded
	return status::ok;
};

// specialization of write_single_value for bool values, which are converted to a u8 value for writing
template<> inline status write_single_value<serialization_type_index::vt_bool, bool>( WriteStream &dstream, const char *key, const u8 key_length, const bool *data )
{
	// if data is set, convert to an u8 value, and point at it
	const u8 u8val = ( data ) ? ( (u8)( *data ) ) : 0;
	const u8 *p_data = ( data ) ? ( &u8val ) : nullptr;
	return write_single_value<serialization_type_index::vt_bool, u8>( dstream, key, key_length, p_data );
}

// specialization of write_single_value for strings, which are encoded differently from other single block values
template<> inline status write_single_value<serialization_type_index::vt_string, std::string>( WriteStream &dstream, const char *key, const u8 key_length, const std::string *string_value )
{
	// record start position, we need this in the end block
	const u64 start_pos = dstream.GetPosition();

	// begin a large block
	ctStatusCall(begin_write_large_block(dstream, serialization_type_index::vt_string, key, key_length));

	// record start of the array data, for error check
	const u64 string_data_start_pos = dstream.GetPosition();

	// if empty value (not same as empty string, which just has size 0 but is still set)
	if( !string_value )
	{
		// empty string, early out 
		ctStatusCall(end_write_large_block(dstream, start_pos));
		return status::ok;
	}

	// write the size of the string, and the actual string values
	const u64 character_count = u64( string_value->size() );
	const u64 values_size = character_count + sizeof( character_count );
	dstream.Write( character_count );
	if( character_count > 0 )
	{
		const i8 *data = (const i8 *)string_value->data();
		dstream.Write( data, character_count );
	}

	// make sure all data was written
	const u64 expected_end_pos = string_data_start_pos + values_size;

	// end the block by going back to the start and writing the start position offset
	ctStatusCall(end_write_large_block(dstream, start_pos));

	// validate the write 
	const u64 end_pos = dstream.GetPosition();
	ctValidate(end_pos == expected_end_pos, status::cant_write)
		<< "Failed to write value to stream. End position of data " << end_pos
		<< " does not equal the expected end position which is " << expected_end_pos
		<< "." << ctValidateEnd;

	// succeeded
	return status::ok;
}

// write metadata and index for an array
inline status write_array_metadata_and_index( WriteStream &dstream, size_t per_item_size, size_t item_count, const std::vector<i32> *index )
{
	static_assert( sizeof( u64 ) <= sizeof( size_t ), "Unsupported size_t, current code requires it to be at least 8 bytes in size, equal to u64" );
	ctSanityCheck( per_item_size <= 0xff ); // max 8 bits for per item size
	const u64 start_pos = dstream.GetPosition();
	
	// set flags for the array
	const u16 has_index_flag = ( index ) ? ( 0x100 ) : ( 0 );
	const u16 index_is_64bit_flag = ( false ) ? ( 0x200 ) : ( 0 ); // we do not support 64 bit indices yet
	const u16 array_flags = has_index_flag | index_is_64bit_flag | u16( per_item_size );
	dstream.Write( array_flags );

	// write the number of items
	dstream.Write( u64( item_count ) );

	// if we have an index, write it 
	u64 index_size = 0;
	if( index )
	{
		const u64 index_count = index->size();
		dstream.Write( index_count );
		dstream.Write( index->data(), index_count );

		index_size = ( index_count * sizeof( i32 ) ) + sizeof( u64 ); // the index values and the value count
	}

	// make sure all data was written
	const u64 expected_end_pos =
		start_pos
		+ sizeof( u16 ) // the flags
		+ sizeof( u64 ) // the item count
		+ index_size;   // the (optional) index

	const u64 end_pos = dstream.GetPosition();
	ctValidate(end_pos == expected_end_pos, status::cant_write) 
		<< "End position of data " << end_pos 
		<< " does not equal the expected end position which is " << expected_end_pos 
		<< "." << ctValidateEnd;
	
	return status::ok;
}

// write array to stream
template<serialization_type_index VT, class T> inline status write_array( WriteStream &dstream, const char *key, const u8 key_size_in_bytes, const std::vector<T> *items, const std::vector<i32> *index )
{
	static_assert( ( VT >= serialization_type_index::vt_array_bool ) && ( VT <= serialization_type_index::vt_array_hash ), "Invalid type for write_array" );
	static_assert( sizeof( typename element_type_information<T>::value_type ) <= 0xff, "Invalid value size, cannot exceed 255 bytes" );
	static_assert( sizeof( u64 ) >= sizeof( size_t ), "Unsupported size_t, current code requires it to be at most 8 bytes in size, equal to an u64" ); // assuming sizeof(u64) >= sizeof(size_t)
	const size_t value_size = sizeof( typename element_type_information<T>::value_type );
	const size_t values_per_type = element_type_information<T>::value_count;

	// record start position, we need this in the end block
	const u64 start_pos = dstream.GetPosition();

	// begin a large block
	ctStatusCall( begin_write_large_block( dstream, VT, key, key_size_in_bytes ) );

	// write data if we have it
	if( items )
	{
		const u64 values_count = items->size() * values_per_type;
		ctStatusCall( write_array_metadata_and_index( dstream, value_size, values_count, index ) );

		// write the values
		if( values_count > 0 )
		{
			const typename element_type_information<T>::value_type *p_values = value_ptr( *( items->data() ) );

			const u64 values_expected_end_pos = dstream.GetPosition() + ( values_count * value_size );
			dstream.Write( p_values, values_count );
			const u64 values_end_pos = dstream.GetPosition();

			// make sure all were written
			ctValidate(values_end_pos == values_expected_end_pos, status::cant_write) 
				<< "End position of data " << values_end_pos 
				<< " does not equal the expected end position which is " << values_expected_end_pos
				<< "." << ctValidateEnd;
		}
	}

	// end the block by going back to the start and writing the size of the payload
	ctStatusCall( end_write_large_block( dstream, start_pos ) );

	// succeeded
	return status::ok;
}

// specialization of write_array for bool arrays
template<> inline status write_array<serialization_type_index::vt_array_bool, bool>( WriteStream &dstream, const char *key, const u8 key_size_in_bytes, const std::vector<bool> *items, const std::vector<i32> *index )
{
	// record start position, we need this in the end block
	const u64 start_pos = dstream.GetPosition();

	// begin a large block
	ctStatusCall( begin_write_large_block( dstream, serialization_type_index::vt_array_bool, key, key_size_in_bytes ) );

	// write data if we have it
	if( items )
	{
		// write the item count and items
		ctStatusCall( write_array_metadata_and_index( dstream, 0, items->size(), index ) );

		if( items->size() > 0 )
		{
			const u64 number_of_packed_u8s = ( items->size() + 7 ) / 8;

			// pack the bool vector to a temporary u8 vector
			// round up, should the last u8 be not fully filled
			std::vector<u8> packed_vec( number_of_packed_u8s );
			for( size_t bool_index = 0; bool_index < items->size(); ++bool_index )
			{
				if( ( *items )[bool_index] )
				{
					const size_t packed_index = bool_index >> 3; // bool_index / 8
					const size_t packed_subindex = bool_index & 0x7; // bool_index % 8
					packed_vec[packed_index] |= 1 << packed_subindex;
				}
			}

			// write u8 vector to stream
			const u64 values_expected_end_pos = dstream.GetPosition() + number_of_packed_u8s;
			dstream.Write( packed_vec.data(), number_of_packed_u8s );
			const u64 values_end_pos = dstream.GetPosition();

			// make sure all were written
			ctValidate(values_end_pos == values_expected_end_pos, status::cant_write) 
				<< "End position of data " << values_end_pos 
				<< " does not equal the expected end position which is " << values_expected_end_pos
				<< "." << ctValidateEnd;
		}
	}

	// end the block by going back to the start and writing the start position offset
	ctStatusCall( end_write_large_block( dstream, start_pos ) );

	// succeeded
	return status::ok;
}

// specialization of write_array for string arrays
template<> inline status write_array<serialization_type_index::vt_array_string, std::string>( WriteStream &dstream, const char *key, const u8 key_size_in_bytes, const std::vector<std::string> *items, const std::vector<i32> *index )
{
	// record start position, we need this in the end block
	const u64 start_pos = dstream.GetPosition();

	// begin a large block
	ctStatusCall( begin_write_large_block( dstream, serialization_type_index::vt_array_string, key, key_size_in_bytes ) );

	// write data if we have it
	if( items )
	{
		// write the item count and items
		ctStatusCall( write_array_metadata_and_index( dstream, 0, items->size(), index ) );

		if( items->size() > 0 )
		{
			const u64 values_start_pos = dstream.GetPosition();

			// this is the minimum size, with only empty strings. each string will add to the values_size
			u64 values_size = sizeof( u64 ) * items->size();

			// write each string in the array
			for( size_t string_index = 0; string_index < items->size(); ++string_index )
			{
				u64 string_length = ( *items )[string_index].size();
				dstream.Write( string_length );
				if( string_length > 0 )
				{
					i8 *p_data = (i8 *)( ( *items )[string_index].data() );
					dstream.Write( p_data, string_length );

					// update size of values
					values_size += string_length;
				}
			}

			const u64 values_expected_end_pos = values_start_pos + values_size;
			const u64 values_end_pos = dstream.GetPosition();

			// make sure all were written
			ctValidate(values_end_pos == values_expected_end_pos, status::cant_write) 
				<< "End position of data " << values_end_pos 
				<< " does not equal the expected end position which is " << values_expected_end_pos
				<< "." << ctValidateEnd;

		}
	}

	// end the block by going back to the start and writing the start position offset
	ctStatusCall( end_write_large_block( dstream, start_pos ) );

	// succeeded
	return status::ok;
}

#include "_pds_undef_macros.inl"
}
// namespace pds

#endif//__PDS__WRITER_TEMPLATES_H__