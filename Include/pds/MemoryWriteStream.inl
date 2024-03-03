// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "MemoryWriteStream.h"

namespace pds
{

MemoryWriteStream::MemoryWriteStream( u64 _InitialAllocationSize ) 
{ 
	this->ReserveForSize( _InitialAllocationSize ); 
}

MemoryWriteStream::~MemoryWriteStream() 
{ 
	this->FreeAllocation(); 
}

void MemoryWriteStream::ReserveForSize( u64 reserveSize )
{
	// we need to resize the reserved data area, try doubling size
	// and if that is not enough, set to the reserveSize
	this->DataReservedSize *= 2;
	if( this->DataReservedSize < reserveSize )
	{
		this->DataReservedSize = reserveSize;
	}

	// allocate a new area
#ifdef _MSC_VER
	u8 *pNewData = ( u8 * )::VirtualAlloc( nullptr, this->DataReservedSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
#elif defined(__GNUC__)
	u8 *pNewData = (u8 *)malloc( this->DataReservedSize );
#endif
	if( pNewData == nullptr )
	{
		throw std::bad_alloc();
	}

	// if there is an old area, copy the data and free the old area
	// the area can never shrink, so we don't need to worry about capping
	if( this->Data )
	{
		memcpy( pNewData, this->Data, this->DataSize );
		this->FreeAllocation();
	}

	// set the new pointer
	this->Data = pNewData;
}

void MemoryWriteStream::FreeAllocation()
{
	if( this->Data )
	{
#ifdef _MSC_VER			
		::VirtualFree( this->Data, 0, MEM_RELEASE );
#elif defined(__GNUC__)
		free( this->Data );
#endif							
		this->Data = nullptr;
	}
}

void MemoryWriteStream::Resize( u64 newSize )
{
	if( newSize > this->DataReservedSize )
	{
		this->ReserveForSize( newSize );
	}

	this->DataSize = newSize;
}

}
// namespace pds