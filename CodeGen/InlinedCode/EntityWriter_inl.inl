#include "_pds_macros.inl"

EntityWriter::EntityWriter(WriteStream& _dstream)
	: dstream(_dstream)
	, start_position(_dstream.GetPosition())
{
}

EntityWriter::~EntityWriter() {}

// Build a section. 
status_return<EntityWriter*> EntityWriter::BeginWriteSection( const char *key, const u8 key_length )
{
	ctValidate(!this->active_subsection, status::invalid)
		<< "This reader already has an active subsection. Close the subsection before opening a new."
		<< ctValidateEnd;

	// create a writer for the array, to store the start position before calling the begin large block 
	this->active_subsection = std::unique_ptr<EntityWriter>( new EntityWriter( this->dstream ) );
	ctStatusCall(begin_write_large_block(this->dstream, serialization_type_index::vt_subsection, key, key_length));
	return this->active_subsection.get();
}

status EntityWriter::EndWriteSection( const EntityWriter *section_writer )
{
	ctValidate(section_writer == this->active_subsection.get(), status::invalid)
		<< "Invalid parameter section_writer, it does not match the internal expected value."
		<< ctValidateEnd;

	ctStatusCall(end_write_large_block(this->dstream, this->active_subsection->start_position));
	this->active_subsection.reset();
	return status::ok;
}

status EntityWriter::WriteNullSection( const char *key, const u8 key_length )
{
	ctStatusAutoReturnCall( subsection, this->BeginWriteSection(key, key_length) );
	return this->EndWriteSection( subsection );
}

status_return<EntityWriter*> EntityWriter::BeginWriteSectionsArray( const char *key, const u8 key_length, const size_t array_size, const std::vector<i32> *index )
{
	ctValidate(!this->active_subsection, status::invalid)
		<< "This reader already has an active subsection. Close the subsection before opening a new."
		<< ctValidateEnd;

	// create a writer for the array, to store the start position before calling the begin large block 
	this->active_subsection = std::unique_ptr<EntityWriter>(new EntityWriter(this->dstream));
	
	ctStatusCall(begin_write_large_block(this->dstream, serialization_type_index::vt_array_subsection, key, key_length));

	// if array_size is ~0, the array is null, so end directly
	if( array_size == (size_t)~0 )
	{
		this->active_array_size = 0;
		return this->active_subsection.get();
	}

	// write out flags, index and array size
	ctStatusCall(write_array_metadata_and_index(dstream, 0, array_size, index));

	// reset the size and write index in the array
	this->active_array_size = array_size;
	this->active_array_index = size_t( ~0 );
	this->active_array_index_start_position = 0;
	return this->active_subsection.get();
}

status EntityWriter::BeginWriteSectionInArray( const EntityWriter *sections_array_writer, const size_t section_index )
{
	ctValidate(sections_array_writer == this->active_subsection.get(), status::invalid)
		<< "Invalid parameter section_writer, it does not match the internal expected value."
		<< ctValidateEnd;

	ctValidate((this->active_array_index + 1) == section_index, status::invalid_param)
		<< "Synch error, incorrect subsection index: " << section_index
		<< " expected: " << (this->active_array_index + 1)
		<< ctValidateEnd;

	ctValidate(section_index < this->active_array_size, status::invalid_param)
		<< "Invalid subsection index: " << section_index
		<< " total array size: " << this->active_array_size
		<< ctValidateEnd;

	this->active_array_index = section_index;
	this->active_array_index_start_position = this->dstream.GetPosition();

	// write a temporary subsection size (INT64_MAX on purpose, to provoke an error if we do not update it)
	dstream.Write( (u64)INT64_MAX );
	ctSanityCheck(dstream.GetPosition() == (this->active_array_index_start_position + sizeof(u64)));
	return status::ok;
}

status EntityWriter::EndWriteSectionInArray( const EntityWriter *sections_array_writer, const size_t section_index )
{
	ctValidate(sections_array_writer == this->active_subsection.get(), status::invalid)
		<< "Invalid parameter section_writer, it does not match the internal expected value."
		<< ctValidateEnd;

	ctValidate(this->active_array_index == section_index, status::invalid_param)
		<< "Synch error, incorrect subsection index: " << section_index
		<< " expected: " << this->active_array_index
		<< ctValidateEnd;	

	const u64 end_pos = dstream.GetPosition();
	const u64 block_size = end_pos - this->active_array_index_start_position - sizeof( u64 ); // total block size - ( sizeof( section_size_value )=8 )
	dstream.SetPosition( this->active_array_index_start_position );
	dstream.Write( block_size );
	dstream.SetPosition( end_pos ); // move back the where we were
	ctSanityCheck(end_pos > this->active_array_index_start_position);
	return status::ok;
}

status EntityWriter::EndWriteSectionsArray( const EntityWriter *sections_array_writer )
{
	ctValidate(sections_array_writer == this->active_subsection.get(), status::invalid)
		<< "Invalid parameter section_writer, it does not match the internal expected value."
		<< ctValidateEnd;

	ctValidate((this->active_array_index + 1) == this->active_array_size, status::invalid_param)
		<< "Synch error, the subsection index does not equal the end of the array. Total sections read count:" << (this->active_array_index + 1)
		<< " expected: " << this->active_array_size
		<< ctValidateEnd;

	ctValidate(end_write_large_block( this->dstream, this->active_subsection->start_position ), status::invalid )
		<< "end_write_large_block failed unexpectedly, the stream is corrupted or the reading is out of sync with the stream."
		<< ctValidateEnd;

	// release active subsection writer
	this->active_subsection.reset();
	this->active_array_size = 0;
	this->active_array_index = size_t( ~0 );
	this->active_array_index_start_position = 0;
	return status::ok;
}

status EntityWriter::WriteNullSectionsArray( const char *key, const u8 key_length )
{
	ctStatusAutoReturnCall( subsection, this->BeginWriteSectionsArray( key, key_length, size_t( ~0 ), nullptr ) );
	return this->EndWriteSectionsArray( subsection );
}

#include "_pds_undef_macros.inl"
