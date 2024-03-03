// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include "pds.h"
#include <set>

namespace pds
{

// DirectedGraph keeps a directed graph, using a pair of key values, with frist -> second. 
// The graph can optionally be kept acyclic (DAG), be rooted (where a separate list keeps track of the roots of the graph), and can also have a single root.

enum DirectedGraphFlags : uint
{
	Acyclic = 0x1, // if set, validation make sure the directed graph is acyclic (DAG)
	Rooted = 0x2, // if set, validation will make sure all graph vertices can be reachable from the root(s)
	SingleRoot = 0x4, // if set, validation will make sure there is a single graph root vertex
};

template<class _Ty, uint _Flags = 0, class _SetTy = std::set<std::pair<const _Ty, const _Ty>>>
class DirectedGraph
{
public:
	using node_type = _Ty;
	using allocator_type = typename _SetTy::allocator_type;

	using set_type = _SetTy;
	using value_type = typename set_type::value_type;
	using iterator = typename set_type::iterator;
	using const_iterator = typename set_type::iterator;

	static const bool type_acyclic = ( _Flags & DirectedGraphFlags::Acyclic ) != 0;
	static const bool type_rooted = ( _Flags & DirectedGraphFlags::Rooted ) != 0;
	static const bool type_single_root = ( _Flags & DirectedGraphFlags::SingleRoot ) != 0;

	class MF;
	friend MF;

	DirectedGraph() = default;
	DirectedGraph( const DirectedGraph &other ) = default;
	DirectedGraph &operator=( const DirectedGraph &other ) = default;
	DirectedGraph( DirectedGraph &&other ) = default;
	DirectedGraph &operator=( DirectedGraph &&other ) = default;
	~DirectedGraph() = default;

private:
	std::set<_Ty> v_Roots;
	set_type v_Edges;

public:
	// inserts an edge, unless it already exists
	void InsertEdge( const node_type &key, const node_type &value );

	// find a particular key-value pair (directed edge)
	bool HasEdge( const node_type &key, const node_type &value ) const;

	// get the range of iterators to enumerate all successors of the key, or end() if no successor exists in the graph
	std::pair<iterator, iterator> GetSuccessors( const node_type &key );
	std::pair<const_iterator, const_iterator> GetSuccessors( const node_type &key ) const;

	// direct access to Edges structure
	set_type &Edges() noexcept { return this->v_Edges; }
	const set_type &Edges() const noexcept { return this->v_Edges; }

	// direct access to Roots set
	std::set<_Ty> &Roots() noexcept { return this->v_Roots; }
	const std::set<_Ty> &Roots() const noexcept { return this->v_Roots; }
};

template<class _Ty, uint _Flags, class _SetTy>
inline void DirectedGraph<_Ty, _Flags, _SetTy>::InsertEdge( const node_type &key, const node_type &value )
{
	this->v_Edges.emplace( key, value );
}

template<class _Ty, uint _Flags, class _SetTy>
inline bool DirectedGraph<_Ty, _Flags, _SetTy>::HasEdge( const node_type &key, const node_type &value ) const
{
	return this->v_Edges.find( pair_type( key, value ) ) != this->v_Edges.end();
}

template<class _Ty, uint _Flags, class _SetTy>
inline std::pair<typename DirectedGraph<_Ty, _Flags, _SetTy>::iterator, typename DirectedGraph<_Ty, _Flags, _SetTy>::iterator>
DirectedGraph<_Ty, _Flags, _SetTy>::GetSuccessors( const node_type &key )
{
	return std::pair<iterator, iterator>(
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::inf ) ),
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::sup ) )
	);
}

template<class _Ty, uint _Flags, class _SetTy>
inline std::pair<typename DirectedGraph<_Ty, _Flags, _SetTy>::const_iterator, typename DirectedGraph<_Ty, _Flags, _SetTy>::const_iterator>
DirectedGraph<_Ty, _Flags, _SetTy>::GetSuccessors( const node_type &key ) const
{
	return std::pair<const_iterator, const_iterator>(
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::inf ) ),
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::sup ) )
	);
}

}
// namespace pds