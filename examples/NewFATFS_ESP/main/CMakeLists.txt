idf_component_register(SRCS "main.c" "NewFATFS/ffs.c" "NewFATFS/ffunicode.c"
                    "NewFATFS/ffs_syscalls.c" "NewFATFS/ffs_diskio.c" "RAMDisk/ram_diskio.c"
                    "NewLib/closer.c" "NewLib/fstatr.c" "NewLib/lseek64r.c" "NewLib/lseekr.c"
                    "NewLib/mkdirr.c" "NewLib/openr.c" "NewLib/readr.c" "NewLib/renamer.c"
                    "NewLib/unlinkr.c" "NewLib/writer.c"
                    INCLUDE_DIRS "." "./NewFATFS" "./NewLib" "./RAMDisk")