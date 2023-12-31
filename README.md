# NewFATFS
POSIX and NewLib compatible FAT filesystem based on Chan's FatFs

/==============================================================================/

FAT File System routines based on Chan's FatFs (<http://elm-chan.org/fsw/ff/>).
Please see his original copyright and license further below.

Copyright (c)2022-2023, Isaac Marino Bavaresco (isaacbavaresco@yahoo.com.br)

Although most of the code is still Chan's, this version has extensive changes
to its structure and API.

The main goal was to implement the most POSIX-compliant version possible, so
we could reuse code from desktop applications without (or with minimal) changes.

Another goal was to integrate seamlessly with NewLib, which I think was
acomplished very well.

The original version has a lot of issues, including security related, but the
most annoying one is its weird API.

In 2005 I wrote a FLASH file system for a commercial product and it proved to
be very good, although only suitable for embedded memories. In 2020 the product's
lifecycle was (long) past, then I published it on GitHub:
(<https://github.com/Ibavaresco/FileSystem/tree/master>)

In 2013 I started development of a new product that needed SD-Card support. The
selected MCU was an Atmel ARM, and the application automatically generated by
ASF included Chan's FatFs. It annoyed me that its API was so weird, so I wrote
a (more or less) POSIX compatibility wrapper layer
(<http://www.piclist.com/techref/member/IMB-yahoo-J86/Atmel-s-FatFS-Wrapper.htm>)

Later I re-wrote it in a NewLib-compatible format. It was published on
<https://spaces.atmel.com/gf/project/posixfatfs>, but now it is not accesible
anymore.

Now it is time to go all the way and create a native version without adaptation
layers, and in the process fix a lot of other issues.

First I "maximized" the configuration. That is, I removed all the options that
reduced features (FF_FS_READONLY, FF_FS_MINIMIZE, FF_FS_TINY, etc.) and enabled
all the options that included features (FF_FS_EXFAT, FF_USE_LFN, etc.).

Then I removed all the conditional compilation directives and replaced several
macros with their values. Also, I replaced several "magic-numbers" spread all
over the code. There are still a lot of them that I plan to replace as time goes
by.

A code re-formatter (Astyle) was used to change the style to Whitesmiths and fix
a lot of unreadable and confusing formatting.

I noticed that the way he uses the semaphores/mutexes was not safe enough and
left some critical parts unprotected from race conditions, so I changed it.
I chose a simple solution of a single mutex for all the filesystem, but that may
not be ideal for some configurations and I plan to add more options in the future.
I think that the ideal solution is comprised of a global mutex to protect the
accesses to filesystem-wide objects for a short time and one mutex for each volume
or physical drive (that will depend on whether the physical drive layer can
support concurrency or not).

Another point that I think he got wrong was the necessity for several LFN buffers.
In my analysis, it seems that the LFN buffers will never be used concurrently,
because they are always initialized only after the filesystem is locked and
released before the filesystem is unlocked. Besides, there is just one pointer in
the 'FATFS' struct. I'm still trying to find some condition that requires several
buffers, but for now I'm using one static buffer in each 'ffs_volume_t' struct.

The locking mechanism is another complicated point. I chose a different approach
of two or threee function layers, to simplify the logic and provide some sort of
"finally" feature.

I also changed most of the types and macros names, to provide a more homogeneous
approach and require less memorization. I plan to change many of the variables and
field names too, because most of them doesn't make much sense to me.

I used a different way of loading physical drivers, based on code from STM, but
with my personal touch. Also, I totally changed the way volumes are mounted and
unmonted, because I never managed the original way to work correctly. If I unmounted
a volume, I could not manage to mount it again without a reboot.

As the new file system is POSIX and NewLib compatible, we can use all the resources
of the C library, including bufferd files and the functions that work with them
(fprintf, fscanf, etc.). That rendered useless several of the FatFs functions
(f_gets, f_puts, f_printf).


WARNING: Although I tested most of the code, it is still under development. I am
publishing it now to allow for others to try it and help in the testing and
perhaps in the development too.

/==============================================================================/

    /-----------------------------------------------------------------------------/

    /  FatFs - Generic FAT Filesystem Module  R0.15 w/patch1                      /
    
    /-----------------------------------------------------------------------------/

    Copyright (C) 2022, ChaN, all right reserved.
    
    FatFs module is an open source software. Redistribution and use of FatFs in
    source and binary forms, with or without modification, are permitted provided
    that the following condition is met:
    
    1. Redistributions of source code must retain the above copyright notice,
        this condition and the following disclaimer.
    
    This software is provided by the copyright holder and contributors "AS IS"
    and any warranties related to this software are DISCLAIMED.
    The copyright owner or contributors be NOT LIABLE for any damages caused
    by use of this software.
