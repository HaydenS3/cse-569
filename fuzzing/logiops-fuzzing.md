# Logiops Fuzzing

Author: Hayden Schroeder

## Finding a target

### Searching for unsafe functions

`grep -Pr --include="*.cpp" '<unsafe function>' .`

Unfortunately, no unsafe functions were found. Unsafe functions that were tried include:
- strcpy
- strcat
- sprintf
- vsprintf
- gets
- makepath
- _splitpath
- scanf
- snscanf
- strlen

### Fuzzing the input

The program enters at `logid.cpp`. The program immediately loads the config file using the `Configuration.cpp` constructor. This constructor calls [`_config.readfile`](https://github.com/hyperrealm/libconfig/blob/f9404f60a435aa06321f4ccd8357364dcb216d46/lib/libconfigcpp.c%2B%2B#L516) to parse the file which is a function in the [libconfig](https://github.com/hyperrealm/libconfig/tree/master) library. This library is used to parse the config file. The `readfile` function is a wrapper for [`config_read_file`](https://github.com/hyperrealm/libconfig/blob/f9404f60a435aa06321f4ccd8357364dcb216d46/lib/libconfig.c#L627) which is a wrapper for [`__config_read`](https://github.com/hyperrealm/libconfig/blob/f9404f60a435aa06321f4ccd8357364dcb216d46/lib/libconfig.c#L512).

Fuzzing `__config_read` sounds difficult. I'm going to try fuzzing the `readfile` function to start.

I've written `/src/logid/harness.cpp` just to see if I can compile the project through it rather than `logid.cpp`. I've made changes to the logid `CMakeLists.txt` file for this.

make is throwing this stinker:
```
[100%] Linking CXX executable ../../harness
/usr/bin/ld: CMakeFiles/harness.dir/backend/raw/RawDevice.cpp.o: warning: relocation against `_ZN5logid15global_loglevelE' in read-only section `.text'
/usr/bin/ld: CMakeFiles/harness.dir/util/log.cpp.o: in function `logid::logPrintf(logid::LogLevel, char const*, ...)':
log.cpp:(.text+0x72): undefined reference to `logid::global_loglevel'
/usr/bin/ld: CMakeFiles/harness.dir/features/RemapButton.cpp.o: in function `logid::features::RemapButton::RemapButton(logid::Device*)':
RemapButton.cpp:(.text+0x5430): undefined reference to `logid::global_loglevel'
/usr/bin/ld: CMakeFiles/harness.dir/backend/raw/RawDevice.cpp.o: in function `logid::backend::raw::RawDevice::_readReports()':
RawDevice.cpp:(.text+0x112c): undefined reference to `logid::global_loglevel'
/usr/bin/ld: RawDevice.cpp:(.text+0x116a): undefined reference to `logid::global_loglevel'
/usr/bin/ld: CMakeFiles/harness.dir/backend/raw/RawDevice.cpp.o: in function `logid::backend::raw::RawDevice::sendReport(std::vector<unsigned char, std::allocator<unsigned char> > const&)':
RawDevice.cpp:(.text+0x1e6d): undefined reference to `logid::global_loglevel'
/usr/bin/ld: warning: creating DT_TEXTREL in a PIE
collect2: error: ld returned 1 exit status
make[2]: *** [src/logid/CMakeFiles/harness.dir/build.make:1010: harness] Error 1
make[1]: *** [CMakeFiles/Makefile2:170: src/logid/CMakeFiles/harness.dir/all] Error 2
make: *** [Makefile:136: all] Error 2
```

I fixed it with some changes to harness.cpp!

Going to try fuzzing with AFL++ in LLVM mode. Got everything built and ready to go!

Use [dictionary](https://github.com/AFLplusplus/AFLplusplus/blob/stable/dictionaries/README.md) to better refine fuzzing?

Compile binary with AFL fuzzer:
```
CC=/home/hayden/applications/AFLplusplus/afl-clang-fast CXX=/home/hayden/applications/AFLplusplus/afl-clang-fast++ cmake ..
make
```

Create and mount ramdisk

Run AFL: `~/applications/AFLplusplus/afl-fuzz -i ../logiops-fuzzing/seeds/logid.example.cfg -o output/ -m 2000 -- harness @@`. This throws an error for some reason.