// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once

#include "SHA256.h"

// directly include librock_sha256.c
// silence warning we can't control
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4456 )
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#include <picosha2.h>

// re-enable warnings again
#ifdef _MSC_VER
#pragma warning( pop )
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace pds
{
namespace SHA256
{

Status CalculateHash( u8 destDigest[32], const u8 *srcData, size_t srcDataLength )
	{
	picosha2::hash256_one_by_one hasher;
	
	hasher.process(srcData, srcData + srcDataLength);
	hasher.finish();
	hasher.get_hash_bytes(destDigest, destDigest + 32);

	return Status::Ok;
	}

Status CalculateHash( hash &destHash, const u8 *srcData, size_t srcDataLength )
	{
	return CalculateHash( destHash.digest, srcData, srcDataLength );
	}

}
}
