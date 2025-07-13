// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

namespace pds
{

template<class _Ty, class _IdxTy, class _Base>
inline bool IndexedVector<_Ty, _IdxTy, _Base>::operator==( const IndexedVector &rval ) const
{
	return _Base::operator==( rval );
}

template<class _Ty, class _IdxTy, class _Base> 
inline bool IndexedVector<_Ty, _IdxTy, _Base>::operator!=( const IndexedVector &rval ) const
{
	return _Base::operator!=( rval );
}

}
// namespace pds
