ESP32 already has a "Virtual File System" (VFS) that interferes with my code.

To make my code work, first I disabled VFS in the "ESP-IDF SDK Configuration Editor" (menuconfig).
That was not enough, because there is a linker-script (esp-idf\components\esp_rom\esp32\ld\esp32.rom.syscalls.ld)
that provides fixed addresses for "close", "open", "read" and "write". It seems that these functions
are located in an internal ROM and are not part of the application.

I just commented the code in the linker-script:

/*
close = 0x40001778;
open = 0x4000178c;
read = 0x400017dc;
write = 0x4000181c;
*/
sbrk = 0x400017f4;
times = 0x40001808;


"sbrk" and "times" were intermixed with the others, I just moved them to the bottom and added the /* */.


The files in the directory "NewLib" provide the correct implementation for "read"/"_read_r", "write"/"_write_r", etc.





# _Sample project_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This is the simplest buildable example. The example is used by command `idf.py create-project`
that copies the project to user specified path and set it's name. For more information follow the [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project)



## How to use example
We encourage the users to use the example as a template for the new projects.
A recommended way is to follow the instructions on a [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project).

## Example folder contents

The project **sample_project** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
