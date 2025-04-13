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
	invalid_count	 = 0x01, // an invalid size of lists etc
	null_not_allowed = 0x02, // an object which is not allowed to be empty/null, is empty/null
	missing_object	 = 0x04, // a required object is missing
	invalid_object	 = 0x08, // an object is invalid or used in an invalid way
	invalid_setup	 = 0x10, // the set up of an object or system is invalid 
	invalid_value	 = 0x20, // a value or index is out of bounds or not allowed
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

	void ReportErrorDescription( u64 errorid, const std::string &errorDescription, const char *filename, int fileline, const char *funcsig )
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

	const vector<ErrorDescription> &GetErrorDescriptions() const
	{
		return this->ErrorDescriptions;
	}

private:
	uint ErrorCount = 0;
	u64 Errors = 0;
	vector<ErrorDescription> ErrorDescriptions;
	bool RecordErrorDescriptions = true;
};

}
// namespace pds

#endif//__PDS__ENTITYVALIDATOR_H__