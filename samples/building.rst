..
   This is a file to be included in all sample C programs, since all are built
   the same way.

Building
########

This program is provided as a ``cmake`` project in order to be run on *any*
USB-enabled EFM32 based device supported by geckOS. [#]_ Make sure to set the
variables ``FLASH_ORIGIN``, ``FLASH_LENGTH``, ``RAM_ORIGIN``, ``RAM_LENGTH``,
either as command-line arguments (with the ``-D`` flag as below) or directly in
the ``CMakeLists.txt`` file, depending on the microcontroller your device has
and the size in flash and RAM of the compiled kernel.

.. code-block:: console

   $ mkdir build-release && cd build-release
   $ cmake -DCMAKE_BUILD_TYPE=Release \
      -DFLASH_ORIGIN=0xa000 -DFLASH_LENGTH=0x6000 \
      -DRAM_ORIGIN=0x20001000 -DRAM_LENGTH=0x2000 ..
   $ make

You should obtain an ELF file with debugging symbols named like the project,
useful for disassembly and debugging, and a ``.bin`` file, to be transferred to
geckOS's mass storage device on bootloader start up.

.. [#] The cmake project is based on ``efm32-base`` (`link <https://github.com/ryankurte/efm32-base>`_).
