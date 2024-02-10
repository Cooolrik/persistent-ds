# define the set of pds library files
set(
	pds_library_files

	./Include/pds/BidirectionalMap.h
	./Include/pds/DataTypes.h
	./Include/pds/DataTypes.inl
	./Include/pds/DataValuePointers.h
	./Include/pds/DirectedGraph.h
	./Include/pds/DynamicTypes.h
	./Include/pds/DynamicTypes.inl
	./Include/pds/EntityReader.h
	./Include/pds/EntityReader.inl
	./Include/pds/EntityReaderTemplates.inl
	./Include/pds/EntityValidator.h
	./Include/pds/EntityWriter.h
	./Include/pds/EntityWriter.inl
	./Include/pds/EntityWriterTemplates.inl
	./Include/pds/IndexedVector.h
	./Include/pds/ItemTable.h
	./Include/pds/MemoryReadStream.h
	./Include/pds/MemoryWriteStream.h
	./Include/pds/pds.h
	./Include/pds/pds.inl
	./Include/pds/SHA256.h
	./Include/pds/SHA256.inl
	./Include/pds/ValueTypes.h
	./Include/pds/ValueTypes.inl
	./Include/pds/Varying.h
	./Include/pds/Varying.inl
)

source_group_by_folder( ./Include/pds pds ${pds_library_files} )

# if we are to build the tests of pds
if(BUILD_PERSISTENT_DS_TESTS)			
				
	message(STATUS "Building the persistent-ds tests...")

	# generate the pds test code
	execute_process( 
		COMMAND 			${PYTHON_EXECUTABLE} GenerateTestPacks.py
		WORKING_DIRECTORY 	${CMAKE_CURRENT_LIST_DIR}/CodeGen
		RESULT_VARIABLE 	py_result
	)
	message(STATUS "Result of GenerateTestPacks.py: ${py_result}")
	
	set (CMAKE_CXX_STANDARD 14)

	# lots of warnings, big object files
	if(MSVC)
		add_compile_options(/W4 /bigobj)
	else()
		add_compile_options(-Wall -Wextra -pedantic ) 
	endif()
	set( CMAKE_DEBUG_POSTFIX _d )

	# SystemTest run full system tests
	add_executable( 
		systemtest
		./Tests/SystemTest.cpp 
		./Tests/TestPackA/TestPackA.cpp 

		${pds_library_files}
		)	

	target_include_directories(	
		systemtest 
		PUBLIC ${PROJECT_SOURCE_DIR}/Include
		PUBLIC ${glm_SOURCE_DIR}
		PUBLIC ${picosha2_SOURCE_DIR}
		PUBLIC ${ctle_SOURCE_DIR}
		PUBLIC ${PROJECT_SOURCE_DIR}/Tests 
		) 

	target_link_libraries( 	
		systemtest 
		)

	# Tests run unit tests and other specific tests
	add_executable( 
		tests

		${pds_library_files}

		./Tests/Tests.cpp 
		./Tests/DirectedGraphTests.cpp
		./Tests/DynamicTypesTests.cpp
		./Tests/EntityReaderRandomTests.cpp
		./Tests/EntityReadWriteTests.cpp
		./Tests/EntityTests.cpp
		./Tests/ItemTableTests.cpp
		./Tests/ReadWriteTests.cpp
		./Tests/SectionHierarchyReadWriteTests.cpp
		./Tests/TypeTests.cpp 
		./Tests/TestHelpers/random_vals.cpp 
		./Tests/TestPackA/TestPackA.cpp 
		)

	target_include_directories( 
		tests 
		PUBLIC ${PROJECT_SOURCE_DIR}/Include
		PUBLIC ${glm_SOURCE_DIR}
		PUBLIC ${picosha2_SOURCE_DIR}
		PUBLIC ${ctle_SOURCE_DIR}
		PUBLIC ${PROJECT_SOURCE_DIR}/Tests	
		)

	target_link_libraries( 	
		tests 
		gtest 
		)

else() # BUILD_PERSISTENT_DS_TESTS

	message(NOTICE "Not building the persistent-ds tests. To enable, use -DBUILD_PERSISTENT_DS_TESTS=ON")

endif() # BUILD_PERSISTENT_DS_TESTS
