
project(rlbwt_iterator)
#SET(CMAKE_INSTALL_PREFIX "abc")
cmake_minimum_required(VERSION 3.2)

if(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64  -D_INT128 -Wall -Wextra")
else(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64  -D_INT128 -static -Wall -Wextra")
endif(APPLE)

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
include_directories(${MYMOD_SOURCE_DIR}/Basics)
include_directories(${MYMOD_SOURCE_DIR}/BTree)
include_directories(${MYMOD_SOURCE_DIR}/OnlineRlbwt)


FILE(GLOB SOURCES src/common/*.cpp)

#add_executable(compress.out src/compress_main.cpp ${SOURCES})
#target_link_libraries(compress.out online_rlbwt Basics BTree)

#add_executable(decompress.out src/decompress_main.cpp ${SOURCES})
#target_link_libraries(decompress.out online_rlbwt Basics BTree)

#add_executable(iterator.out src/iterator_main.cpp ${SOURCES})
#target_link_libraries(iterator.out online_rlbwt Basics BTree)

#add_executable(slcp.out src/slcp_main.cpp ${SOURCES})
#target_link_libraries(slcp.out online_rlbwt Basics BTree)

#add_executable(ms.out src/ms_main.cpp ${SOURCES})
#target_link_libraries(ms.out online_rlbwt Basics BTree)


FILE(GLOB INCLUDE_FILES src/include/*.hpp)
install (FILES ${INCLUDE_FILES} DESTINATION include/rlbwt_iterator)
install (FILES ${MYMOD_SOURCE_DIR}/OnlineRlbwt/online_rlbwt.hpp DESTINATION include/rlbwt_iterator)


#install (TARGETS rlbwt_iter DESTINATION bin)

