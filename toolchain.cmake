# this is required
set(CMAKE_SYSTEM_NAME Linux)
#this one not so much
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR armv7l)

set(TOOLCHAIN_HOME /opt/toolchains/ti335x/i686-arago-linux/usr/bin)
set(HOST arm-linux-gnueabihf)
# specify the cross compiler
set(CMAKE_C_COMPILER   ${TOOLCHAIN_HOME}/${HOST}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_HOME}/${HOST}-g++)

# where is the target environment 
set(CMAKE_FIND_ROOT_PATH /opt/toolchains/ti335x/cortexa8t2hf-vfp-neon-linux-gnueabi/)

# search for programs in the build host directories (not necessary)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)

# configure Boost and Qt
#SET(QT_QMAKE_EXECUTABLE /home/yuyan/Qt5.10.0/5.10.0/gcc_64/bin/qmake)
#SET(BOOST_ROOT /opt/boost_arm)

# include
include_directories(/opt/toolchains/ti335x/cortexa8t2hf-vfp-neon-linux-gnueabi/usr/include)
include_directories(/opt/net-snmp-5.7.3-arm/include)

# library
#link_directories(/opt/net-snmp-5.7.3-arm/lib)
# Forced to link arm-net-snmp library, because i can't normally link it by using link_directories function.
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/net-snmp-5.7.3-arm/lib")
