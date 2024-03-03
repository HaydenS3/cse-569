# Fuzzing Logiops Userspace Driver

Author: Hayden Schroeder

## Finding a target

### Searching for unsafe functions

First, I tried to find unsafe functions in the logiops project. I used the following command to search for unsafe functions in the project:

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

Fuzzing `__config_read` sounds difficult, so I'm going to try fuzzing the `readfile` function to start.

## Fuzzing the `readfile` function

### Compiling the project

I began by trying to compile the project through my own `harness.cpp` file. The harness.cpp file acts as a wrapper for the `readfile` function. I made changes to the logid `CMakeLists.txt` file to compile the project through `harness.cpp`.

The following is the finalized harness.cpp file:

```
#include <stdio.h>
#include <Configuration.h>
#include <util/log.h>
#include <libconfig.h++>

using namespace logid;

LogLevel logid::global_loglevel = INFO;
libconfig::Config _config;

int main(int argc, char *argv[])
{
    // Read file name from command line
    if (argc != 2)
    {   
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    try
    {
        _config.readFile(argv[1]); // Fuzzing point
    }
    catch (const libconfig::ParseException &e)
    {
        return 0; // Catching the exception and returning 0
    }

    return 0;
}
```

I encountered quite a few errors at this step. Most of which were fixed by adding the correct includes to `harness.cpp` and installing the necessary packages for compiling the project.

### Instrumenting the target

I built AFL++ locally and enabled LLVM mode. I then instrumented the target with the following command:

```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/home/hayden/applications/AFLplusplus/afl-clang-fast -DCMAKE_CXX_COMPILER=/home/hayden/applications/AFLplusplus/afl-clang-fast++ ..
make
```

Once again, I ran into a few errors, but I was able to solve the errors by instaling the necessary packages.

### Seed files

I used the default configuration file provided within the logiops project as a seed file. I also did some google dorking, `logiops config file site:github.com`, to find about a dozen other configuration files to use as seed files.

### Fuzzing the target

I used AFL++ to fuzz the target.

`~/applications/AFLplusplus/afl-fuzz -i ~/repos/cse-569/fuzzing/logiops-fuzzing/seeds -o output/ -m 2000 -- ~/repos/cse-569/fuzzing/logiops-fuzzing/build/harness @@`