# netsnmpProjects

There are my examples ,by using net-snmp library. (ref: www.net-snmp.org) (ref: www.net-snmp.org)

build arm or normal application:

```sh
# build arm app
$cd arm-build
$cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
$make
# or
# build normal app
$cd build
$cmake ..
$make
```