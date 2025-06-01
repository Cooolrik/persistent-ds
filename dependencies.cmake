
find_package(PythonInterp 3.11 REQUIRED)
find_package(Python 3.11 REQUIRED)

# if we are to build the tests, fetch the needed libraries
if(BUILD_PERSISTENT_DS_TESTS)			
				
	message(STATUS "Fetching needed libraries for building the persistent-ds tests...")

	include(FetchContent)

	# ctle - core dependency of pds
	FetchContent_Declare(
		ctle
		GIT_REPOSITORY https://github.com/Cooolrik/ctle.git
		GIT_TAG		   smallfixes # (latest)
	)
	
	# xxHash - hashing implementation used by testing
	FetchContent_Declare( 
		xxhash
		GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
		GIT_TAG		   e626a72bc2321cd320e953a0ccf1584cad60f363 # v0.8.3
	)

	# googletest - testing dependency
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY 	https://github.com/google/googletest.git
		GIT_TAG			58d77fa8070e8cec2dc1ed015d66b454c8d78850 # 1.12.1 
		)

	FetchContent_MakeAvailable( 
		ctle 
		xxhash
		googletest
		)

endif() #BUILD_PERSISTENT_DS_TESTS
