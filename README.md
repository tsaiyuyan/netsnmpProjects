# netsnmpProjects

There are my examples ,by using net-snmp library. (ref: www.net-snmp.org) (ref: www.net-snmp.org)

## Directory structure

```sh
$ tree -d
.
├── arm-build                   (建置arm程式)
├── build                       (建置PC程式)
├── include                     (header files)
├── lib                         (library files)
├── ref                         (參考範例)
├── src                         (source code files)
├── toolchain.cmake             (cross-compiler configure)
├── CMakeLists.txt              (master cmake file)
└── .vscode
    ├── c_cpp_properties.json   (Compiler confugure)
    ├── launch.json             (Debug configure)
    └── settings.json
```

## How to build

build arm or normal application:

```sh
# build arm app
$mkdir arm-build
$cd arm-build
$cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
$make
# or
# build normal app
$mkdir build
$cd build
$cmake ..
$make
```
