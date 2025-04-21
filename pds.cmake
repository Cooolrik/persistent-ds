# define the set of pds library files
set(
	pds_library_files

	# library files
	./Include/pds/value_types.h
	./Include/pds/value_types.inl

	./Include/pds/container_types.h
	
	./Include/pds/element_types.h
	./Include/pds/element_types.inl
	./Include/pds/element_value_ptrs.h

	./Include/pds/entity_ref.h		
	./Include/pds/fwd.h	
	
	./Include/pds/dynamic_types.h
	./Include/pds/dynamic_types.inl

	./Include/pds/fileops_common.h		

	#./Include/pds/EntityManager.h
	#./Include/pds/EntityManager.inl
	./Include/pds/EntityReader.h
	./Include/pds/EntityReader.inl
	./Include/pds/EntityValidator.h
	./Include/pds/EntityWriter.h
	./Include/pds/EntityWriter.inl
	./Include/pds/writer_templates.h
	./Include/pds/reader_templates.h

	./Include/pds/item_ref.h		
	./Include/pds/ReadStream.h
	./Include/pds/ReadStream.inl
	./Include/pds/WriteStream.h
	./Include/pds/WriteStream.inl
	./Include/pds/pds.h
	
	./Include/pds/BidirectionalMap.h
	./Include/pds/BidirectionalMap.inl
	./Include/pds/mf/BidirectionalMap_MF.h
	./Include/pds/IndexedVector.h
	./Include/pds/IndexedVector.inl
	./Include/pds/mf/IndexedVector_MF.h
	./Include/pds/DirectedGraph.h
	./Include/pds/DirectedGraph.inl
	./Include/pds/mf/DirectedGraph_MF.h
	./Include/pds/ItemTable.h
	./Include/pds/mf/ItemTable_MF.h

	./Include/pds/Varying.h
	./Include/pds/mf/Varying_MF.h
	./Include/pds/Varying.inl

	# compilation helper files
	./Include/pds/_pds_macros.inl
	./Include/pds/_pds_undef_macros.inl
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
		./Tests/HeaderLibraries.cpp 
		
		${pds_library_files}
		
		dependencies.cmake
		pds.cmake
		)	

	target_include_directories(	
		systemtest 
		PUBLIC ${PROJECT_SOURCE_DIR}/Include
		PUBLIC ${picosha2_SOURCE_DIR}
		PUBLIC ${ctle_SOURCE_DIR}/include
		PUBLIC ${PROJECT_SOURCE_DIR}/Tests 
		) 

	target_link_libraries( 	
		systemtest 
		)

	# Tests run unit tests and other specific tests
	add_executable( 
		tests

		${pds_library_files}

		./Tests/Tests.h 
		./Tests/Tests.cpp 
		./Tests/HeaderLibraries.cpp 
		./Tests/BidirectionalMapTests.cpp
		./Tests/DirectedGraphTests.cpp
		./Tests/DynamicTypesTests.cpp
		./Tests/EntityReaderRandomTests.cpp
		./Tests/EntityReadWriteTests.cpp
		#./Tests/EntityTests.cpp
		#./Tests/ItemTableTests.cpp
		./Tests/IndexedVectorTests.cpp
		./Tests/ReadWriteTests.cpp
		./Tests/SectionHierarchyReadWriteTests.cpp
		./Tests/TypeTests.cpp 
		./Tests/TestHelpers/random_vals.h
		./Tests/TestHelpers/random_vals.cpp 
		#./Tests/TestPackA/TestPackA.cpp 
		./Tests/VaryingTests.cpp
		
		dependencies.cmake
		pds.cmake
		)

	target_include_directories( 
		tests 
		PUBLIC ${PROJECT_SOURCE_DIR}/Include
		PUBLIC ${picosha2_SOURCE_DIR}
		PUBLIC ${ctle_SOURCE_DIR}/include
		PUBLIC ${PROJECT_SOURCE_DIR}/Tests	
		)

	target_link_libraries( 	
		tests 
		gtest 
		)

else() # BUILD_PERSISTENT_DS_TESTS

	message(NOTICE "Not building the persistent-ds tests. To enable, use -DBUILD_PERSISTENT_DS_TESTS=ON")

endif() # BUILD_PERSISTENT_DS_TESTS
