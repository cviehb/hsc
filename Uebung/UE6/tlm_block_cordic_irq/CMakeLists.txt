# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.5)

# Project's name
project(tlm_block_cordic_irq)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compile with c++11
#macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 14)
  endif ()
#endmacro(use_cxx11)

include_directories("${PROJECT_SOURCE_DIR}" "/usr/local/systemc-2.3.2/include/")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall")
LINK_DIRECTORIES("${PROJECT_SOURCE_DIR}" "/usr/local/systemc-2.3.2/lib")

add_executable(tlm_block_cordic_irq
  ${PROJECT_SOURCE_DIR}/main.cpp
  ${PROJECT_SOURCE_DIR}/cordicModule.cpp
  ${PROJECT_SOURCE_DIR}/cordicModule.h
  ${PROJECT_SOURCE_DIR}/cc_cordicModule.cpp
  ${PROJECT_SOURCE_DIR}/cc_cordicModule.h
  ${PROJECT_SOURCE_DIR}/cc_cordicModule_fixed.cpp
  ${PROJECT_SOURCE_DIR}/cc_cordicModule_fixed.h
  ${PROJECT_SOURCE_DIR}/emu_cpu.h
  ${PROJECT_SOURCE_DIR}/slave_cordic.h
  ${PROJECT_SOURCE_DIR}/bus_spec.h
  ${PROJECT_SOURCE_DIR}/xparameter.h
  ${PROJECT_SOURCE_DIR}/xscugic.h
  ${PROJECT_SOURCE_DIR}/xcordic_sinit.h
  ${PROJECT_SOURCE_DIR}/cordic_driver.h
  ${PROJECT_SOURCE_DIR}/main_c.h
  ${PROJECT_SOURCE_DIR}/xscugic.c
  ${PROJECT_SOURCE_DIR}/xcordic_sinit.c
  )

# add_executable(firmeware_cordic
#   ${PROJECT_SOURCE_DIR}/main_c.c
#   ${PROJECT_SOURCE_DIR}/cordic_driver.c
#   ${PROJECT_SOURCE_DIR}/hal_cordic.h
#   ${PROJECT_SOURCE_DIR}/bus_spec.h
#   )

target_link_libraries(tlm_block_cordic_irq "systemc")
