cmake_minimum_required(VERSION 2.8)

# use folder name as project name
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

set(CMAKE_BUILE_TYPE DEBUG)
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -Wall" )      # Debug not optimize
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG " )     # release optimeze

set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -Wall" )    # Debug not optimize
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG " )   # release optimeze

#set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

message(STATUS "Project: ${ProjectId}")
message(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Source Directory: ${CMAKE_CURRENT_LIST_DIR}/src")
message(STATUS "Output Directory: ${CMAKE_CURRENT_BINARY_DIR}")

# source code
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src DIR_SRC) 

# include
include_directories(${CMAKE_CURRENT_LIST_DIR}/include /usr/local/net-snmp/include)

# library ${CMAKE_CURRENT_LIST_DIR}/lib
link_directories(/usr/local/net-snmp/lib)

# build 
message(STATUS "Source Files: ${DIR_SRC}")
add_executable(${PROJECT_NAME}.out ${DIR_SRC})
target_link_libraries(${PROJECT_NAME}.out -lnetsnmpmibs -lnetsnmpagent -lnetsnmp -lnetsnmpmibs -lnetsnmpagent -lnetsnmp -ldl -lm)