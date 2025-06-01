// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

// build ctle & pds library implementations in this source file
#define PDS_IMPLEMENTATION
#define CTLE_IMPLEMENTATION

#include <xxhash.h>
#include <xxhash.c>

#include <ctle/base_types.h>
#include <ctle/uuid.h>
#include <ctle/digest.h>
#include <ctle/string_funcs.h>
#include <ctle/ntup.h>
#include <ctle/hasher.h>

#include <pds/pds.h>
