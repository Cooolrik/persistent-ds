// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

namespace pds
{

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline void DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::InsertEdge( const node_type &key, const node_type &value )
{
	this->v_Edges.emplace( key, value );
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline void DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::AddRoot( const node_type &root )
{
	this->v_Roots.emplace( root );
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline bool DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::HasEdge( const node_type &key, const node_type &value ) const
{
	return this->v_Edges.find( pair_type( key, value ) ) != this->v_Edges.end();
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline std::pair<typename DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::iterator, typename DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::iterator>
DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::GetSuccessors( const node_type &key )
{
	return std::pair<iterator, iterator>(
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::inf ) ),
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::sup ) )
	);
}

template<class _Ty, directed_graph_flags _Flags, class _EdgesSetTy, class _RootsSetTy>
inline std::pair<typename DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::const_iterator, typename DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::const_iterator>
DirectedGraph<_Ty, _Flags, _EdgesSetTy, _RootsSetTy>::GetSuccessors( const node_type &key ) const
{
	return std::pair<const_iterator, const_iterator>(
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::inf ) ),
		this->v_Edges.lower_bound( std::pair<_Ty, _Ty>( key, data_type_information<_Ty>::sup ) )
	);
}

}
// namespace pds