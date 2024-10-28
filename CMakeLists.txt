cmake_minimum_required(VERSION 3.26)
project(libcurl_cpp VERSION 0.1 LANGUAGES CXX C)

find_package(CURL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)


set(CMAKE_C_COMPILER "gcc" CACHE STRING "C compiler" FORCE)
set(CNAKE_CXX_COMPILE "g++" CACHE STRING "C++ compiler" FORCE)

if(DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER}" CACHE STRING "C compiler" FORCE)
endif()
if(DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}" CACHE STRING "C++ compiler" FORCE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    link_libraries(stdc++)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(BIN_OUT ${CMAKE_SOURCE_DIR}/bin/x64/${CMAKE_BUILD_TYPE})
        set(LIB_OUT ${CMAKE_SOURCE_DIR}/lib64/)
    else()
        set(BIN_OUT ${CMAKE_SOURCE_DIR}/bin/x86/${CMAKE_BUILD_TYPE})
        set(LIB_OUT ${CMAKE_SOURCE_DIR}/lib/)
    endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${LIBXML2_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_OUT})

set(LIB_DIR_SHARED ${LIB_OUT}/shared)
set(LIB_DIR_STATIC ${LIB_OUT}/static)

file(GLOB SOURCES "./src/*.cpp")

add_library(curlc++ SHARED ${SOURCES})
target_link_libraries(curlc++ CURL::libcurl)
# target_include_directories(curlc++ PUBLIC ${CMAKE_SOURCE_DIR}/include)
set_target_properties(curlc++ PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR_SHARED})

file(GLOB CRAWLER ${CMAKE_SOURCE_DIR}/test/crawler.cpp)

add_executable(crawler ${CRAWLER})

target_link_libraries(crawler curlc++ ${LIBXML2_LIBRARIES})
