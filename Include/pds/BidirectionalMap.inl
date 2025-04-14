// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

namespace pds
{

template<class _Kty, class _Vty, class _Base>
inline bool BidirectionalMap<_Kty, _Vty, _Base>::operator==( const BidirectionalMap &rval ) const 
{ 
	return ctle::bimap::operator==(rval); 
}

template<class _Kty, class _Vty, class _Base>
inline bool BidirectionalMap<_Kty, _Vty, _Base>::operator!=( const BidirectionalMap &rval ) const 
{ 
	return ctle::bimap::operator!=(rval); 
}

}
// namespace pds
