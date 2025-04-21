// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once

#include "fwd.h"
#include <ctle/uuid.h>
#include <ctle/digest.h>

namespace pds
{

// Memory write stream is a write-only memory area.
// The stream is used to write structured values. 
// It can write out more complex types than just plain old data (POD) 
// types, and also supports std::string, UUIDs and std::vectors of 
// the above types.
// Caveat: The stream is NOT thread safe, and should be accessed by 
// only one thread at a time.
class WriteStream
{
private:
	static const u64 InitialAllocationSize = 1024 * 1024 * 64; // 64MB initial size

	u8 *Data = nullptr; // the allocated data
	u64 DataSize = 0; // the size of the memory stream (not the reserved allocation)
	u64 Position = 0; // the write position in the memory stream

	u64 DataReservedSize = 0; // the reserved size of the allocation
	u32 PageSize = 0; // size of each page of allocation

	// reserve data for at least reserveSize.
	void ReserveForSize( u64 reserveSize );
	void FreeAllocation();

	// resize (grow) the data stream. if the new size is larger than the reserved size, the allocation will be resized to fit the new size
	void Resize( u64 newSize );

	// write raw bytes to the memory stream. this will increase reserved allocation of data as needed
	void WriteRawData( const void *src, u64 count );

	// write 1,2,4 or 8 byte values and make sure they are in the correct byte order
	template <class T> void WriteValues( const T *src, u64 count );

public:
	WriteStream( u64 _InitialAllocationSize = InitialAllocationSize );
	~WriteStream();

	// get a read-only pointer to the data
	const void *GetData() const { return this->Data; }

	// get the Size of the stream in bytes
	u64 GetSize() const;

	// Position is the current data position. the beginning of the stream is position 0. the stream grows whenever the position moves past the current end of the stream.
	u64 GetPosition() const;
	void SetPosition( u64 new_pos );

	// write one item to the memory stream. makes sure to convert endianness
	void Write( const i8 &src );
	void Write( const i16 &src );
	void Write( const i32 &src );
	void Write( const i64 &src );
	void Write( const u8 &src );
	void Write( const u16 &src );
	void Write( const u32 &src );
	void Write( const u64 &src );
	void Write( const float &src );
	void Write( const double &src );
	void Write( const uuid &src );
	void Write( const hash &src );

	// write an array of items to the memory stream. makes sure to convert endianness
	void Write( const i8 *src, u64 count );
	void Write( const i16 *src, u64 count );
	void Write( const i32 *src, u64 count );
	void Write( const i64 *src, u64 count );
	void Write( const u8 *src, u64 count );
	void Write( const u16 *src, u64 count );
	void Write( const u32 *src, u64 count );
	void Write( const u64 *src, u64 count );
	void Write( const float *src, u64 count );
	void Write( const double *src, u64 count );
	void Write( const uuid *src, u64 count );
	void Write( const hash *src, u64 count );
};

inline void WriteStream::WriteRawData( const void *src, u64 count )
{
	// cap the end position
	u64 end_pos = this->Position + count;
	if( end_pos > this->DataSize )
	{
		this->Resize( end_pos );
	}

	// copy the data and move the position
	memcpy( &this->Data[this->Position], src, count );
	this->Position = end_pos;
}

template <class T> inline void WriteStream::WriteValues( const T *src, u64 count )
{
	// this intermediate step is kept to allow for adding support for byte flipping (big-endian)
	this->WriteRawData( src, count * sizeof( T ) );
}

inline u64 WriteStream::GetSize() const
{
	return this->DataSize;
}

inline u64 WriteStream::GetPosition() const
{
	return this->Position;
}

inline void WriteStream::SetPosition( u64 new_pos )
{
	if( new_pos > DataSize )
	{
		this->Resize( new_pos );
	}
	this->Position = new_pos;
}

//// write one item of data, (but using the multi-values method)
inline void WriteStream::Write( const i8 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const i16 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const i32 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const i64 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const u8 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const u16 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const u32 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const u64 &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const float &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const double &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const uuid &src ) { this->Write( &src, 1 ); }
inline void WriteStream::Write( const hash &src ) { this->Write( &src, 1 ); }

// 8 bit data
inline void WriteStream::Write( const i8 *src, u64 count ) { return this->WriteValues<u8>( (const u8*)src, count ); }
inline void WriteStream::Write( const u8 *src, u64 count ) { return this->WriteValues<u8>( src, count ); }
								   
// 16 bit data				   
inline void WriteStream::Write( const i16 *src, u64 count ) { return this->WriteValues<u16>( (const u16*)src, count ); }
inline void WriteStream::Write( const u16 *src, u64 count ) { return this->WriteValues<u16>( src, count ); }
								   
// 32 bit data				   
inline void WriteStream::Write( const i32 *src, u64 count ) { return this->WriteValues<u32>( (const u32*)src, count ); }
inline void WriteStream::Write( const u32 *src, u64 count ) { return this->WriteValues<u32>( src, count ); }
inline void WriteStream::Write( const float *src, u64 count ) { return this->WriteValues<u32>( (const u32*)src, count ); }
								   
// 64 bit data				   
inline void WriteStream::Write( const i64 *src, u64 count ) { return this->WriteValues<u64>( (const u64*)src, count ); }
inline void WriteStream::Write( const u64 *src, u64 count ) { return this->WriteValues<u64>( src, count ); }
inline void WriteStream::Write( const double *src, u64 count ) { return this->WriteValues<u64>( (const u64*)src, count ); }

// uuids
inline void WriteStream::Write( const uuid *src, u64 count )
{
	static_assert( sizeof( uuid ) == 16, "Invalid size of uuid struct, needs to be exactly 16 bytes." );

	// Write raw bytes, assumes the values are contiguous 
	// No need for byte-swapping, the uuids are always stored as raw bytes, and ordered 
	// big-endian (the order which the hex values are printed when printing a guid)
	this->WriteValues<u8>( (const u8 *)src, sizeof( uuid ) * count );
}

// hashes
inline void WriteStream::Write( const hash *src, u64 count )
{
	static_assert( sizeof( hash ) == 32, "Invalid size of hash struct, needs to be exactly 32 bytes." );

	// Write raw bytes, assumes the values are contiguous 
	// No need for byte-swapping, the hashes are always stored as raw bytes, and ordered 
	// big-endian (the order which the hex values are printed when printing a hash)
	this->WriteValues<u8>( (const u8 *)src, sizeof( hash ) * count );
}

}
// namespace pds

#ifdef PDS_IMPLEMENTATION
#include "WriteStream.inl"
#endif//PDS_IMPLEMENTATION

