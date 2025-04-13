// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once
#ifndef __PDS__ENTITYVALIDATOR_H__
#define __PDS__ENTITYVALIDATOR_H__

#include "fwd.h"

namespace pds
{

enum class validation_error_flag : u64
{
	vf_invalid_count	= 0x01,	// an invalid size of lists etc
	vf_null_not_allowed	= 0x02,	// an object which is not allowed to be empty/null, is empty/null
	vf_missing_bject	= 0x04,	// a required object is missing
	vf_invalid_object	= 0x08,	// an object is invalid or used in an invalid way
	vf_invalid_setup	= 0x10,	// the set up of an object or system is invalid 
	vf_invalid_value	= 0x20,	// a value or index is out of bounds or not allowed
};

// EntityValidator is used to validate an entity's integrity before locking the entity and writing it to disk
class EntityValidator
{
public:
	struct ErrorDescription
	{
		u64 id;
		std::string description;
		std::string file;
		int line;
		std::string func;
	};

	void ReportError( u64 errorid )
	{
		++this->ErrorCount;
		this->Errors |= errorid;
	}

	void ReportError( u64 errorid, const std::string &errorDescription, const char *filename, int fileline, const char *funcsig )
	{
		this->ReportError( errorid );

		// if set, reports individual error strings to the error strings log
		if( this->RecordErrorDescriptions )
		{
			this->ErrorDescriptions.emplace_back(
				ErrorDescription{ 
					errorid,
					errorDescription,
					(filename)?(std::string(filename)):(std::string("")),
					fileline,
					(funcsig)?(std::string(funcsig)):(std::string(""))
				}
			);
		}
	}

	void SetRecordErrorDescriptions( bool value )
	{
		this->RecordErrorDescriptions = value;
	}

	bool GetRecordErrorDescriptions() const
	{
		return this->RecordErrorDescriptions;
	}

	void Clear()
	{
		this->ErrorCount = 0;
		this->Errors = 0;
		this->ErrorDescriptions.clear();
	}

	uint GetErrorCount() const
	{
		return this->ErrorCount;
	}

	u64 GetErrors() const
	{
		return this->Errors;
	}

	const std::vector<ErrorDescription> &GetErrorDescriptions() const
	{
		return this->ErrorDescriptions;
	}

private:
	uint ErrorCount = 0;
	u64 Errors = 0;
	std::vector<ErrorDescription> ErrorDescriptions;
	bool RecordErrorDescriptions = true;
};

}
// namespace pds

#endif//__PDS__ENTITYVALIDATOR_H__