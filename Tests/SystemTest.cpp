// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <iostream>
#include <filesystem>

#include "TestPackA/TestEntityA.h"
#include "TestPackA/TestItemA.h"
#include "TestPackA/TestEntityB.h"

#include <pds/EntityManager.h>
#include <pds/WriteStream.h>
#include <pds/EntityWriter.h>

using namespace pds;
#include <pds/_pds_macros.inl>

using namespace TestPackA;
namespace fs = std::filesystem;

int main()
{
	pds::EntityManager eh;
	
	auto testfolder = fs::absolute("./testfolder");
	fs::create_directory(testfolder);
	if( eh.Initialize( testfolder.u8string(), { TestPackA::GetPackageRecord() } ) != status::ok )
		return -1;
	
	auto pentA = std::make_shared<TestEntityA>();
	TestEntityA &entA = *pentA;
	auto pentB = std::make_shared<TestEntityB>();
	TestEntityB &entB = *pentB;
	
	entA.Name() = "hej";
	entB.Name2() = "hej2";
	
	auto refA = eh.AddEntity( pentA ).value();
	auto refB = eh.AddEntity( pentB ).value();

	pentA.reset();
	pentB.reset();

	eh.UnloadNonReferencedEntities();

	eh.LoadEntity( refA );
	eh.LoadEntity( refB );

	auto l1 = eh.GetLoadedEntity( refA );
	auto l2 = eh.GetLoadedEntity( refB );
	
	auto pentA2 = TestEntityA::EntitySafeCast( l1 );
	auto pentB2 = TestEntityB::EntitySafeCast( l2 );
	
	return 0;
}

