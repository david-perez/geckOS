.. _introduction:

Introduction
============

geckOS is a minimal embedded operating system for programmable devices that
facilitates the development of applications by abstracting away the interaction
with the device's hardware. It currently supports USB-enabled Silicon Labs
energy-friendly 32-bit microcontrollers.

Why geckOS?
###########

Typically, embedded systems are a co-development of hardware and software.
Applications are written in a low-level setting, directly accessing device
peripherals and coupling their implementation to the underlying hardware. Most
developers link their application code directly against device driver
implementations, microcontroller vendor SDKs or operating systems to abstract
their code from the lower level details.

However, this situation makes it hard for device manufacturers to release
*programmable* devices, that is, embedded devices that rely on the user to
provide some code in order for their behaviour to be completely defined. In
this sense, the users are also application writers, and as such they need to
know how to cross-compile their code for the target device and link their
implementations with the hardware libraries and the manufacturer's firmware.
This process is cumbersome, error-prone, and very difficult to be carried out
by programmers with little programming experience.

This project introduces geckOS, a minimal embedded operating system for
programmable devices that facilitates the development of applications by
abstracting away the interaction with the device’s hardware. It allows the
device manufacturer to bake into the kernel any kind of functionality,
exposed to the application writer via a clean, domain-specific API.
Application writers are able to work on their code independently, in a
possibly different programming language, to produce a smaller executable
image that only includes the "pluggable" application logic.

The *separation* between the OS and the application is largely
accomplished by a *bootloader*. This is a piece of code in charge of
loading in the application firmware. It exposes a region of the device's
memory to the user's computer via USB in a user-friendly manner. The device
shows up in the user's file explorer as a removable drive configured as a
regular filesystem, to which they simply have to drag and drop their
binary.

The *union* is the easy-to-use programming layer defined by the
manufacturer. The operating system makes this interface accessible by
providing a set of well-defined "system calls". These system calls
provide a standard, safe and extendable communication mechanism that glues
the lower-level manufacturer's firmware and the relocatable application
together.

A couple of example situations in which this separation between application and
OS firmware are:

- A user buys a temperature sensor device for which he wants to act upon with a
  small piece of code, depending on the temperature reading. This piece of code
  will have to be cross-compiled for the target device and linked against a
  vendor SDK to produce a piece of firmware that will finally have to be
  flashed to the microcontroller.
- A manufacturer releases a board for use in school-level education. In a newer
  release, a discontinued LED component has to be substituted by another one
  using a different communication protocol. All of the applications will have
  to be rewritten and recompiled for them to work on the newer board.
- An enviromental biologist wants to write a custom algorithm to detect a
  particular species on an audio recording device. He will have to figure out
  how to interact with the microphone, where to plug his code into the firmware
  and how to compile and flash the whole thing.

With geckOS, the device manufacturer writes the low-level bits and compiles
them with the operating system's kernel to provide a clean,
domain-specific API that is exposed to the application writer via system calls.
The firmware is flashed to the microcontroller using a
hardware programmer. On the other end, the user writes their application,
calling some functions defined in a header file whenever they need to interact with the
programmable device, without having to provide an implementation for them. The
application is compiled separately and the binary is loaded into the
microcontroller using geckOS's USB mass storage device bootloader.

Advantages of geckOS
####################

- Application writers don't need to worry about the programmable device's
  architecture (e.g. interrupt vector tables) nor how the peripherals are
  hooked into the hardware. Their code will continue to work independent of the
  underlying platform it's running on.
- Application writers don't need to link their code against SDKs or use
  complicated vendor IDEs. They can write their code from the comfortableness
  of their favourite text editor, without any dependencies, and in their
  favourite programming language, as long as it can be compiled down to the
  target's instruction set.
- Firmware and application code are developed and compiled separately. If one's
  implementation changes, the other doesn't need to be recompiled.
- Smaller application binaries, useful in situations in which already deployed
  devices need to be updated via downlink-constrained or costly mediums (e.g.
  radio).
- Manufacturers don't need to bundle an on-board programmer in their device
  design for the user to flash applications, since these are loaded using
  geckOS's included USB mass storage device bootloader. Users just plug it into
  their computers and drag and drop their application binaries.
