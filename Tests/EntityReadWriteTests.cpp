// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include "Tests.h"

#include <pds/EntityWriter.h>
#include <pds/EntityReader.h>
#include <pds/WriteStream.h>
#include <pds/ReadStream.h>

template<class T> void TestEntityWriter_TestValueType( const WriteStream &ws, EntityWriter &ew, const std::vector<std::string> &key_names )
{
	const T value = random_value<T>();
	const optional_value<T> opt_value = random_optional_value<T>();

	std::vector<T> value_vec;
	random_vector<T>( value_vec, 10, 100 );
	optional_vector<T> opt_value_vec;
	random_optional_vector<T>( opt_value_vec, 10, 100 );
	idx_vector<T> value_inxarr;
	random_idx_vector<T>( value_inxarr, 10, 100 );
	optional_idx_vector<T> opt_value_inxarr;
	random_optional_idx_vector<T>( opt_value_inxarr, 10, 100 );

	const std::string key = key_names[rand() % key_names.size()];

	// write value
	u64 start_pos = ws.GetPosition();
	status result = ew.Write<T>( key.c_str(), (u8)key.size(), value );
	EXPECT_EQ( result , status::ok );

	// write an optional value
	result = ew.Write<optional_value<T>>( key.c_str(), (u8)key.size(), opt_value );
	EXPECT_EQ( result , status::ok );

	// write a random vector of values
	result = ew.Write<std::vector<T>>( key.c_str(), (u8)key.size(), value_vec );
	EXPECT_EQ( result , status::ok );

	// write an optional random vector of values
	result = ew.Write<optional_vector<T>>( key.c_str(), (u8)key.size(), opt_value_vec );
	EXPECT_EQ( result , status::ok );

	// write a random indexed array of values
	result = ew.Write<idx_vector<T>>( key.c_str(), (u8)key.size(), value_inxarr );
	EXPECT_EQ( result , status::ok );

	// write an optional random indexed array of values
	result = ew.Write<optional_idx_vector<T>>( key.c_str(), (u8)key.size(), opt_value_inxarr );
	EXPECT_EQ( result , status::ok );

	// set up a temporary entity reader and read back the values
	ReadStream rs( ws.GetData(), ws.GetSize() );
	EntityReader er( rs );
	rs.SetPosition( start_pos );

	// read back value
	T read_back_value = {};
	result = er.Read( key.c_str(), (u8)key.size(), read_back_value );
	EXPECT_EQ( result , status::ok );
	EXPECT_EQ( value, read_back_value );

	// read back optional value
	optional_value<T> read_back_opt_value;
	result = er.Read( key.c_str(), (u8)key.size(), read_back_opt_value );
	EXPECT_EQ( result , status::ok );
	EXPECT_EQ( opt_value.has_value(), read_back_opt_value.has_value() );
	if( opt_value.has_value() )
	{
		EXPECT_EQ( opt_value.value(), read_back_opt_value.value() );
	}

	// read back vector of values
	std::vector<T> read_back_value_vec;
	result = er.Read( key.c_str(), (u8)key.size(), read_back_value_vec );
	EXPECT_EQ( result , status::ok );
	EXPECT_EQ( value_vec, read_back_value_vec );

	// read back optional vector of values
	optional_vector<T> read_back_opt_value_vec;
	result = er.Read( key.c_str(), (u8)key.size(), read_back_opt_value_vec );
	EXPECT_EQ( result , status::ok );
	EXPECT_EQ( opt_value_vec.has_value(), read_back_opt_value_vec.has_value() );
	if( opt_value_vec.has_value() )
	{
		EXPECT_EQ( opt_value_vec.values(), read_back_opt_value_vec.values() );
	}

	// read back indexed_array of values
	idx_vector<T> read_back_value_inxarr;
	result = er.Read( key.c_str(), (u8)key.size(), read_back_value_inxarr );
	EXPECT_EQ( result , status::ok );
	EXPECT_EQ( value_inxarr.values(), read_back_value_inxarr.values() );
	EXPECT_EQ( value_inxarr.index(), read_back_value_inxarr.index() );

	// read back optional indexed_array of values
	optional_idx_vector<T> read_back_opt_value_inxarr;
	result = er.Read( key.c_str(), (u8)key.size(), read_back_opt_value_inxarr );
	EXPECT_EQ( result , status::ok );
	EXPECT_EQ( read_back_opt_value_inxarr.has_value(), read_back_opt_value_inxarr.has_value() );
	if( opt_value_inxarr.has_value() )
	{
		EXPECT_EQ( opt_value_inxarr.values(), read_back_opt_value_inxarr.values() );
		EXPECT_EQ( opt_value_inxarr.index(), read_back_opt_value_inxarr.index() );
	}
}

TEST( EntityReadWriteTests, TestEntityWriterAndReadback )
{
	setup_random_seed();

	for( uint pass_index = 0; pass_index < ( global_number_of_passes ); ++pass_index )
	{
		WriteStream ws;
		EntityWriter ew( ws );

		std::vector<std::string> key_names =
		{
		std::string( "PLbYYDnVEpoPO2Yz" ),
		std::string( "h3HHExIVS4eCngO1UZr4" ),
		std::string( "At1w2H4jZe" ),
		std::string( "Hi2I" ),
		std::string( "uGp3TU67GSkitXB" ),
		std::string( "c" ),
		std::string( "PQkmX7Og" ),
		std::string( "hellofoobar" ),
		std::string( "ARJVMxS6yF6lasdjllg8jE292A7" ),
		std::string( "z8ERgfAM8" ),
		};

		TestEntityWriter_TestValueType<bool>( ws, ew, key_names );

		TestEntityWriter_TestValueType<i8>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i16>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i32>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i64>( ws, ew, key_names );

		TestEntityWriter_TestValueType<u8>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u16>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u32>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u64>( ws, ew, key_names );

		TestEntityWriter_TestValueType<float>( ws, ew, key_names );
		TestEntityWriter_TestValueType<double>( ws, ew, key_names );

		TestEntityWriter_TestValueType<f32vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f64vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f32vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f64vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f32vec4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f64vec4>( ws, ew, key_names );

		TestEntityWriter_TestValueType<i8vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i16vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i32vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i64vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i8vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i16vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i32vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i64vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i8vec4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i16vec4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i32vec4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<i64vec4>( ws, ew, key_names );

		TestEntityWriter_TestValueType<u8vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u16vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u32vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u64vec2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u8vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u16vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u32vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u64vec3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u8vec4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u16vec4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u32vec4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<u64vec4>( ws, ew, key_names );

		TestEntityWriter_TestValueType<f32mat2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f64mat2>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f32mat3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f64mat3>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f32mat4>( ws, ew, key_names );
		TestEntityWriter_TestValueType<f64mat4>( ws, ew, key_names );

		TestEntityWriter_TestValueType<uuid>( ws, ew, key_names );
		TestEntityWriter_TestValueType<hash>( ws, ew, key_names );
		TestEntityWriter_TestValueType<string>( ws, ew, key_names );

		TestEntityWriter_TestValueType<item_ref>( ws, ew, key_names );
		TestEntityWriter_TestValueType<entity_ref>( ws, ew, key_names );
	}
}
