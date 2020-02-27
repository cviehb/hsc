# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.5)

# Project's name
project(hello_systemc)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compile with c++11
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

include_directories("${PROJECT_SOURCE_DIR}/src/" "/usr/local/systemc-2.3.2/include/")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall")
LINK_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/" "/usr/local/systemc-2.3.2/lib")
add_executable(hello_systemc ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/src/thread.cpp
  ${PROJECT_SOURCE_DIR}/src/thread.h)
target_link_libraries(hello_systemc "systemc")
