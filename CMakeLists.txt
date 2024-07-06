cmake_minimum_required(VERSION 3.21)
project( persistent-ds )

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)	
	set( PDS_MAIN_PROJECT ON )
else()
	set( PDS_MAIN_PROJECT OFF )
endif()

option( BUILD_PERSISTENT_DS_TESTS "Build the persistent-ds tests" ${PDS_MAIN_PROJECT} )
			
include( dependencies.cmake )			

# macro which groups files by their file folder 
macro( source_group_by_folder rootPath projectFolder )
  file( REAL_PATH ${rootPath} rootAbs )
  foreach( file ${ARGN} )
	file( REAL_PATH ${file} fileAbs )
	file(RELATIVE_PATH relative_file ${rootAbs} ${fileAbs} )
	get_filename_component(dir "${relative_file}" DIRECTORY )
	source_group("${projectFolder}/${dir}" FILES ${file})
  endforeach()
endmacro()

# generate the generated pds code
execute_process( 
	COMMAND 			${PYTHON_EXECUTABLE} CodeGenerator.py ${ctle_SOURCE_DIR}
	WORKING_DIRECTORY 	${CMAKE_CURRENT_LIST_DIR}/CodeGen
	RESULT_VARIABLE 	py_result
)
message(STATUS "Result of CodeGenerator.py: ${py_result}")
	
include( pds.cmake )			

