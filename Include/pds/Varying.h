// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
#pragma once
#ifndef __PDS__VARYING_H__
#define __PDS__VARYING_H__

#include "fwd.h"

namespace pds
{
class Varying
{
public:
	class MF;

	Varying() = default;
	Varying( const Varying &rval );
	Varying &operator=( const Varying &rval );
	Varying( Varying &&rval ) noexcept;
	Varying &operator=( Varying &&rval ) noexcept;
	~Varying();

	// Initialize the varying object, and allocate the data
	status Initialize( element_type_index elementType, container_type_index containerType );

	// Initialize the object using the combined type, and return the reference to the object
	template <class _Ty> _Ty &Initialize();

	// Returns true if the object has and allocated data object
	bool IsInitialized() const noexcept;

	// The current data type in the object, a combination of element and container types
	std::tuple<element_type_index, container_type_index> Type() const noexcept;

	// value compare operators
	bool operator==( const Varying &rval ) const;
	bool operator!=( const Varying &rval ) const;

	// Check if the data is of the template combined type _Ty
	template<class _Ty> bool IsA() const noexcept;

	// Retreive a reference to the data in the object, as a combined type _Ty
	template<class _Ty> const _Ty &Data() const;
	template<class _Ty> _Ty &Data();

protected:
	friend MF;

	element_type_index type_m = {};
	container_type_index container_type_m = {};
	void *data_m = {};

	bool Deinitialize();
};

}
// namespace pds

#include "Varying.inl"

#endif//__PDS__VARYING_H__
