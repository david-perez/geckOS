Timers
######

A :dfn:`timer` is a kernel object that measures the passage of time
using the kernel's system clock. When a timer's specified time limit
is reached it can perform an application-defined action,
or it can simply record the expiration and wait for the application
to read its status.

Any number of timers can be defined. Each timer is referenced by its
memory address.

A timer has the following key properties:

* A :dfn:`duration` specifying the time interval before the timer expires
  for the first time, measured in milliseconds. It must be greater than zero.

* A :dfn:`period` specifying the time interval between all timer expirations
  after the first one, measured in milliseconds. It must be non-negative.
  A period of zero means that the timer is a one shot timer that stops
  after a single expiration. (For example then, if a timer is started with a
  duration of 200 and a period of 75, it will first expire after 200ms and
  then every 75ms after that.)

* An :dfn:`expiry function` that is executed each time the timer expires.
  The function is executed by the system clock interrupt handler.
  If no expiry function is required a :c:macro:`NULL` function can be specified.

* A :dfn:`status` value that indicates how many times the timer has expired
  since the status value was last read.

A timer must be initialized before it can be used. Timer initialization specifies its
expiry function, sets the timer's status to zero,
and puts the timer into the **stopped** state.

A timer is **started** by specifying a duration and a period.
The timer's status is reset to zero, then the timer enters
the **running** state and begins counting down.

When a running timer expires its status is incremented
and the timer executes its expiry function, if one exists;
If the timer's period is zero the timer enters the stopped state;
otherwise the timer restarts with a new duration equal to its period.

Defining a Timer
================

A timer is defined using a variable of type :c:type:`struct timer`.
It must then be initialized by calling :cpp:func:`timer_init()`.

The following code defines and initializes a timer.

.. code-block:: c

    struct timer my_timer;
    extern void my_expiry_function(struct timer *timer_id);

    timer_init(&my_timer, my_expiry_function, NULL);

Suggested Uses
==============

Use a timer to determine whether or not a specified amount of time
has elapsed.

Use a timer to perform other work while carrying out operations
involving time limits.

API Reference
=============

.. doxygengroup:: timer_apis
