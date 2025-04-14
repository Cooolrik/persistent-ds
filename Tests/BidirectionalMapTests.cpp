// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include "Tests.h"

#include <pds/EntityValidator.h>
#include <pds/EntityWriter.h>
#include <pds/EntityReader.h>
#include <pds/WriteStream.h>
#include <pds/ReadStream.h>

#include <pds/mf/BidirectionalMap_MF.h>

using pds::BidirectionalMap;

TEST( BidirectionalMapTests, InversedMapTest )
{
	// build std map and bimap
	pds::BidirectionalMap<uuid,uuid> bdmap;
	std::map<uuid,uuid> kvmap;
	for( size_t inx=0; inx<100; ++inx )
	{
		auto k = uuid::generate();
		auto v = uuid::generate();

		kvmap.emplace(k,v);	
		bdmap.insert(k,v);
	}

	// make sure that the values are correctly mapped backwards
	for( const auto &kvpair: kvmap )
	{
		auto k = kvpair.first;
		auto v = kvpair.second;

		auto kres = bdmap.get_key( v );
		EXPECT_TRUE( kres.second );
		EXPECT_EQ( kres.first, k );
		auto vres = bdmap.get_value( k );
		EXPECT_TRUE( vres.second );
		EXPECT_EQ( vres.first, v );
	}
}

template<class T> void BidirectionalMapTests_TestKeyType( const WriteStream &ws, EntityWriter &ew )
{
	typedef BidirectionalMap<T, string> Dict;

	using std::to_string;
	using ctle::to_string;

	Dict random_dict;

	// create random dictionary with random entries (half of them null)
	for( size_t inx=0; inx<80; ++inx )
	{
		// find a key which has not already been added to the dictionary
		auto key = random_value<T>();
		while( random_dict.get_value(key).second )
		{
			key = random_value<T>();
		}

		// covert the key into a string, and add key value pair
		auto value = to_string(key);
		random_dict.insert( key, value );
	}

	EntityValidator validator;

	// validate
	EXPECT_EQ( Dict::MF::Validate( random_dict, validator ), status::ok );

	// write dictionary to stream
	u64 start_pos = ws.GetPosition();
	EXPECT_EQ( Dict::MF::Write( random_dict, ew ), status::ok );

	// set up a temporary entity reader 
	ReadStream rs( ws.GetData(), ws.GetSize() );
	EntityReader er( rs );
	rs.SetPosition( start_pos );

	// read back the dictionary 
	Dict readback_dict;
	EXPECT_EQ( Dict::MF::Read( readback_dict, er ), status::ok );

	// validate
	EXPECT_EQ( Dict::MF::Validate( readback_dict, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );

	EXPECT_TRUE( Dict::MF::Equals( &random_dict, &readback_dict ) );

	// compare the values in the dictionaries
	EXPECT_TRUE( random_dict.size() == readback_dict.size() );
	for( auto &val: random_dict )
	{
		auto k = val.first;
		auto v = val.second;

		auto kres = readback_dict.get_key( v );
		EXPECT_TRUE( kres.second );
		EXPECT_EQ( kres.first, k );
		auto vres = readback_dict.get_value( k );
		EXPECT_TRUE( vres.second );
		EXPECT_EQ( vres.first, v );
	}
}

TEST( BidirectionalMapTests, ReadWriteTests )
{
	setup_random_seed();

	for( uint pass_index = 0; pass_index < global_number_of_passes; ++pass_index )
	{
		WriteStream ws;
		EntityWriter ew( ws );

		BidirectionalMapTests_TestKeyType<i8>( ws, ew );
		BidirectionalMapTests_TestKeyType<i16>( ws, ew );
		BidirectionalMapTests_TestKeyType<i32>( ws, ew );
		BidirectionalMapTests_TestKeyType<i64>( ws, ew );
		
		BidirectionalMapTests_TestKeyType<u8>( ws, ew );
		BidirectionalMapTests_TestKeyType<u16>( ws, ew );
		BidirectionalMapTests_TestKeyType<u32>( ws, ew );
		BidirectionalMapTests_TestKeyType<u64>( ws, ew );
		
		BidirectionalMapTests_TestKeyType<float>( ws, ew );
		BidirectionalMapTests_TestKeyType<double>( ws, ew );
		
		BidirectionalMapTests_TestKeyType<uuid>( ws, ew );
		BidirectionalMapTests_TestKeyType<hash>( ws, ew );
	}
}
