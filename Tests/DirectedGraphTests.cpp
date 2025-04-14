// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#include "Tests.h"

#include <pds/mf/DirectedGraph_MF.h>
#include <pds/EntityValidator.h>
#include <pds/EntityWriter.h>
#include <pds/EntityReader.h>
#include <pds/WriteStream.h>
#include <pds/ReadStream.h>

#include "TestHelpers/structure_generation.h"

TEST( DirectedGraphTests, DirectedGraphBasicTest )
{
	setup_random_seed();

	// create basic graph
	typedef DirectedGraph<int> Graph;
	Graph dg;

	dg.Edges().emplace( 0, 1 );
	dg.Edges().emplace( 1, 2 );
	dg.Edges().emplace( 2, 3 );
	dg.Edges().emplace( 3, 1 );

	EntityValidator validator;
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );
}

TEST( DirectedGraphTests, DirectedGraphItemRefTest )
{
	setup_random_seed();

	// create basic graph
	typedef DirectedGraph<item_ref, directed_graph_flags::acyclic> Graph;
	Graph dg;

	item_ref A = item_ref::make_ref();
	item_ref B = item_ref::make_ref();
	item_ref C = item_ref::make_ref();
	item_ref D = item_ref::make_ref();

	dg.Edges().emplace( A, B );
	dg.Edges().emplace( B, C );
	dg.Edges().emplace( C, A );
	dg.Edges().emplace( A, D );

	EntityValidator validator;
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 1 ) ); // should report a cycle
}

TEST( DirectedGraphTests, DirectedGraphDuplicateEdgesTest )
{
	setup_random_seed();

	// create basic graph
	typedef DirectedGraph<int> Graph;
	Graph dg;

	// create two identical edges in the graph
	dg.InsertEdge( 0, 1 );
	dg.InsertEdge( 0, 1 );

	// the second edge should never be added
	EXPECT_EQ( dg.Edges().size(), size_t( 1 ) );

	// make sure this is invalid
	EntityValidator validator;
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );
}

TEST( DirectedGraphTests, DirectedGraphAcyclicTest )
{
	setup_random_seed();

	typedef DirectedGraph<i64, directed_graph_flags::acyclic> Graph;

	// create a tree, which by definition does not have cycles
	Graph dg;
	GenerateRandomTreeRecursive( dg, 3 );

	// make sure this is valid
	EntityValidator validator;
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );

	// now, insert a cycle into the tree
	dg.Edges().emplace( 0, 1 );
	dg.Edges().emplace( 1, 2 );
	dg.Edges().emplace( 2, 0 );

	// make sure this is not valid anymore, and that the error is the cycle
	validator.Clear();
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_NE( validator.GetErrorCount(), uint( 0 ) );
	EXPECT_TRUE( validator.GetErrors() == validation_error_flags::invalid_setup );
}

TEST( DirectedGraphTests, DirectedGraphSingleRootTest )
{
	setup_random_seed();

	typedef DirectedGraph<i64, directed_graph_flags::single_root> Graph;

	// create a tree, which by definition only has one root
	Graph dg;
	GenerateRandomTreeRecursive( dg, 2 );

	// make sure this is valid
	EntityValidator validator;
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );

	// now, insert a second root into the tree, by adding two random nodes
	dg.Edges().emplace( random_value<i64>(), random_value<i64>() );

	// make sure this is not valid anymore, and that the error is multiple roots
	validator.Clear();
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_NE( validator.GetErrorCount(), uint( 0 ) );
	EXPECT_TRUE( validator.GetErrors() == validation_error_flags::invalid_count );
}

TEST( DirectedGraphTests, DirectedGraphRootedTest )
{
	setup_random_seed();

	typedef DirectedGraph<i64, directed_graph_flags::rooted> Graph;

	// create a forest of trees with multiple roots
	Graph dg;
	size_t roots = capped_rand( 1, 9 );
	for( size_t i = 0; i < roots; ++i )
	{
		i64 rootid = random_value<i64>();

		dg.Roots().insert( rootid );
		GenerateRandomTreeRecursive( dg, 2, 0, rootid );
	}

	// make sure this is valid
	EntityValidator validator;
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );

	// now, remove the first root in the roots list
	dg.Roots().erase( dg.Roots().begin() );

	// make sure this is not valid anymore, and that the error the missing root node in the Roots list
	validator.Clear();
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_NE( validator.GetErrorCount(), uint( 0 ) );
	EXPECT_EQ( validator.GetErrors(), ( validation_error_flags::invalid_setup | validation_error_flags::missing_object ) );
}

TEST( DirectedGraphTests, DirectedGraphSceneGraphTest )
{
	setup_random_seed();

	// single root, acyclic, with root defined in the Roots list
	typedef DirectedGraph<ctle::uuid, ( directed_graph_flags::acyclic | directed_graph_flags::rooted | directed_graph_flags::single_root )> Graph;

	// create a tree, which by definition does not have cycles, a single root, and add the root to the roots list
	Graph dg;
	uuid root_node = random_value<uuid>();
	dg.Roots().insert( root_node );
	GenerateRandomTreeRecursive( dg, 3, 0, root_node );

	// get a set of all downstream nodes
	std::set<Graph::node_type> downstream_nodes;
	for( const auto &p : dg.Edges() )
	{
		if( downstream_nodes.find( p.second ) == downstream_nodes.end() )
			downstream_nodes.insert( p.second );
	}

	// make all downstream nodes point at a single leaf node 
	uuid leaf_node = random_value<uuid>();
	for( auto p : downstream_nodes )
	{
		dg.Edges().insert( std::pair<uuid, uuid>( p, leaf_node ) );
	}

	// make sure this is valid (no cycles)
	EntityValidator validator;
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_EQ( validator.GetErrorCount(), uint( 0 ) );

	// add two new roots, insert a cycle (leaf node points at original root, which is no longer a root), dont add the new roots to the root list, and add two identical edges to the graph
	dg.Edges().insert( std::pair<uuid, uuid>( random_value<uuid>(), root_node ) );
	dg.Edges().insert( std::pair<uuid, uuid>( random_value<uuid>(), root_node ) );
	dg.Edges().insert( std::pair<uuid, uuid>( leaf_node, root_node ) );
	dg.Edges().insert( std::pair<uuid, uuid>( leaf_node, root_node ) );

	// make sure this is not valid anymore, and that the error the missing root node in the Roots list
	validator.Clear();
	EXPECT_EQ( Graph::MF::Validate( dg, validator ), status::ok );
	EXPECT_NE( validator.GetErrorCount(), uint( 0 ) );
	const validation_error_flags expected_error = (
		validation_error_flags::invalid_setup
		| validation_error_flags::missing_object
		| validation_error_flags::invalid_object
		| validation_error_flags::invalid_count
		);
	EXPECT_EQ( validator.GetErrors(), expected_error );
}

template<class _Ty, directed_graph_flags _Flags>
void ReadWriteTest( WriteStream &ws, EntityWriter &ew )
{
	typedef DirectedGraph<_Ty, _Flags> Graph;

	// generate a tree, one or multiple roots 
	Graph dg;
	size_t roots = ( Graph::type_single_root ) ? 1 : capped_rand( 0, 9 );
	for( size_t i = 0; i < roots; ++i )
	{
		_Ty rootid = random_value<_Ty>();
		dg.Roots().insert( rootid );
		GenerateRandomTreeRecursive( dg, 2, 0, rootid );
	}

	// store to file
	u64 start_pos = ws.GetPosition();
	EXPECT_EQ( Graph::MF::Write( dg, ew ), status::ok );

	// read from file
	ReadStream rs( ws.GetData(), ws.GetSize() );
	EntityReader er( rs );
	rs.SetPosition( start_pos );

	// read back the graph 
	Graph readback_dg;
	EXPECT_EQ( Graph::MF::Read( readback_dg, er ), status::ok );

	// compare values
	EXPECT_EQ( dg.Edges(), readback_dg.Edges() );
}

template<class _Ty>
void ReadWriteTypeTest( WriteStream &ws, EntityWriter &ew )
{
	// all combinations of Acyclic (0x1), Rooted (0x2), and Single root (0x4)
	ReadWriteTest<_Ty, directed_graph_flags(0x0)>( ws, ew );
	ReadWriteTest<_Ty, directed_graph_flags(0x1)>( ws, ew );
	ReadWriteTest<_Ty, directed_graph_flags(0x2)>( ws, ew );
	ReadWriteTest<_Ty, directed_graph_flags(0x3)>( ws, ew );
	ReadWriteTest<_Ty, directed_graph_flags(0x4)>( ws, ew );
	ReadWriteTest<_Ty, directed_graph_flags(0x5)>( ws, ew );
	ReadWriteTest<_Ty, directed_graph_flags(0x6)>( ws, ew );
	ReadWriteTest<_Ty, directed_graph_flags(0x7)>( ws, ew );
}

TEST( DirectedGraphTests, DirectedGraphSerializeTest )
{
	setup_random_seed();

	for( uint pass_index = 0; pass_index < global_number_of_passes; ++pass_index )
	{
		WriteStream ws;
		EntityWriter ew( ws );

		ReadWriteTypeTest<int>( ws, ew );
		ReadWriteTypeTest<uint>( ws, ew );
		ReadWriteTypeTest<i64>( ws, ew );
		ReadWriteTypeTest<string>( ws, ew );
		ReadWriteTypeTest<uuid>( ws, ew );
		ReadWriteTypeTest<hash>( ws, ew );
	}
}
