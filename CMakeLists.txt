
project(rlbwt_iterator)
#SET(CMAKE_INSTALL_PREFIX "abc")

if(CMAKE_OLD)
cmake_minimum_required(VERSION 2.8)
else(CMAKE_OLD)
cmake_minimum_required(VERSION 3.1)
endif(CMAKE_OLD)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra")
endif(CPP_STATIC)

#set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")


#INCLUDE_DIRECTORIES(src)
#INCLUDE_DIRECTORIES(src/common)

#### Modules
if(NOT MYMOD_DIR)
set(MYMOD_DIR 1)
set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
  set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)
  add_subdirectory(${MYMOD_SOURCE_DIR}/Basics ${MYMOD_BINARY_DIR}/Basics)
  add_subdirectory(${MYMOD_SOURCE_DIR}/BTree ${MYMOD_BINARY_DIR}/BTree)
  add_subdirectory(${MYMOD_SOURCE_DIR}/OnlineRlbwt ${MYMOD_BINARY_DIR}/OnlineRlbwt)

endif()
include_directories(${MYMOD_SOURCE_DIR})
#include_directories(${MYMOD_SOURCE_DIR}/Basics)
#include_directories(${MYMOD_SOURCE_DIR}/BTree)
#include_directories(${MYMOD_SOURCE_DIR}/OnlineRlbwt)

#SDSL lib and include directory are here
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
include_directories(~/include)


FILE(GLOB SOURCES src/common/*.cpp)
FILE(GLOB STOOL_SOURCES module/stool/src/*.cpp)


include_directories(module)
add_executable(compress.out ${STOOL_SOURCES} src/main/compress_main.cpp ${SOURCES})
target_link_libraries(compress.out online_rlbwt Basics BTree sdsl)

#add_executable(decompress.out src/decompress_main.cpp ${SOURCES})
#target_link_libraries(decompress.out online_rlbwt Basics BTree)

#add_executable(iterator.out src/iterator_main.cpp ${SOURCES})
#target_link_libraries(iterator.out online_rlbwt Basics BTree)

#add_executable(slcp.out src/slcp_main.cpp ${SOURCES})
#target_link_libraries(slcp.out online_rlbwt Basics BTree)

#add_executable(slcp.out src/debug/slcp_main.cpp ${INCLUDE_FILES})
#target_link_libraries(slcp.out online_rlbwt Basics BTree)

#add_executable(test.out src/debug/test_main.cpp ${INCLUDE_FILES})
#target_link_libraries(test.out online_rlbwt Basics BTree)

add_executable(check.out ${STOOL_SOURCES} src/debug/check_main.cpp)
target_link_libraries(check.out online_rlbwt Basics BTree sdsl)


add_executable(data_viewer.out ${STOOL_SOURCES} src/debug/data_viewer_main.cpp)
target_link_libraries(data_viewer.out online_rlbwt Basics BTree sdsl)


FILE(GLOB INCLUDE_FILES src/include/*.hpp)
install (FILES ${INCLUDE_FILES} DESTINATION include/rlbwt_iterator)
install (FILES ${MYMOD_SOURCE_DIR}/OnlineRlbwt/online_rlbwt.hpp DESTINATION include/rlbwt_iterator)


#install (TARGETS rlbwt_iter DESTINATION bin)

