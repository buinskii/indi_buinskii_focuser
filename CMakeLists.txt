cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(indi_buinskii_focuser CXX C)
include(ExternalProject)
set(CMAKE_INSTALL_PREFIX "/usr")

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
set(MAKE make)

find_package(INDI 1.7.8 REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${INDI_INCLUDE_DIR})

set(indi_buinskii_focuser_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/indi_buinskii_focuser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/buinskii_focuser_serial_client.cpp
   )

add_executable(indi_buinskii_focuser ${indi_buinskii_focuser_SRCS})

target_link_libraries(indi_buinskii_focuser indidriver)

install(TARGETS indi_buinskii_focuser RUNTIME DESTINATION bin)
install(FILES indi_buinskii_focuser.xml DESTINATION ${INDI_DATA_DIR})
