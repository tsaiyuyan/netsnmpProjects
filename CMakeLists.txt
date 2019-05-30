cmake_minimum_required(VERSION 2.8)

# set project name
project(snmpProjects)

# set build type
set(CMAKE_BUILD_TYPE Debug)

# get binary folder name
get_filename_component(BFolderName ${CMAKE_CURRENT_BINARY_DIR} NAME)
string(REPLACE " " "_" BFolderName ${BFolderName})
message(STATUS "Current Binary Directory Name: ${BFolderName}")

# add projects
if(NOT BFolderName STREQUAL "arm-build")
    add_subdirectory(snmp-command)
endif()

add_subdirectory(snmp-subagent)
