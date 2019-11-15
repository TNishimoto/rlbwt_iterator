cmake_minimum_required(VERSION 2.8)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra")
endif(CPP_STATIC)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../build/lib)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../build/include)

add_executable(test.out test_main.cpp)
target_link_libraries(test.out online_rlbwt Basics BTree)

add_executable(sample.out sample_main.cpp)
target_link_libraries(sample.out online_rlbwt Basics BTree)

add_executable(slcp.out slcp_main.cpp)
target_link_libraries(slcp.out online_rlbwt Basics BTree)

