// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE
//
// _pds_macros.inl & _pds_undef_macros.inl are used to define convenience macros 
// for implementation files. 
//
// usage: include _macros.inl in implementation, *after* any other header file, so 
// as to not pollute any of those files with the macros defined. 
//
// Include the _undef_macros.inl after the macros are used in the same file. This undefines/removes the
// macros from the rest of files used, and does not pollute the other parts of compilation.
//
// The _pds_macros.inl and _pds_undef_macros.inl can be safely included multiple times, 
// but pair each inclusion of _pds_macros.inl with an inclusion of _pds_undef_macros.inl, 
// before the next inclusion of _pds_macros.inl
//

// Remove the marker that _pds_macros.inl has been included. Also make sure that _pds_macros.inl actually was included before the
// inclusion of _pds_undef_macros.inl
#ifndef _PDS_MACROS_INCLUDED
#error File include mismatch: The _pds_macros.inl has not been included before including the _pds_undef_macros.inl file. 
#endif//_PDS_MACROS_INCLUDED
#undef _PDS_MACROS_INCLUDED

// include the ctle _undef_macros.inl which undefs the used ctle macro definitions
#include <ctle/_undef_macros.inl>

#undef pdsValidationError
#undef pdsValidationErrorEnd
#undef pdsKeyMacro
