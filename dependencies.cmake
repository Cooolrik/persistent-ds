
find_package(PythonInterp 3.11 REQUIRED)
find_package(Python 3.11 REQUIRED)

# if we are to build the tests, fetch the needed libraries
if(BUILD_PERSISTENT_DS_TESTS)			
				
	message(STATUS "Fetching needed libraries for building the persistent-ds tests...")

	include(FetchContent)

	# ctle 
	FetchContent_Declare(
		ctle
		GIT_REPOSITORY https://github.com/Cooolrik/ctle.git
		GIT_TAG		   smallfixes # (latest)
	)

	# googletest
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY 	https://github.com/google/googletest.git
		GIT_TAG			58d77fa8070e8cec2dc1ed015d66b454c8d78850 # 1.12.1 
		)

	FetchContent_MakeAvailable( 
		ctle 
		googletest 
		)

endif() #BUILD_PERSISTENT_DS_TESTS
 