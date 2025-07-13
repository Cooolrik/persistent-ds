@echo off

if [%~1] == [] goto mainbuildfile

:: build the test code
echo Building and testing C++ version %2
mkdir %1
cd %1
cmake .. -DCMAKE_CXX_STANDARD=%2 -DBUILD_PERSISTENT_DS_TESTS=ON
cmake --build .
Debug\tests > ..\%3
cd ..

goto:eof

:mainbuildfile

:: build the main code
mkdir build
cd build 
cmake ..
cd ..

:: build test code for each c++ version
start "" buildall build_c++14 14 results_c++14
start "" buildall build_c++17 17 results_c++17
start "" buildall build_c++20 20 results_c++20

goto:eof
