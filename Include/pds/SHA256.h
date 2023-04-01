// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "pds.h"

#include <vector>

namespace pds
{
namespace SHA256
{

// calculate the sha256 hash of the source data
Status CalculateHash( hash &destHash, const u8 *srcData, size_t srcDataLength );
Status CalculateHash( u8 destDigest[32], const u8 *srcData, size_t srcDataLength );

}
}