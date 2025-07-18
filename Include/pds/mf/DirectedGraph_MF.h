// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__DIRECTEDGRAPH_MF_H__
#define __PDS__DIRECTEDGRAPH_MF_H__

#include <stack>
#include <queue>
#include <ctle/log.h>

#include "../DirectedGraph.h"

#include "../EntityWriter.h"
#include "../EntityReader.h"
#include "../EntityValidator.h"

namespace pds
{
#include "../_pds_macros.inl"

// Management Functions for the DirectedGraph template class
template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
class DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF
{
	using _MgmCl = DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>;

	static bool set_contains( const std::set<_Ty> &set, const _Ty &val );

	static void ValidateNoCycles( const _MgmCl::set_type &edges, EntityValidator &validator );
	static void ValidateRooted( const std::set<_Ty> &roots, const std::set<_Ty> &downstream_nodes, const _MgmCl::set_type &edges, EntityValidator &validator );

public:
	static status Clear( _MgmCl &obj );
	static status DeepCopy( _MgmCl &dest, const _MgmCl *source );
	static bool Equals( const _MgmCl *lval, const _MgmCl *rval );
	static status Write( const _MgmCl &obj, EntityWriter &writer );
	static status Read( _MgmCl &obj, EntityReader &reader );
	static status Validate( const _MgmCl &obj, EntityValidator &validator );

	// additional validation with external data
	template<class _Table> static bool ValidateAllKeysAreContainedInTable( const _MgmCl &obj, EntityValidator &validator, const _Table &otherTable, const char *otherTableName );
};

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline bool DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::set_contains( const std::set<_Ty> &set, const _Ty &val )
{
	return set.find( val ) != set.end();
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline status DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::Clear( _MgmCl &obj )
{
	obj.v_Roots.clear();
	obj.v_Edges.clear();
	return status::ok;
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline status DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::DeepCopy( _MgmCl &dest, const _MgmCl *source )
{
	if( !source )
	{
		MF::Clear( dest );
		return status::ok;
	}

	// replace contents
	dest.v_Roots = std::set<_Ty>( source->v_Roots.begin(), source->v_Roots.end() );
	dest.v_Edges = set_type( source->v_Edges.begin(), source->v_Edges.end() );
	return status::ok;
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline bool DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::Equals( const _MgmCl *lval, const _MgmCl *rval )
{
	// early out if the pointers are equal (includes nullptr)
	if( lval == rval )
		return true;

	// early out if one of the pointers is nullptr (both can't be null because of above test)
	if( !lval || !rval )
		return false;

	// early out if the sizes are not the same 
	if( lval->v_Roots.size() != rval->v_Roots.size() )
		return false;
	if( lval->v_Edges.size() != rval->v_Edges.size() )
		return false;

	// compare roots
	auto lval_roots_it = lval->v_Roots.begin();
	auto rval_roots_it = rval->v_Roots.begin();
	while( lval_roots_it != lval->v_Roots.end() )
	{
		if( ( *lval_roots_it ) != ( *rval_roots_it ) )
			return false;
		++lval_roots_it;
		++rval_roots_it;
	}

	// compare all the edges
	auto lval_edges_it = lval->v_Edges.begin();
	auto rval_edges_it = rval->v_Edges.begin();
	while( lval_edges_it != lval->v_Edges.end() )
	{
		if( ( *lval_edges_it ) != ( *rval_edges_it ) )
			return false;
		++lval_edges_it;
		++rval_edges_it;
	}

	return true;
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline status DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::Write( const _MgmCl &obj, EntityWriter &writer )
{
	// store the roots 
	std::vector<_Ty> roots( obj.v_Roots.begin(), obj.v_Roots.end() );
	if( !writer.Write( pdsKeyMacro( Roots ), roots ) )
		return status::cant_write;

	// collect the keys-value pairs into a vector and store as an array
	std::vector<_Ty> graph_pairs( obj.v_Edges.size() * 2 );
	size_t index = 0;
	for( auto it = obj.v_Edges.begin(); it != obj.v_Edges.end(); ++it, ++index )
	{
		graph_pairs[index * 2 + 0] = it->first;
		graph_pairs[index * 2 + 1] = it->second;
	}
	if( !writer.Write( pdsKeyMacro( Edges ), graph_pairs ) )
		return status::cant_write;

	// sanity check, make sure all sections were written
	ctSanityCheck( index == obj.v_Edges.size() );

	return status::ok;
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline status DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::Read( _MgmCl &obj, EntityReader &reader )
{
	size_t map_size = {};

	// read the roots 
	std::vector<_Ty> roots;
	if( !reader.Read( pdsKeyMacro( Roots ), roots ) )
		return status::cant_read;
	obj.v_Roots = std::set<_Ty>( roots.begin(), roots.end() );

	// read in the graph pairs
	std::vector<_Ty> graph_pairs;
	if( !reader.Read( pdsKeyMacro( Edges ), graph_pairs ) )
		return status::cant_read;

	// insert into map
	obj.v_Edges.clear();
	map_size = graph_pairs.size() / 2;
	for( size_t index = 0; index < map_size; ++index )
	{
		obj.InsertEdge( graph_pairs[index * 2 + 0], graph_pairs[index * 2 + 1] );
	}

	return status::ok;
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
void DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::ValidateNoCycles( const _MgmCl::set_type &edges, EntityValidator &validator )
{
	// Do a depth-first search, find all nodes starting from the root nodes 
	// Note: Only reports first found cycle, if any
	std::stack<_Ty> stack;
	std::set<_Ty> on_stack;
	std::set<_Ty> checked;

	// try all nodes
	for( const auto &p : edges )
	{
		const _Ty &node = p.first;

		// if already checked, skip
		if( set_contains( checked, node ) )
			continue;

		// push on to stack
		stack.push( node );

		// run until all items on stack are popped again
		while( !stack.empty() )
		{
			// get the top from the stack
			_Ty curr = stack.top();

			if( !set_contains( checked, curr ) )
			{
				// if we havent checked, mark the node as on the stack
				checked.insert( curr );
				on_stack.insert( curr );
			}
			else
			{
				// we are done with it, remove from stack
				on_stack.erase( curr );
				stack.pop();
			}

			// list all nodes downstream from curr
			auto itr = edges.lower_bound( std::pair<_Ty, _Ty>( curr, element_type_information<_Ty>::inf ) );
			auto itr_end = edges.upper_bound( std::pair<_Ty, _Ty>( curr, element_type_information<_Ty>::sup ) );
			while( itr != itr_end )
			{
				ctSanityCheck( itr->first == curr );
				const _Ty &child = itr->second;

				if( !set_contains( checked, child ) )
				{
					// this has not been checked, add on top of stack to be checked next
					stack.push( child );
				}
				else if( set_contains( on_stack, child ) )
				{
					// This child node is already marked on the stack, so we have already visited it once 
					// We have a cycle, report it, and return
					pdsValidationError( validation_error_flags::invalid_setup )
						<< "The node " << to_string(child)
						<< " in Graph is a part of a cycle, but the graph is flagged as being acyclic."
						<< pdsValidationErrorEnd;
					return;
				}

				++itr;
			}
		}
	}

	// no cycles found, all good
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline void DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::ValidateRooted( const std::set<_Ty> &roots, const std::set<_Ty> &downstream_nodes, const _MgmCl::set_type &edges, EntityValidator &validator )
{
	std::queue<_Ty> queue;
	std::set<_Ty> reached;

	// push all the roots onto the queue
	for( const auto &n : roots )
	{
		queue.push( n );
	}

	// try to reach all downstream nodes from the roots
	while( !queue.empty() )
	{
		// get the first value in the queue
		_Ty curr = queue.front();
		queue.pop();

		// if already reached, skip
		if( set_contains( reached, curr ) )
			continue;

		// mark it as reached
		reached.insert( curr );

		// check downstream nodes
		auto itr = edges.lower_bound( std::pair<_Ty, _Ty>( curr, element_type_information<_Ty>::inf ) );
		auto itr_end = edges.upper_bound( std::pair<_Ty, _Ty>( curr, element_type_information<_Ty>::sup ) );
		while( itr != itr_end )
		{
			ctSanityCheck( itr->first == curr );
			const _Ty &child = itr->second;

			if( !set_contains( reached, child ) )
			{
				queue.push( child );
			}

			++itr;
		}
	}

	// make sure all downstream nodes were reached
	for( auto n : downstream_nodes )
	{
		if( !set_contains( reached, n ) )
		{
			// This child node is already marked on the stack, so we have already visited it once 
			// We have a cycle, report it, and return
			pdsValidationError( validation_error_flags::invalid_setup )
				<< "The node " << to_string(n)
				<< " in Graph could not be reached from (any of) the root(s) in the Roots set."
				<< pdsValidationErrorEnd;
		}
	}
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline status DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::Validate( const _MgmCl &obj, EntityValidator &validator )
{
	// make a set of all nodes with incoming edges
	std::set<_Ty> downstream_nodes;
	for( const auto &p : obj.v_Edges )
	{
		if( !set_contains( downstream_nodes, p.second ) )
			downstream_nodes.insert( p.second );
	}

	// the rest of the nodes are root nodes (no incoming edges)
	std::set<_Ty> root_nodes;
	for( const auto &p : obj.v_Edges )
	{
		if( !set_contains( downstream_nodes, p.first ) )
			root_nodes.insert( p.first );
	}

	// check for single root object
	if( type_single_root )
	{
		if( root_nodes.size() != 1 )
		{
			pdsValidationError( validation_error_flags::invalid_count ) 
				<< "The number of roots found when searching through the graph is " << root_nodes.size() 
				<< " but the graph is required to have exactly one root." 
				<< pdsValidationErrorEnd;
		}
	}

	// if this is a rooted graph, make sure that the v_Roots set is correct
	if( type_rooted )
	{
		if( type_single_root )
		{
			if( obj.v_Roots.size() != 1 )
			{
				pdsValidationError( validation_error_flags::invalid_count ) 
					<< "The graph is single rooted, but the Roots set has " << obj.v_Roots.size() 
					<< " nodes. The Roots set must have exactly one node." 
					<< pdsValidationErrorEnd;
			}
		}

		// make sure that all nodes in the v_Roots list do not have incoming edges
		for( auto n : obj.v_Roots )
		{
			if( set_contains( downstream_nodes, n ) )
			{
				pdsValidationError( validation_error_flags::invalid_object ) 
					<< "Node " << to_string(n) << " in the Roots set has incoming edges, which makes it invalid as a root node."
					<< pdsValidationErrorEnd;
			}
		}

		// make sure that all nodes that are root nodes (no incoming edges) are in the v_Roots list
		for( auto n : root_nodes )
		{
			if( !set_contains( obj.v_Roots, n ) )
			{
				pdsValidationError( validation_error_flags::missing_object ) 
					<< "Node " << to_string(n) << " has no incoming edges, so is by definition a root, but is not listed in the Roots set."
					<< pdsValidationErrorEnd;
			}
		}

		// make sure no node is unreachable from the roots
		ValidateRooted( obj.v_Roots, downstream_nodes, obj.v_Edges, validator );
	}

	// check for cycles if the graph is acyclic
	if( type_acyclic )
	{
		ValidateNoCycles( obj.v_Edges, validator );
	}

	return status::ok;
}


template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
template<class _Table>
bool DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::MF::ValidateAllKeysAreContainedInTable( const _MgmCl &obj, EntityValidator &validator, const _Table &otherTable, const char *otherTableName )
{
	// collect all items 
	std::set<_Ty> nodes;
	for( const auto &p : obj.v_Edges )
	{
		if( !set_contains( nodes, p.first ) )
			nodes.insert( p.first );
		if( !set_contains( nodes, p.second ) )
			nodes.insert( p.second );
	}

	// make sure they are all in the table
	for( auto it = nodes.begin(); it != nodes.end(); ++it )
	{
		if( !_Table::MF::ContainsKey( otherTable, ( *it ) ) )
		{
			pdsValidationError( validation_error_flags::missing_object ) 
				<< "The key " << ( *it ) 
				<< " is missing in " << otherTableName 
				<< pdsValidationErrorEnd;
		}
	}

	return true;
}

#include "../_pds_undef_macros.inl"
}
// namespace pds
#endif//__PDS__DIRECTEDGRAPH_MF_H__
