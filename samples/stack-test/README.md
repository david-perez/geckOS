# `stack-test`

This C application tests the functionality of the STACK device, calling the
SVCs compiled in the `geckOS` kernel.

## Building

```console
$ mkdir build-release && cd build-release
$ cmake -DCMAKE_BUILD_TYPE=Release \
    -DFLASH_ORIGIN=0xa000 -DFLASH_LENGTH=0x6000 \
    -DRAM_ORIGIN=0x20001000 -DRAM_LENGTH=0x2000 ..
$ make
```

Remember that flash origin has to sit somewhere above kernel code, and you have
to link geckOS's bootloader so that it jumps to the value you set it to.
Similarly, RAM origin has to sit somewhere above the RAM region used by the
kernel.
