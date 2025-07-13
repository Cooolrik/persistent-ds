// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include "Tests.h"

using ctle::from_hex_string;
using ctle::to_string;
using ctle::to_hex_string;

TEST( TypeTests, StandardTypes )
{
	EXPECT_EQ( sizeof( u8 ), 1 );
	EXPECT_EQ( sizeof( u16 ), 2 );
	EXPECT_EQ( sizeof( u32 ), 4 );
	EXPECT_EQ( sizeof( u64 ), 8 );
	EXPECT_EQ( sizeof( i8 ), 1 );
	EXPECT_EQ( sizeof( i16 ), 2 );
	EXPECT_EQ( sizeof( i32 ), 4 );
	EXPECT_EQ( sizeof( i64 ), 8 );
	EXPECT_EQ( sizeof( uint ), 4 );
	EXPECT_EQ( sizeof( uuid ), 16 );
	EXPECT_EQ( sizeof( hash ), 32 );
}

TEST( TypeTests, HexStringFunctions )
{
	uuid uuidval;
	uuidval._data_q[0] = 0xb2b1a2a178563412;
	uuidval._data_q[1] = 0xc8c7c6c5c4c3c2c1;

	std::string expected_hexuuidval = "12345678-a1a2-b1b2-c1c2-c3c4c5c6c7c8";
	EXPECT_EQ( to_string( uuidval ), expected_hexuuidval );

	const u8 hashdata[32] = {
		0xf6,0x48,0x54,0x2d,0xf8,0xcc,0xf2,0x1f,
		0xd3,0x4e,0x95,0xf6,0x7d,0xf5,0xf2,0xb4,
		0xf2,0x72,0x72,0xaa,0x14,0xf5,0x03,0x09,
		0x0c,0xc4,0x76,0x6f,0xe2,0x78,0xc4,0xb5
	};
	hash hashval;
	memcpy( &hashval, hashdata, 32 );
	std::string expected_hexhashval = "f648542df8ccf21fd34e95f67df5f2b4f27272aa14f503090cc4766fe278c4b5";
	EXPECT_EQ( to_string( hashval ), expected_hexhashval );

	u8 u8val = 0x13;
	std::string expected_hexu8 = "13";
	EXPECT_EQ( to_hex_string( u8val ), expected_hexu8 );

	u16 u16val = 0x1234;
	std::string expected_hexu16 = "1234";
	EXPECT_EQ( to_hex_string( u16val ), expected_hexu16 );

	u32 u32val = 0x0218a782;
	std::string expected_hexu32 = "0218a782";
	EXPECT_EQ( to_hex_string( u32val ), expected_hexu32 );

	u64 u64val = 0x35023218a7828505;
	std::string expected_hexu64 = "35023218a7828505";
	EXPECT_EQ( to_hex_string( u64val ), expected_hexu64 );
}

//TEST( TypeTests, SHA256Hashing )
//{
//	if( true )
//	{
//		hash sha;
//
//		u8 testdata[] = {
//			0x34,0x2b,0x1f,0x3e,0x61,
//			0x4b,0x03,0x4b,0x02,0x36,
//			0x05,0x5c,0x17,0x29,0x3d,
//			0x53,0x0e,0x5e,0x5b,0x4d,
//			0x52,0x5f,0x12,0x20,0x0a,
//			0x56,0x31,0x3b,0x2c,0x06,
//			0x51,0x28,0x28,0x5d,0x05,
//			0x59,0x2b,0x41,0x0d,0x1f,
//			0x01,0x01,0x1b,0x1f,0x09,
//			0x2c,0x13,0x01,0x46,0x19,
//			0x05,0x3e,0x3c,0x2d,0x58,
//			0x16,0x5f,0x19,0x0f,0x07,
//			0x39,0x48,0x46,0x4b,0x23,
//			0x06,0x15,0x0b,0x44,0x18,
//			0x0e,0x38,0x56,0x0e,0x0a,
//			0x0e,0x54,0x43,0x0a,0x31,
//			0x2d,0x51,0x0d,0x2a,0x5a,
//			0x09,0x06,0x10,0x23,0x24,
//			0x23,0x33,0x2e,0x1d,0x56,
//			0x48,0x2f,0x4a,0x33,0x06
//		};
//
//		ctle::calculate_sha256_hash( sha, testdata, sizeof( testdata ) );
//
//		u8 expected_hash[32] = {
//			0xf6,0x48,0x54,0x2d,0xf8,0xcc,0xf2,0x1f,
//			0xd3,0x4e,0x95,0xf6,0x7d,0xf5,0xf2,0xb4,
//			0xf2,0x72,0x72,0xaa,0x14,0xf5,0x03,0x09,
//			0x0c,0xc4,0x76,0x6f,0xe2,0x78,0xc4,0xb5
//		};
//
//		EXPECT_EQ( memcmp( sha.data, expected_hash, 32 ), 0 );
//	}
//}

TEST( TypeTests, Test_item_ref )
{
	item_ref ref;
	EXPECT_TRUE( !ref );
	uuid val = ref;
	EXPECT_TRUE( val == uuid_zero );
	EXPECT_TRUE( val == item_ref::null() );
	EXPECT_TRUE( ref == item_ref() );

	item_ref ref2 = item_ref::make_ref();
	EXPECT_TRUE( ref != ref2 );
	EXPECT_TRUE( !( ref == ref2 ) );
	EXPECT_TRUE( ref < ref2 ); // ref is zero, ref2 must be more 
	val = ref2;
	EXPECT_TRUE( val != uuid_zero );
	EXPECT_TRUE( uuid_zero < val );

	ref = std::move( ref2 );
	EXPECT_TRUE( ref != ref2 );
	EXPECT_TRUE( !( ref == ref2 ) );
	EXPECT_TRUE( ref2 < ref ); // ref2 is zero, ref must be more 

	ref2 = ref;
	EXPECT_TRUE( ref == ref2 );
	EXPECT_TRUE( !( ref != ref2 ) );
	EXPECT_TRUE( !( ref2 < ref ) );
}

TEST( TypeTests, Test_entity_ref )
{
	entity_ref ref;
	EXPECT_TRUE( !ref );
	hash val = ref;
	EXPECT_EQ( val, hash_zero );
	EXPECT_EQ( entity_ref( val ), entity_ref::null() );
	EXPECT_EQ( ref, entity_ref() );

	entity_ref ref2 = random_value<hash>();
	EXPECT_NE( ref, ref2 );
	EXPECT_TRUE( !( ref == ref2 ) );
	EXPECT_LT( ref, ref2 ); // ref is zero, ref2 must be more 
	val = ref2;
	EXPECT_NE( val, hash_zero );
	EXPECT_LT( hash_zero, val );

	ref = std::move( ref2 );
	EXPECT_NE( ref, ref2 );
	EXPECT_TRUE( !( ref == ref2 ) );
	EXPECT_LT( ref2, ref ); // ref2 is zero, ref must be more 

	ref2 = ref;
	EXPECT_EQ( ref, ref2 );
	EXPECT_TRUE( !( ref != ref2 ) );
	EXPECT_TRUE( !( ref2 < ref ) );
}

// this tests the hash function, the less than operator and the equals operator
template<class K> void TestSetAndMapWithKey()
{
	const size_t item_count = 100;

	std::map<K, size_t> mmap;
	std::unordered_map<K, size_t> umap;
	std::set<K> mset;
	std::unordered_set<K> uset;
	for( size_t i = 0; i < item_count; ++i )
	{
		K key = random_value<K>();
		mmap.emplace( key, i );
		umap.emplace( key, i );
		mset.emplace( key );
		uset.emplace( key );
	}

	EXPECT_EQ( item_count, mmap.size() );
	EXPECT_EQ( item_count, umap.size() );
	EXPECT_EQ( item_count, mset.size() );
	EXPECT_EQ( item_count, uset.size() );

	auto lookup = inverse_map( mmap );

	for( size_t i = 0; i < item_count; ++i )
	{
		// lookup key from value
		bool found = lookup.find( i ) != lookup.end();
		EXPECT_TRUE( found );
		K key = lookup[i];

		// make sure that the key is in map and umap, and returns same value
		found = mmap.find( key ) != mmap.end();
		EXPECT_TRUE( found );
		EXPECT_EQ( mmap[key], i );
		found = umap.find( key ) != umap.end();
		EXPECT_TRUE( found );
		EXPECT_EQ( umap[key], i );

		// make sure the key is in the two sets
		found = mset.find( key ) != mset.end();
		EXPECT_TRUE( found );
		found = uset.find( key ) != uset.end();
		EXPECT_TRUE( found );
	}
}

TEST( TypeTests, Test_functors )
{
	setup_random_seed();

	for( uint pass_index = 0; pass_index < global_number_of_passes; ++pass_index )
	{
		TestSetAndMapWithKey<uuid>();
		TestSetAndMapWithKey<hash>();
	}
}
