// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__DIRECTEDGRAPH_H__
#define __PDS__DIRECTEDGRAPH_H__

#include "fwd.h"
#include <set>

namespace pds
{

// DirectedGraph keeps a directed graph, using pairs of key values, with frist -> second. 
// The graph can optionally be kept acyclic (DAG), be rooted (where a separate list keeps track of the roots of 
// the graph, and makes sure those roots can reach all nodes), and can also enforce a single root.

enum class directed_graph_flags : uint
{
	acyclic = 0x1,		// if set, validation make sure the directed graph is acyclic (DAG)
	rooted = 0x2,		// if set, validation will make sure all graph vertices can be reachable from the root(s)
	single_root = 0x4,	// if set, validation will make sure there is a single graph root vertex
};


template<
	class _Ty, 
	directed_graph_flags _Flags, // = 0, a combination of directed_graph_flags flags for the behaviour of the graph
	class _EdgesSetTy,			 // = std::set<std::pair<const _Ty, const _Ty>>, the set type to use for the graph
	class _RootsSetTy			 // = std::set<_Ty>, the set type to use for the graph root(s)
> class DirectedGraph
{
public:
	using node_type = _Ty;
	using allocator_type = typename _EdgesSetTy::allocator_type;

	using set_type = _EdgesSetTy;
	using value_type = typename set_type::value_type;
	using iterator = typename set_type::iterator;
	using const_iterator = typename set_type::iterator;

	using roots_set_type = _RootsSetTy;

	static constexpr const bool type_acyclic = ( uint(_Flags) & uint(directed_graph_flags::acyclic) ) != 0;
	static constexpr const bool type_rooted = ( uint(_Flags) & uint(directed_graph_flags::rooted) ) != 0;
	static constexpr const bool type_single_root = ( uint(_Flags) & uint(directed_graph_flags::single_root) ) != 0;

	class MF;

	DirectedGraph() = default;
	DirectedGraph( const DirectedGraph &other ) = default;
	DirectedGraph &operator=( const DirectedGraph &other ) = default;
	DirectedGraph( DirectedGraph &&other ) = default;
	DirectedGraph &operator=( DirectedGraph &&other ) = default;
	~DirectedGraph() = default;

public:
	// inserts an edge, unless it already exists
	void InsertEdge( const node_type &key, const node_type &value );

	// adds a root node, unless it already exists 
	void AddRoot( const node_type &root );

	// find a particular key-value pair (directed edge)
	bool HasEdge( const node_type &key, const node_type &value ) const;

	// get the range of iterators to enumerate all successors of the key, or end() if no successor exists in the graph
	std::pair<iterator, iterator> GetSuccessors( const node_type &key );
	std::pair<const_iterator, const_iterator> GetSuccessors( const node_type &key ) const;

	// direct access to Edges structure
	set_type &Edges() noexcept { return this->v_Edges; }
	const set_type &Edges() const noexcept { return this->v_Edges; }

	// direct access to Roots set
	roots_set_type &Roots() noexcept { return this->v_Roots; }
	const roots_set_type &Roots() const noexcept { return this->v_Roots; }

	// value compare operators
	bool operator==( const DirectedGraph &rval ) const;
	bool operator!=( const DirectedGraph &rval ) const;

private:
	friend MF;

	roots_set_type v_Roots;
	set_type v_Edges;
};

}
// namespace pds

#include "DirectedGraph.inl"

#endif//__PDS__DIRECTEDGRAPH_H__