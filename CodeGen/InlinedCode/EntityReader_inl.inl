#include "_pds_macros.inl"

EntityReader::EntityReader(ReadStream& _sstream)
	: sstream(_sstream)
	, end_position(_sstream.GetSize())
{
}

EntityReader::EntityReader( ReadStream &_sstream, const u64 _end_position ) : sstream( _sstream ), end_position( _end_position ) {}

EntityReader::~EntityReader() {}

// Read a section. 
// If the section is null, the section is directly closed, nullptr+success is returned 
// from BeginReadSection, and EndReadSection shall not be called.
status_return<EntityReader*> EntityReader::BeginReadSection( const char *key, const u8 key_length, const bool null_section_is_allowed )
{
	ctValidate(!this->active_subsection, status::invalid)
		<< "This reader already has an active subsection. Close the subsection before opening a new."
		<< ctValidateEnd;

	// read block header
	const u64 end_of_section = begin_read_large_block(sstream, serialization_type_index::vt_subsection, key, key_length);
	ctValidate(end_of_section != 0, status::cant_read)
		<< "begin_read_large_block() failed unexpectedly, stream is probably corrupted"
		<< ctValidateEnd;

	// if we are already at the end, end the block directly (if it is allowed)
	if (end_of_section == sstream.GetPosition())
	{
		return (end_read_empty_large_block(sstream, key, null_section_is_allowed, end_of_section) == reader_status::fail)
			? status::invalid
			: status::ok;
	}

	// allocate the subsection and return it to the caller to be used to read items in the subsection
	this->active_subsection = std::unique_ptr<EntityReader>( new EntityReader( this->sstream, end_of_section ) );
	return this->active_subsection.get();
}

status EntityReader::EndReadSection( const EntityReader *section_reader )
{
	ctValidate(section_reader == this->active_subsection.get(), status::invalid)
		<< "Invalid parameter section_reader, it does not match the internal expected value."
		<< ctValidateEnd;

	ctValidate(end_read_large_block( this->sstream, this->active_subsection->end_position ), status::invalid )
		<< "end_read_large_block failed unexpectedly, the stream is corrupted or the reading is out of sync with the stream."
		<< ctValidateEnd;

	this->active_subsection.reset();
	this->active_subsection_end_pos = 0;
	return status::ok;
}

// Build a sections array. 
// If the section is null, the section array is directly closed, nullptr+success is returned 
// from BeginReadSectionsArray, and EndReadSectionsArray shall not be called.
status_return<EntityReader *> EntityReader::BeginReadSectionsArray( const char *key, const u8 key_length, const bool null_section_array_is_allowed, std::vector<i32> *dest_index )
{
	ctValidate(!this->active_subsection, status::invalid)
		<< "This reader already has an active subsection. Close the subsection before opening a new."
		<< ctValidateEnd;

	// read block header. if we are already at the end, the block is empty, end the block and make sure empty is allowed
	const u64 end_of_section = begin_read_large_block(sstream, serialization_type_index::vt_array_subsection, key, key_length);
	ctValidate(end_of_section != 0, status::cant_read)
		<< "begin_read_large_block() failed unexpectedly, stream is probably corrupted"
		<< ctValidateEnd;

	// if we are already at the end, end the block directly (if it is allowed)
	if (end_of_section == sstream.GetPosition())
	{
		return (end_read_empty_large_block(sstream, key, null_section_array_is_allowed, end_of_section) == reader_status::fail)
			? status::invalid
			: status::ok;
	}

	// read item size & count and index if it exists, or make sure we do not expect an index
	size_t per_item_size = 0;
	ctValidate(read_array_metadata_and_index(sstream, per_item_size, this->active_subsection_array_size, end_of_section, dest_index), status::invalid)
		<< "read_array_metadata_and_index() failed unexpectedly, stream is probably corrupted"
		<< ctValidateEnd;

	// set the subsection index to ~0 to indicate that we are before the first subsection
	this->active_subsection_index = size_t(~0);

	// allocate the subsection and return it to the caller to be used to read items in the subsection
	this->active_subsection = std::unique_ptr<EntityReader>( new EntityReader( this->sstream, end_of_section ) );
	return this->active_subsection.get();
}

status EntityReader::BeginReadSectionInArray( const EntityReader *sections_array_reader, const size_t section_index, bool *dest_section_has_data )
{
	ctValidate(sections_array_reader == this->active_subsection.get(), status::invalid_param)
		<< "Invalid parameter sections_array_reader, it does not match the internal expected value."
		<< ctValidateEnd;

	ctValidate((this->active_subsection_index + 1) == section_index, status::invalid_param)
		<< "Synch error, incorrect subsection index: " << section_index
		<< " expected: " << (this->active_subsection_index + 1)
		<< ctValidateEnd;

	ctValidate(section_index < this->active_subsection_array_size, status::invalid_param)
		<< "Invalid subsection index: " << section_index
		<< " total array size: " << this->active_subsection_array_size
		<< ctValidateEnd;

	this->active_subsection_index = section_index;
	const u64 section_size = sstream.Read<u64>();
	this->active_subsection_end_pos = sstream.GetPosition() + section_size;

	if (dest_section_has_data)
	{
		// if dest_section_has_data is not null, it is allowed that there is no data, return if that is the case
		*dest_section_has_data = (section_size != 0);
	}
	else
	{
		// if dest_section_has_data is null, the section is not allowed to be empty, and we need to check this
		ctValidate(section_size != 0, status::invalid)
			<< "A section which is not allowed to be empty, is empty in the stream."
			<< ctValidateEnd;
	}

	return status::ok;
}

status EntityReader::EndReadSectionInArray( const EntityReader *sections_array_reader, const size_t section_index )
{
	ctValidate(sections_array_reader == this->active_subsection.get(), status::invalid_param)
		<< "Invalid parameter sections_array_reader, it does not match the internal expected value."
		<< ctValidateEnd;

	ctValidate(this->active_subsection_index == section_index, status::invalid_param)
		<< "Synch error, incorrect subsection index: " << section_index
		<< " expected: " << this->active_subsection_index
		<< ctValidateEnd;

	const u64 end_pos = sstream.GetPosition();
	ctValidate(end_pos == this->active_subsection_end_pos, status::invalid)
		<< "The current subsection did not end where expected. Current" << end_pos
		<< " expected: " << this->active_subsection_end_pos
		<< ctValidateEnd;
	
	return status::ok;
}

status EntityReader::EndReadSectionsArray( const EntityReader *sections_array_reader )
{
	ctValidate(sections_array_reader == this->active_subsection.get(), status::invalid)
		<< "Invalid parameter sections_array_reader, it does not match the internal expected value."
		<< ctValidateEnd;

	ctValidate((this->active_subsection_index + 1) == this->active_subsection_array_size, status::invalid_param)
		<< "Synch error, the subsection index does not equal the end of the array. Total sections read count:" << (this->active_subsection_index + 1)
		<< " expected: " << this->active_subsection_array_size
		<< ctValidateEnd;

	ctValidate(end_read_large_block( this->sstream, this->active_subsection->end_position ), status::invalid )
		<< "end_read_large_block failed unexpectedly, the stream is corrupted or the reading is out of sync with the stream."
		<< ctValidateEnd;

	this->active_subsection.reset();
	this->active_subsection_array_size = 0;
	this->active_subsection_index = size_t( ~0 );
	this->active_subsection_end_pos = 0;
	return status::ok;
}

#include "_pds_undef_macros.inl"
