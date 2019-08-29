.. _porting:

Porting geckOS to a hardware platform
=====================================

geckOS can run on many microcontrollers, but its real benefit, hardware
abstraction, can only be noticed if someone has ported it to work with a
specific programmable device, and defined an API to interact with it. See the
:ref:`Supported devices <supported_devices>` section for a list of currently
supported devices.

This tutorial will guide you through porting :ref:`geckOS <introduction>` to a
new hardware platform in order to enable application writers to program a
device with a clean, safe, and friendly API. Unlike other embedded operating
systems with complicated codebases and interwoven dependencies, porting geckOS
to a new device is an easy and painless task.

Motivation
##########

Suppose you want to manufacture a board that has LEDs on it. You want users to
be able to turn the LEDs on and off, but without burdening them with the
knowledge of how the LEDs are connected, to which pin, what protocol they use,
etc. You just want them to be able to make a call to a regular function, and,
just like that, the LEDs should light up. No configuration, no writing to
memory-mapped registers, no frills... A simple call to a function with a
signature of the likes

.. code-block:: c

    void toggle_led(uint8_t led_number);

and the LED identified by ``led_number`` should toggle its state, that is, if
the LED was lit, it should go off, and if it was off it should light up. A user
should be able to write a short program like this one:

.. code-block:: c

    int main() {
        toggle_led(0);
    }

And LED 0 should light up. Simple enough.

Of course, toggling an LED is not a terribly difficult thing; that's why we
have chosen it as an example for this guide. But you can hide whatever you want
behind one of these exposed functions, keeping your clever designs away from
prying eyes, or making them domain-specific to make the API even more friendlier.
Instead of ``toggle_led()``, you could expose ``toggle_recording_indicator()``
in a device that can record samples with a microphone. Or a
``is_sample_of_endangered_animal(t_sample *sample)`` to allow users to make use
of a clever classification algorithm in their applications without giving them
the source code.

How to do it
############

We're going to name our board "Blink", for lack of a better name. We will
prototype Blink on a starter kit with LEDs. For illustration purposes, we will
assume we are working with the `EFM32 Happy Gecko SLSTK3400A
<https://www.silabs.com/products/development-tools/mcu/32-bit/efm32-happy-gecko-starter-kit>`_,
a starter kit housing:

- An ``EFM32HG322F64`` microcontroller, with
- 64 KiB of flash memory starting at address ``0x0``, and
- 8 KiB of RAM starting at address ``0x20000000``.

Most importantly, the kit also has two GPIO-connected LEDs built-in. Both of
them are connected to port F; LED 0 to pin 4, LED 1 to pin 5.

#. To add support for the Blink board, we need to create a folder for it under
   ``targets/`` and create a set of files inside. To make things easier, we can
   copy a template to get the required structure and we will fill in the
   details.

.. code-block:: console

    $ cp -r targets/template targets/Blink
    $ cd targets/Blink
    $ tree .
    Blink
    ├── device-specific-sources.cmake
    ├── include
    │   ├── device_init.h
    │   ├── svc_device.h
    │   ├── svc_dispatcher_device.h
    │   └── svc_handlers_device.h
    └── source
        ├── device_init.c
        ├── svc_device.c
        ├── svc_dispatcher_device.c
        └── svc_handlers_device.c

    2 directories, 9 files

These files are the bare minimum we need when porting the OS to a new hardware
platform. geckOS needs and will look for these files. Any other *source* files
that we add to the source tree, we'll need to tell geckOS where to find them.
We can do that in the ``device-specific-sources.cmake`` file.

2. Let's start out with our declarations in the header files. We need to tell
   geckOS the signature of the function we want to expose. We do this in
   ``svc_device.h``:

.. code-block:: c

    #ifndef SVC_DEVICE_H
    #define SVC_DEVICE_H

    #include <stdint.h>

    void toggle_led(uint8_t led_number);

    #endif // SVC_DEVICE_H

3. The function will be accessible to application writers via an SVCall
   exception. We need to assign a *number* to the supervisor call that will be
   associated with our toggling function. We can define 150 different
   supervisor calls, with numbers ranging from 0 to 149 inclusive. These
   numbers are defined in an enum in ``svc_dispatcher_device.h``. Unless we
   override the assigned numbers, the first element of the enum will be
   associated with SVC number 0, the next one with 1, and so on.

.. code-block:: c

    #ifndef SVC_DISPATCHER_DEVICE_H
    #define SVC_DISPATCHER_DEVICE_H

    #include <stdint.h>

    enum svc_no_device {
        TOGGLE_LED,
    };

    void svc_dispatcher_device(uint32_t *svc_args);

    #endif // SVC_DISPATCHER_DEVICE_H

4. Next, we have to declare the function that will actually toggle the LED,
   that is, the SVC *handler*. We will later see how this handler gets invoked.

.. code-block:: c

    #ifndef SVC_HANDLERS_DEVICE_H
    #define SVC_HANDLERS_DEVICE_H

    #include <stdint.h>

    void toggle_led_handler(uint8_t led_number);

    #endif // SVC_HANDLERS_DEVICE_H

5. That's it for declarations. Let's implement these things. First of, the
   function that the user calls, ``toggle_led()``. Its implementation is very
   simple, since it delegates to the handler, which does all of the heavy
   lifting. ``toggle_led()`` just has to put the first argument into register 0
   and call the ARM ``svc`` instruction with the number ``TOGGLE_LED``, which
   equals 0. How to do this is a bit tricky, since in GCC, the only way to it
   is directly using Assembly. Thankfully, geckOS comes with a couple of
   bundled C macros that expand out to the necessary assembly. We can simply
   use the ``SYSCALL1`` macro, which takes in the return type of the function,
   its name, its SVC number, and the type and name of the first parameter. It
   will expand to the required implementation. The contents of ``svc_device.c``
   are thus:

.. code-block:: c

    #include "svc_device.h"
    #include "libk_svc_bodies.h"

    // We only need this include to have access to the SVC numbers.
    #include "svc_dispatcher_device.h"

    SYSCALL1(void, toggle_led, TOGGLE_LED, uint8_t, first_param)

In case you're curious, the preprocessor expands it out to:

.. code-block:: c

    void toggle_led(uint8_t led_number) {
        register uint8_t r0 asm("r0") = led_number;

        __asm__ volatile("svc %[nr]\n"
                : "=r" (r0)
                : [nr] "i" (TOGGLE_LED), "r" (r0)
                : "memory", "r1", "r2", "r3", "r12", "lr");
    }

The code is not hard to understand if you're familiar with `GCC's inline
assembly <https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html>`_, but it's
just a bit too long to be writing it out by hand every time you want to expose a
function.

6. We now turn to the issue of how the handler gets invoked. We do this in
   ``svc_dispatcher_device.c``. The function there simply iterates over all of
   the SVC numbers, and calls the corresponding handler, if one matches. It's
   just a switch. Whenever you want to expose a new function, you add a branch
   to it. The arguments the user passed to the supervisor call are laid out in
   memory, which we can comfortably access using ``svc_args``, a pointer that points to
   the them. So the first (32-bit) argument is ``svc_args[0]``, the second one
   is ``svc_args[1]``, and so on. You can cast them to the appropiate types
   once you know which supervisor call has been called.

.. code-block:: c

    #include "svc_dispatcher_device.h"
    #include "svc_handlers_device.h"

    void svc_dispatcher_device(uint32_t *svc_args) {
        uint32_t svc_number = ((char *) svc_args[6])[-2];

        if (svc_number == TOGGLE_LED) {
            toggle_led_handler(svc_args[0]);
        }

        // We should never reach here.
    }

7. Enough with plumbing things together. Let's actually turn the LED on, in
   ``svc_handlers_device.c``. Here's where we do all of the hard work, so that
   the user can delegate to it. In order to turn an LED on, we need to
   configure the GPIO pin it's connected to.

.. code-block:: c

    #include "em_gpio.h"

    #define GPIO_PORT_F         5
    #define GPIO_LED0_PORT      GPIO_PORT_F
    #define GPIO_LED0_PIN       4
    #define GPIO_LED1_PORT      GPIO_PORT_F
    #define GPIO_LED1_PIN       5

    void toggle_led_handler(uint8_t led_number) {
        if (led_number == 0) {
            GPIO_PinOutToggle(GPIO_LED0_PORT, GPIO_LED0_PIN);
        } else if (led_number == 1) {
            GPIO_PinOutToggle(GPIO_LED1_PORT, GPIO_LED1_PIN);
        }
    }

To make our code cleaner and reusable, we should move the ``#define`` s to
``svc_handlers_device.h``. Note we ourselves are delegating to ``em_gpio``,
part of Silicon Labs MCU SDK, in order to toggle the pin the LED is connected
to. We can use any functions from `emlib
<https://docs.silabs.com/mcu/latest/efm32hg/group-emlib>`_ by including the
appropiate header files; there's no need to resort to writing to registers from
scratch.

8. There is only one last thing to do: initialise the LEDs. This has to be done
   before an application calls calls ``toggle_led()``, or otherwise the LEDs
   won't work. One option could be to track if it is the first time
   ``toggle_led()`` is being called in its handler, with a global variable, and
   do the initialisation there as well. However, there exists a general and
   cleaner way to achieve this. Thankfully, geckOS calls a function
   ``device_init()`` at kernel startup, just before jumping to the loaded
   applications. We can use this hook to put any kind of boilerplate
   initialisation code we want, in ``device_init.c``.

.. code-block:: c

    #include "device_init.h"
    #include "em_gpio.h"
    #include "em_cmu.h"

    void device_init() {
        // Route clock sources to GPIO peripherals.
        CMU_ClockEnable(cmuClock_HFPER, true);
        CMU_ClockEnable(cmuClock_GPIO, true);

        // Configure pins where LEDs are connected.
        GPIO_PinModeSet(GPIO_LED0_PORT, GPIO_LED0_PIN, gpioModePushPull, 0);
        GPIO_PinModeSet(GPIO_LED1_PORT, GPIO_LED1_PIN, gpioModePushPull, 0);
    }

That's it! The Blink device is ready to go. We can now compile the kernel
passing ``-DTARGET_DEVICE=Blink`` and write an application for Blink that makes
use of the ``toggle_led()`` function.
