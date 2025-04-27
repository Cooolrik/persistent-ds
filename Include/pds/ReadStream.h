// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__READSTREAM_H__
#define __PDS__READSTREAM_H__

#include "pds.h"

#include <vector>

namespace pds
{

// Memory read stream is a wrapper around a read-only memory area.
// The stream is used to read structured values, and automatically 
// does byte order swapping. It can read in more complex types than
// just plain old data (POD) types, such as std::string, UUIDs and
// std::vectors of the above types.
// Caveat: The stream is NOT thread safe, and should be accessed by 
// only one thread at a time.

class ReadStream
{
private:
	const u8 *Data = nullptr;
	u64 DataSize = 0;
	u64 DataPosition = 0;

	// read raw bytes from the memory stream
	u64 ReadRawData( void *dest, u64 count );

	// read 1,2,4 or 8 byte values and make sure they are in the correct byte order
	template <class T> u64 ReadValues( T *dest, u64 count );

public:
	ReadStream( const void *_Data, u64 _DataSize ) 
		: Data( (u8 *)_Data )
		, DataSize( _DataSize )
	{};

	// get the Size of the stream in bytes
	u64 GetSize() const;

	// Position is the current data position. the beginning of the stream is position 0. the position will not move past the end of the stream.
	u64 GetPosition() const;
	bool SetPosition( u64 new_pos );

	// if at EOF return true, which means that Position >= Size
	bool IsEOF() const;

	// Peek at the next byte in the stream, without modifing the Position or any data. If the Position is beyond the end of the stream, the value will be 0
	u8 Peek() const;

	// read one item from the memory stream. makes sure to convert endianness
	template <class T> T Read();

	// read a number of items from the memory stream. makes sure to convert endianness
	u64 Read( i8 *dest, u64 count );
	u64 Read( i16 *dest, u64 count );
	u64 Read( i32 *dest, u64 count );
	u64 Read( i64 *dest, u64 count );
	u64 Read( u8 *dest, u64 count );
	u64 Read( u16 *dest, u64 count );
	u64 Read( u32 *dest, u64 count );
	u64 Read( u64 *dest, u64 count );
	u64 Read( float *dest, u64 count );
	u64 Read( double *dest, u64 count );
	u64 Read( uuid *dest, u64 count );
	u64 Read( hash *dest, u64 count );
};

inline u8 ReadStream::Peek() const
{
	if( this->DataPosition >= this->DataSize )
		return 0;
	else
		return this->Data[this->DataPosition];
}

template <class T> inline u64 ReadStream::ReadValues( T *dest, u64 count )
{
	return this->ReadRawData( dest, count * sizeof( T ) ) / sizeof( T );
}

inline u64 ReadStream::GetSize() const
{
	return this->DataSize;
}

inline u64 ReadStream::GetPosition() const
{
	return this->DataPosition;
}

inline bool ReadStream::SetPosition( u64 new_pos )
{
	if( new_pos > DataSize )
	{
		return false;
	}
	this->DataPosition = new_pos;
	return true;
}

inline bool ReadStream::IsEOF() const
{
	return this->DataPosition >= this->DataSize;
}

// read one item of data
template <> inline i8 ReadStream::Read<i8>() { i8 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline i16 ReadStream::Read<i16>() { i16 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline i32 ReadStream::Read<i32>() { i32 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline i64 ReadStream::Read<i64>() { i64 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline u8 ReadStream::Read<u8>() { u8 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline u16 ReadStream::Read<u16>() { u16 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline u32 ReadStream::Read<u32>() { u32 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline u64 ReadStream::Read<u64>() { u64 dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline float ReadStream::Read<float>() { float dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline double ReadStream::Read<double>() { double dest = 0; this->Read( &dest, 1 ); return dest; }
template <> inline uuid ReadStream::Read<uuid>() { uuid dest = {}; this->Read( &dest, 1 ); return dest; }
template <> inline hash ReadStream::Read<hash>() { hash dest = {}; this->Read( &dest, 1 ); return dest; }

// 8 bit data
inline u64 ReadStream::Read( i8 *dest, u64 count ) { return this->ReadValues<u8>( (u8*)dest, count ); }
inline u64 ReadStream::Read( u8 *dest, u64 count ) { return this->ReadValues<u8>( dest, count ); }

// 16 bit data
inline u64 ReadStream::Read( i16 *dest, u64 count ) { return this->ReadValues<u16>( (u16*)dest, count ); }
inline u64 ReadStream::Read( u16 *dest, u64 count ) { return this->ReadValues<u16>( dest, count ); }

// 32 bit data
inline u64 ReadStream::Read( i32 *dest, u64 count ) { return this->ReadValues<u32>( (u32*)dest, count ); }
inline u64 ReadStream::Read( u32 *dest, u64 count ) { return this->ReadValues<u32>( dest, count ); }
inline u64 ReadStream::Read( float *dest, u64 count ) { return this->ReadValues<u32>( (u32*)dest, count ); }

// 64 bit data
inline u64 ReadStream::Read( i64 *dest, u64 count ) { return this->ReadValues<u64>( (u64*)dest, count ); }
inline u64 ReadStream::Read( u64 *dest, u64 count ) { return this->ReadValues<u64>( dest, count ); }
inline u64 ReadStream::Read( double *dest, u64 count ) { return this->ReadValues<u64>( (u64*)dest, count ); }

// uuids
inline u64 ReadStream::Read( uuid *dest, u64 count )
{
	static_assert( sizeof( uuid ) == 16, "Invalid size of uuid struct, needs to be exactly 16 bytes." );

	// Read raw bytes, assumes the values are contiguous 
	// No need for byte-swapping, the uuids are always stored as raw bytes, and ordered 
	// big-endian (the order which the hex values are printed when printing a uuid)
	// return number of successfully read full items
	return this->ReadValues<u8>( (u8 *)dest, sizeof( uuid ) * count ) / sizeof( uuid );
}

// hashes
inline u64 ReadStream::Read( hash *dest, u64 count )
{
	static_assert( sizeof( hash ) == 32, "Invalid size of hash struct, needs to be exactly 32 bytes." );

	// Read raw bytes, assumes the values are contiguous 
	// No need for byte-swapping, the hashes are always stored as raw bytes, and ordered 
	// big-endian (the order which the hex values are printed when printing a hash)
	// return number of successfully read full items
	return this->ReadValues<u8>( (u8 *)dest, sizeof( hash ) * count ) / sizeof( hash );
}

}
// namespace pds

#ifdef PDS_IMPLEMENTATION
#include "ReadStream.inl"
#endif//PDS_IMPLEMENTATION

#endif//__PDS__READSTREAM_H__
