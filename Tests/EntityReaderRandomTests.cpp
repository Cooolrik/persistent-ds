// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include "Tests.h"

#include <pds/EntityReader.h>
#include <pds/ReadStream.h>

TEST( RandomFileDataReadTest, Test_EntityReader_with_random_file_fuzzing_expect_no_exceptions )
{
	setup_random_seed();

	for( uint pass_index = 0; pass_index < global_number_of_passes; ++pass_index )
	{
		std::vector<u8> random_file_data;
		random_vector( random_file_data, 10000, 50000 );

		ReadStream rs( random_file_data.data(), random_file_data.size() );
		EntityReader er( rs );

		// try reading some values. calls are allowed to fail with error, but not crash
		for( size_t i = 0; i < 1000; ++i )
		{
			i32 v;
			auto res = er.Read( "i", 1, v );
			EXPECT_TRUE( res == status::ok || res == status::cant_read );
			std::vector<i8> dv;
			res = er.Read( "dv", 2, dv );
			EXPECT_TRUE( res == status::ok || res == status::cant_read );
			std::string ds;
			res = er.Read( "ds", 2, ds );
			EXPECT_TRUE( res == status::ok || res == status::cant_read );
		}
	}
}
