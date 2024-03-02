// pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

#pragma once

#include <sstream>

#include <pds/pds.h>

namespace pds
{


// EntityValidator is used to validate an entity's integrity before locking the entity and writing it to disk
struct ValidationError
{
	static const u64 NoError = 0x00;
	static const u64 InvalidCount = 0x01;	// an invalid size of lists etc
	static const u64 NullNotAllowed = 0x02;	// an object is empty/null, and this is not allowed in the type
	static const u64 MissingObject = 0x04;	// a required object is missing
	static const u64 InvalidObject = 0x08;	// an object is invalid or used in an invalid way
	static const u64 InvalidSetup = 0x10;	// the set up of an object or system is invalid 
	static const u64 InvalidValue = 0x20;	// a value or index is out of bounds or not allowed
};

class EntityValidator
{
	uint ErrorCount = 0;
	u64 ErrorIds = 0;
	std::vector<std::string> ErrorStrings;

	bool RecordErrorStrings = true;

public:
	void ReportError( u64 errorid )
	{
		++this->ErrorCount;
		this->ErrorIds |= errorid;
	}

	void ReportError( u64 errorid, const std::string &errorDescription, const char *filename, int fileline, const char *funcsig )
	{
		this->ReportError( errorid );

		// if set, reports individual error strings to the error strings log
		if( this->RecordErrorStrings )
		{
			std::stringstream errorStream;

			errorStream << "Validation error:\n"
				<< "\tfilename=" << filename << "\n"
				<< "\tfileline=" << fileline << "\n"
				<< "\tfuncsig=" << funcsig << "\n"
				<< "\terrorid=" << errorid << "\n"
				<< "\terrorDescription=" << errorDescription << "\n";

			this->ErrorStrings.resize( this->ErrorCount );
			this->ErrorStrings[this->ErrorCount - 1] = errorStream.str();
		}
	}

	void SetRecordErrorStrings( bool value )
	{
		this->RecordErrorStrings = value;
	}
	bool GetRecordErrorStrings() const
	{
		return this->RecordErrorStrings;
	}

	void Clear()
	{
		this->ErrorCount = 0;
		this->ErrorIds = 0;
		this->ErrorStrings.clear();
	}

	uint GetErrorCount() const
	{
		return this->ErrorCount;
	}
	u64 GetErrorIds() const
	{
		return this->ErrorIds;
	}

	const std::vector<std::string> &GetErrorStrings() const
	{
		return this->ErrorStrings;
	}
};



}
// namespace pds