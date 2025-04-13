// pds Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/ctle/blob/main/LICENSE
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

// Macro marker that the _pds_macros.inl file has been included and the macros are defined. Also makes sure that the
// _pds_macros.inl file is not included twice, without first including _pds_undef_macros.inl before the second include.
#ifdef _PDS_MACROS_INCLUDED
#error The _pds_macros.inl file is included more than once, without including _pds_undef_macros.inl to undefine the macros before the next include of _pds_macros.inl
#endif//_PDS_MACROS_INCLUDED
#define _PDS_MACROS_INCLUDED

// include the ctle _macros.inl macro definitions, which are also used in pds
#include <ctle/_macros.inl>

// pdsValidationError logs a validation error with the EntityValidator
#define pdsValidationError( errorid ) if( !validator.GetRecordErrorDescriptions() ) { validator.ReportError( (u64)errorid ); } else { auto _errorId = errorid; std::stringstream _errorStringStream; _errorStringStream
#define pdsValidationErrorEnd ""; validator.ReportErrorDescription( (u64)_errorId , _errorStringStream.str() , __FILE__ , __LINE__ , __func__ ); }

// pdsKeyMacro is used to define a key in the pds file stream
#define pdsKeyMacro( name ) (#name) , (u8(sizeof(#name)-1))


