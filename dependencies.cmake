
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
		GIT_TAG		   main # (latest)
	)

	# picosha2 
	FetchContent_Declare( 
		picosha2
		GIT_REPOSITORY https://github.com/okdshin/PicoSHA2.git
		GIT_TAG		   27fcf6979298949e8a462e16d09a0351c18fcaf2 # (2022 Aug 08)
	)
	
	# googletest
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY 	https://github.com/google/googletest.git
		GIT_TAG			58d77fa8070e8cec2dc1ed015d66b454c8d78850 # 1.12.1 
		)

	FetchContent_MakeAvailable( 
		ctle 
		picosha2 
		googletest 
		)

endif() #BUILD_PERSISTENT_DS_TESTS

