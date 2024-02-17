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


