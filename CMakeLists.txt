
project(rlbwt_iterator)
#SET(CMAKE_INSTALL_PREFIX "abc")

if(CMAKE_OLD)
cmake_minimum_required(VERSION 2.8)
else(CMAKE_OLD)
cmake_minimum_required(VERSION 3.1)
endif(CMAKE_OLD)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++11 -m64 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++11 -m64 -Wall -Wextra")
endif(CPP_STATIC)

#set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")


#INCLUDE_DIRECTORIES(src)
#INCLUDE_DIRECTORIES(src/common)

# Set PROJECT_IS_TOP_LEVEL to ON if if this is the top level project (not
# if this is included as a subproject of something else). Note that this is
# handled automatically for CMake >= 3.21.
if (CMAKE_VERSION VERSION_LESS 3.21)
    if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
        message("SET PROJECT_IS_TOP_LEVEL ON IN ${PROJECT_NAME}")
        set (PROJECT_IS_TOP_LEVEL ON)
    else()
        message("SET PROJECT_IS_TOP_LEVEL OFF IN ${PROJECT_NAME}")
    set (PROJECT_IS_TOP_LEVEL OFF)    
    endif()
endif ()

if(NOT TARGET OnlineRlbwt)
add_subdirectory(module/OnlineRlbwt)
endif()
include_directories(module)
include_directories(module/OnlineRlbwt/module)




#### Modules
#if(NOT MYMOD_DIR)
#set(MYMOD_DIR 1)
#set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
#  set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)
#  add_subdirectory(${MYMOD_SOURCE_DIR}/Basics ${MYMOD_BINARY_DIR}/Basics)
#  add_subdirectory(${MYMOD_SOURCE_DIR}/BTree ${MYMOD_BINARY_DIR}/BTree)
#  add_subdirectory(${MYMOD_SOURCE_DIR}/OnlineRlbwt ${MYMOD_BINARY_DIR}/OnlineRlbwt)
#
#endif()
#include_directories(${MYMOD_SOURCE_DIR})
#include_directories(${MYMOD_SOURCE_DIR}/Basics)
#include_directories(${MYMOD_SOURCE_DIR}/BTree)
#include_directories(${MYMOD_SOURCE_DIR}/OnlineRlbwt)

#SDSL lib and include directory are here
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
include_directories(~/include)

if (PROJECT_IS_TOP_LEVEL)
FILE(GLOB SOURCES src/common/*.cpp src/include/*.cpp)
FILE(GLOB STOOL_SOURCES module/stool/src/*.cpp)

add_library(fn STATIC ${SOURCES} ${STOOL_SOURCES})
target_link_libraries(fn online_rlbwt Basics BTree sdsl)


add_executable(compress.out src/main/compress_main.cpp)
target_link_libraries(compress.out fn)


add_executable(time_lcp.out src/debug/time_lcp_main.cpp)
target_link_libraries(time_lcp.out fn)

add_executable(sampling_sa.out src/debug/sampling_sa_main.cpp)
target_link_libraries(sampling_sa.out fn)

add_executable(weiner.out src/debug/weiner_main.cpp)
target_link_libraries(weiner.out fn)

FILE(GLOB INCLUDE_FILES src/include/*.hpp)
install (FILES ${INCLUDE_FILES} DESTINATION include/rlbwt_iterator)
install (FILES ${MYMOD_SOURCE_DIR}/OnlineRlbwt/online_rlbwt.hpp DESTINATION include/rlbwt_iterator)

endif()



##add_executable(slcp.out src/debug/slcp_main.cpp)
##target_link_libraries(slcp.out fn)


##add_executable(check.out src/debug/check_main.cpp)
##target_link_libraries(check.out fn)

##add_executable(time.out src/debug/time_main.cpp)
##target_link_libraries(time.out fn)

##add_executable(data_viewer.out src/debug/data_viewer_main.cpp)
##target_link_libraries(data_viewer.out fn)

##add_executable(elias_fano_test.out src/debug/elias_fano_test_main.cpp)
##target_link_libraries(elias_fano_test.out fn)

#add_executable(cartesian_tree_test.out src/debug/cartesian_tree_test_main.cpp)
#target_link_libraries(cartesian_tree_test.out fn)

##add_executable(lcp_test.out src/debug/lcp_test_main.cpp)
##target_link_libraries(lcp_test.out fn)

##add_executable(vfa.out src/debug/variable_fixed_array_test_main.cpp)
##target_link_libraries(vfa.out fn)


##add_executable(interval_test.out src/debug/interval_tree_main.cpp)
##target_link_libraries(interval_test.out fn)

#add_executable(decompress.out src/decompress_main.cpp ${SOURCES})
#target_link_libraries(decompress.out online_rlbwt Basics BTree)

#add_executable(iterator.out src/iterator_main.cpp ${SOURCES})
#target_link_libraries(iterator.out online_rlbwt Basics BTree)


#add_executable(slcp.out src/debug/slcp_main.cpp ${INCLUDE_FILES})
#target_link_libraries(slcp.out online_rlbwt Basics BTree)

#add_executable(test.out src/debug/test_main.cpp ${INCLUDE_FILES})
#target_link_libraries(test.out online_rlbwt Basics BTree)




#install (TARGETS rlbwt_iter DESTINATION bin)

