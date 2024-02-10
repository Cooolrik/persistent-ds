// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#define PDS_IMPLEMENTATION
#include "Tests.h"

#include <ctle/log.h>

static void silence_global_log_function( ctle::log_level /*level*/, const char */*function_name*/, const char */*message*/ )
{
}

int main( int argc, char **argv )
{
	testing::InitGoogleTest( &argc, argv );

	ctle::set_global_log_function( &silence_global_log_function );

	return RUN_ALL_TESTS();
}