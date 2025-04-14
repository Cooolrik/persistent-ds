// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE


#include "Tests.h"
#include <pds/mf/IndexedVector_MF.h>

#include <pds/EntityValidator.h>
#include <pds/EntityWriter.h>
#include <pds/EntityReader.h>
#include <pds/WriteStream.h>
#include <pds/ReadStream.h>


using pds::IndexedVector;

template<class T> void IndexedVector_TestType( const WriteStream &ws, EntityWriter &ew )
{
	IndexedVector<T> vec;

	// keep a map of indices and entries
	std::unordered_map<int,T> valueMap;

	// resize vector to 65536 entries, most empty
	vec.values().resize(65536);

	// create random vector with entries, avoid conflicts
	for( size_t inx=0; inx<200; ++inx )
	{
		auto val = random_value<T>();

		// find an index which has not already been added to the dictionary
		auto index = random_value<uint16_t>();
		while( valueMap.find(index) != valueMap.end() )
		{
			index = random_value<uint16_t>();
		}

		// covert the key into a string, and add key value pair
		vec.index().push_back(index);
		vec.values()[index] = val;
		valueMap.emplace( index, val );
	}

	EntityValidator validator;

	// validate
	EXPECT_EQ( IndexedVector<T>::MF::Validate( vec, validator ), status::ok );

	// write dictionary to stream
	u64 start_pos = ws.GetPosition();
	EXPECT_EQ( IndexedVector<T>::MF::Write( vec, ew ), status::ok );

	// set up a temporary entity reader 
	ReadStream rs( ws.GetData(), ws.GetSize() );
	EntityReader er( rs );
	rs.SetPosition( start_pos );

	// read back the values 
	IndexedVector<T> readback_vec;
	EXPECT_EQ( IndexedVector<T>::MF::Read( readback_vec, er ), status::ok );

	// validate
	EXPECT_EQ( IndexedVector<T>::MF::Validate( readback_vec, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );

	EXPECT_TRUE( IndexedVector<T>::MF::Equals( &vec, &readback_vec ) );

	// compare the values in the vectors
	EXPECT_EQ( vec.index().size() , readback_vec.index().size() );
	EXPECT_EQ( vec.values().size() , readback_vec.values().size() );
	for( size_t inx=0; inx<vec.index().size(); ++inx )
	{
		EXPECT_EQ( vec.index()[inx], readback_vec.index()[inx] );
	}
	for( size_t inx=0; inx<vec.values().size(); ++inx )
	{
		EXPECT_EQ( vec.values()[inx], readback_vec.values()[inx] );
	}

}

TEST( IndexedVectorTests, ReadWriteTests )
{
	setup_random_seed();
	
	for( uint pass_index = 0; pass_index < global_number_of_passes; ++pass_index )
	{
		WriteStream ws;
		EntityWriter ew( ws );
	
		IndexedVector_TestType<i8>( ws, ew );
		IndexedVector_TestType<i16>( ws, ew );
		IndexedVector_TestType<i32>( ws, ew );
		IndexedVector_TestType<i64>( ws, ew );
		
		IndexedVector_TestType<u8>( ws, ew );
		IndexedVector_TestType<u16>( ws, ew );
		IndexedVector_TestType<u32>( ws, ew );
		IndexedVector_TestType<u64>( ws, ew );
		
		IndexedVector_TestType<float>( ws, ew );
		IndexedVector_TestType<double>( ws, ew );
		
		IndexedVector_TestType<uuid>( ws, ew );
		IndexedVector_TestType<hash>( ws, ew );
	}
}
