Kernel services
===============

The following are kernel services provided by geckOS, available on *any*
USB-enabled EFM32 based device. These services provide common functionality
generally available in conventional embedded operating systems. [#]_

These APIs are also exposed as supervisor calls, and meant to be used by
application writers, although device manufacturers can use them as well in
kernelspace by calling their respective supervisor call handlers. Their
implementation is device-independent, as it can be realized without having to
access hardware.

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   timers.rst

.. [#] Far from providing a large featureset such as those of conventional
       real-time operating systems, geckOS only intends to provide services which are
       used *very* commonly in applications and whose implementation is largely
       device-independent. Kernel services that are only useful in certain
       applications (memory protection, communication between threads, file systems)
       or whose implementation depends on the underlying hardware (device driver APIs,
       interaction with peripherals) should better be provided by the device
       manufacturer.
