cmake_minimum_required(VERSION 2.8)

# use folder name as project name
project(snmpProjects)

set(CMAKE_BUILD_TYPE Debug)

get_filename_component(BFolderName ${CMAKE_CURRENT_BINARY_DIR} NAME)
string(REPLACE " " "_" BFolderName ${BFolderName})


message(STATUS "Current Binary Directory Name: ${BFolderName}")

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
if(NOT BFolderName STREQUAL "arm-build")
    add_subdirectory(snmp-command)
endif()

add_subdirectory(snmp-subagent)
