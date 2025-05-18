// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include "TestPackA/fwd.h"
//#include "TestPackA/TestEntityB.h"

//#include <pds/ValueTypes.h>
//#include <pds/Varying.h>

//using namespace TestPackA;

//#include <ctle/ntup.h>
//#include <pds/pds.h>
//#include <pds/ValueTypes.h>

//#include <pds/writer_templates.h>
//#include <pds/reader_templates.h>

//#include <pds/dynamic_types.h>
//#include <pds/element_types.h>
//#include <pds/container_types.h>

#include <pds/WriteStream.h>
#include <pds/EntityWriter.h>

using namespace pds;
#include <pds/_pds_macros.inl>

int main()
	{
	TestPackA::f32 val;

	val = 32.f;
	
	//auto type = dynamic_types::new_type( element_type_index::dt_bool, container_type_index::ct_vector );
	//
	//WriteStream ws;
	//EntityWriter ew(ws);
   	//
	//auto section = ew.BeginWriteSection( pdsKeyMacro(Hej) );
	//
	//f32 val = 123;
	//section->Write( pdsKeyMacro(Hoj), val );
	//
	//ew.EndWriteSection(section);

	//std::vector<int> myvec = {1,2,3};
	
	//pds::clear_value_type( myvec );

	//pds::clear_combined_type( myvec );

	//WriteStream ws;
	//
	//f64 val = 12.3f;
	//auto res = write_single_value<serialization_type_index::vt_float, f64>( ws, pdsKeyMacro( Hej ), &val );
	//

	//pds::EntityManager eh;
	//
	//if( eh.Initialize( "./testfolder", { GetPackageRecord() } ) != pds::Status::Ok )
	//	return -1;
	//
	//auto pentA = std::make_shared<TestEntityA>();
	//TestEntityA &entA = *pentA;
	//auto pentB = std::make_shared<TestEntityB>();
	////TestEntityB &entB = *pentB;
	//
	//entA.Name() = "hej";
	//entA.TestVariableA().set();
	//entA.TestVariableA().value().Insert( pds::item_ref_zero );
	//entA.TestVariableA().value()[pds::item_ref_zero].Name() = "Ullebulle";
	//
	////entB.Name() = "hej";
	//
	//eh.AddEntity( pentA );
	//eh.AddEntity( pentB );

	//auto e1ref = pds::entity_ref( hex_string_to_value<hash>( "5771e7bb72582619919523b8bc5567a6e17678cdb82f79c2d9e7ce93aa8ddfe6" ) );
	//auto e2ref = pds::entity_ref( hex_string_to_value<hash>( "89b1d8e9e5ac248c2e154f4c212d5a8ea9b9d43408a502a5e55339feb32e50f0" ) );
	//
	//eh.LoadEntity( e1ref );
	//eh.LoadEntity( e2ref );
	//
	//auto l1 = eh.GetLoadedEntity( e1ref );
	//auto l2 = eh.GetLoadedEntity( e2ref );
	//
	//auto pentA = TestEntityA::MF::EntitySafeCast( l1 );
	//auto pentB = TestEntityB::MF::EntitySafeCast( l2 );
	
	////auto t3ent = std::make_shared<tp3::TestEntity>();
	////t3ent->Name3() = te->Name2();
	////t3ent->OptionalText3() = te->OptionalText2();
	////auto e3ref = eh.AddEntity( t3ent ).first;

	return 0;
	}

