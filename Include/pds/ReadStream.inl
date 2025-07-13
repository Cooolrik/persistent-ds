// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "ReadStream.h"

#include <vector>

namespace pds
{

u64 ReadStream::ReadRawData( void *dest, u64 count )
{
	// cap the end position
	u64 end_pos = this->DataPosition + count;
	if( end_pos > this->DataSize )
	{
		end_pos = this->DataSize;
		count = end_pos - this->DataPosition;
	}

	// copy the data and move the position
	memcpy( dest, &this->Data[this->DataPosition], count );
	this->DataPosition = end_pos;
	return count;
}

}
// namespace pds