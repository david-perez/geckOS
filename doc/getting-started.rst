.. _getting-started:

Getting started
===============

This tutorial will guide you through building :ref:`geckOS <introduction>` and
compiling and running a sample application.

.. note:: Building has only been tested on Linux, but there should be no
    impediment to building on other platforms as long as you can install the
    required tools below.

Requirements
############

- ``cmake``
- ``git``
- ``make``
- `GNU Arm Embedded Toolchain
  <https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads>`_

Know your hardware
##################

geckOS can run on many microcontrollers, but its real benefit, hardware
abstraction, can only be noticed if someone has ported it to work with a
specific programmable device, and defined an API to interact with it. See the
:ref:`Supported devices <supported_devices>` section for a list of currently
supported devices.

In this guide we'll work with the `EFM32 Happy Gecko SLSTK3400A
<https://www.silabs.com/products/development-tools/mcu/32-bit/efm32-happy-gecko-starter-kit>`_
for illustration purposes, a starter kit by Silicon Labs. This device houses:

- An ``EFM32HG322F64`` microcontroller, with
- 64 KiB of flash memory starting at address ``0x0``, and
- 8 KiB of RAM starting at address ``0x20000000``.

The device also has many peripherals, such as two buttons, two LEDs, and a 128
x 128 LCD screen, among others.

Compiling geckOS
################

The operating system has a ``cmake``-based buildsystem generator. [#]_

#. Create an out-of-source directory where the build artifacts will get placed,
   for example in the root directory. We'll create a directory named
   ``build-release`` because it will be a "Release" build (i.e. optimized code
   without debugging symbols):

   .. code-block:: console

        cd geckOS
        mkdir build-release
        cd build-release

#. Configure the build:

   .. code-block:: console

        cmake -DCMAKE_BUILD_TYPE=Release \
            -DDEVICE=EFM32HG322F64 \
            -DTARGET_DEVICE=efm32hg_slstk3400a \
            -DAPPLICATION_ORIGIN=0x8800 \
            -DAPPLICATION_MAX_SIZE=0x7800 ..

Note how we pass in some options to configure the build:

CMAKE_BUILD_TYPE
    This is to configure whether the type of build should be a "Debug" build or
    a "Release" build.

DEVICE
    This option actually specifies the *microcontroller* our device has. geckOS
    can run on *any* USB-enabled Silicon Labs EFM32 microcontroller.

TARGET_DEVICE
    This option specifies the device we're building geckOS for. Application
    writers will be able to use the API defined for the device.

APPLICATION_ORIGIN
    This has to be set to an address well-above the operating system's binary
    code. It is the address the bootloader will jump to when a geckOS
    application has been loaded. ``0x8800`` works fine for release builds.

APPLICATION_MAX_SIZE
    The maximum size of an application. Ideally this should be set to
    ``RAM_LENGTH - APPLICATION_ORIGIN``, where ``RAM_LENGTH`` is the amount of
    available RAM on your microcontroller. In our case, we have 8 KiB of RAM.
    Note that ``8 * 1024 - 0x8800 = 0x7800``.

You can pass these options to ``cmake`` in the command line as we have done so
above, or you can define these in the root-level build configuration file
``CMakeLists.txt``. Command-line options take precedence in case of
redefinition.

3. Compile the code.

   .. code-block:: console

        make

It should take a couple of seconds to build. You'll be able to see a log
detailing the progress of the build process. If successful, it should end with
something like this:

   .. code-block:: console

    ...
    GNU ld (GNU Tools for Arm Embedded Processors 8-2018-q4-major) 2.31.51.20181213
       text	   data	    bss	    dec	    hex	filename
      20396	      0	   3720	  24116	   5e34	geckO

Your folder should be populated with a bunch of files. Among others, you should
see:

``geckOS``
    An ELF file which you can disassemble with ``arm-none-eabi-objdump``. Very
    useful for debugging.

``geckOS.dmp``
    A dump text file you can inspect to see the memory contents and addresses
    of all symbols in your executable.

``geckOS.{bin, hex}``
    These are the binary files you can use to upload to your physical device.

Running geckOS
##############

Flash the binary file to your device. If you're using a starter kit, you can do
this by connecting it to your computer with the mini USB connector and dragging
and dropping the ``.bin`` file. Alternatively, you can also do it from the
command line:

    .. code-block:: console

        make erase # (make e also works)
        make flash # (make f also works)

.. warning:: Make sure the microcontroller's memory is empty before flashing
    the device. You can erase its contents by typing ``make erase``.

If you're not running on a starter kit, you can use one to configure your
device, or use an external hardware programmer.

That's it! geckOS is now running on your device, and it should launch the
bootloader, waiting for you to copy an application. In the next section, we'll
compile a sample program that makes use of the device's API and geckOS' kernel
services, and run it on the device.

Compiling an application
########################

Let's have a look at a sample application for our starter kit. Head on to the
``samples/`` folder and print the source code of the :ref:`blinky-timers
<blinky-timers>` application.

    .. code-block:: console

        cd ../samples/blinky-timers
        cat source/main.c

The source code is very short. Here it is:

    .. code-block:: c

        #include <stdbool.h>
        #include <stdint.h>
        #include "libk.c"

        void my_expiry_function(struct timer *timer) {
            svc_leds_toggle(1);
        }

        void my_expiry_function2(struct timer *timer) {
            svc_leds_toggle(0);
        }

        int main() {
            struct timer my_timer;
            svc_timer_init(&my_timer, my_expiry_function);
            svc_timer_start(&my_timer, 1000, 1000);

            struct timer my_timer2;
            svc_timer_init(&my_timer2, my_expiry_function2);
            svc_timer_start(&my_timer2, 2000, 2000);

            while (true) {
            }
        }

The application is simple and easy to read. It defines two periodic timers that
toggle the LEDs on the board. One of the timers expires every second, while the
other one expires every 2 seconds.

You're free to compile the application any way you want. It's easy to do so
with plain ``arm-none-eabi-gcc`` from the command line, since it consists of a
single source file and a couple of includes. However, the application comes
with a ``cmake``-based buildsystem generator to make life easier. The steps to
compile an application are very similar to the ones we followed to compile the
operating system.

#. Create an out-of-source directory where the build artifacts will get placed:

   .. code-block:: console

        mkdir build-release
        cd build-release

#. Configure the build:

   .. code-block:: console

        cmake -DCMAKE_BUILD_TYPE=Release \
            -DFLASH_ORIGIN=0x8800 \
            -DFLASH_LENGTH=0x7800 \
            -DRAM_ORIGIN=0x20001000 \
            -DRAM_LENGTH=0x1000 ..

We're instructing it to resolve addresses starting at ``0x8800``, the address
the bootloader will jump to (the value we gave above for the
``APPLICATION_ORIGIN`` option). The linker script will take care of placing the
start of the `Reset_Handler` at this very same address. The value of
``FLASH_LENGTH`` should be the same as the value we gave for
``APPLICATION_MAX_SIZE`` when compiling the OS. We also specify that the
application should use the higher 4 KiB of RAM, mapped from address
``0x20001000`` to address ``0x20002000``. This is because geckOS uses the lower
4 KiB of RAM to maintain kernel structures. [#]_

Again, you're free to pass in these options to ``cmake`` as command-line
arguments, or define them in the build configuration file ``CMakeLists.txt``.

3. Compile the code.

   .. code-block:: console

        make

Again, if everything went OK, you should get the same set of files as before,
useful for inspection, dissassembly and debugging. Out you should get a small
binary, of just a couple hundred bytes, named ``blinky-timers.bin``.
This is the file we will copy to our device.

Running the application
#######################

Connect the microcontroller's onboard USB to your machine. In our starter kit,
this is done by plugging in the *micro*-USB connector. A regular removable mass
storage device should pop up in your file explorer, named after the target
device (``efm32hg_slstk3400`` in our case). It should be configured as an empty
FAT filesystem. Simply drag and drop the ``blinky-timers.bin`` file.

That's it! If successful, you should see the LEDs on the board start to blink
periodically.

.. [#] Credit goes to the wonderful ``efm32-base`` (`link <https://github.com/ryankurte/efm32-base>`_) project.
.. [#] For now. geckOS's RAM usage can be heavily optimized.
