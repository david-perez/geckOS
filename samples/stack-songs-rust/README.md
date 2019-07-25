# `stack-songs-rust`

This application showcases some of the features of the STACK device by playing
songs while flashing leds.
It can also serve as a template for other Rust applications written for geckOS.

Songs are defined by two arrays, a melody array specifying the notes, and a
tempo array specifying the beats for which each note should play. The frequency
(in hertz) of the pitches of the notes is defined in `notes.rs`. Tempo arrays
are filled with `u16` numbers: the larger the number, the longer the note will
play for. The (very simple) example songs provided have been transcribed from
their scores using 1 to represent a semiquaver, 4 to represent a quaver, 8 to
represent a crotchet and so on. Songs are played by calling the `play_song`
function:

```console
fn play_song(melody: &[Note], tempo: &[u16], slowdown:u16, f: fn(Note));
```

which takes in the melody and tempo arrays, as well as a `slowdown` parameter
which slows down the tempo: notes play for `beats * slowdown` milliseconds, and
there is a brief pause in between notes proportional to the length of the note
just played. The `f` function is a callback invoked just before each note is to
be played. The example program uses these callback functions to light up the
leds of the STACK device in interesting ways. The `g` function is another
callback, invoked just after the note has ceased to play, but before the pause
between notes.

The project includes three songs: `happy_birthday.rs`,
[`marcha_real.rs`](marcha-real) and `mario_bros.rs`.

[marcha-real]: https://en.wikipedia.org/wiki/Marcha_Real

## Dependencies

To build embedded programs using this template you'll need:

- Rust 1.31, 1.30-beta, nightly-2018-09-13 or a newer toolchain. e.g. `rustup
  default beta`

- The `cargo generate` subcommand. [Installation
  instructions](https://github.com/ashleygwilliams/cargo-generate#installation).

- `rust-std` components (pre-compiled `core` crate) for the ARM Cortex-M
  targets. Run:

``` console
$ rustup target add thumbv6m-none-eabi thumbv7m-none-eabi thumbv7em-none-eabi
thumbv7em-none-eabihf
```

## Building

`cargo build` will produce an ELF file under
`target/thumbv6m-none-eabi/debug`, which is useful for dissassembly and
debugging. To produce the final binary file to be transferred to geckOS's mass
storage device, you can run the command:

``` console
$ cargo objcopy --release --bin stack-songs-rust -- -O binary stack-songs-rust.bin
```

which will automatically carry out all of the steps the binary file generation
process depends on.

## Using this project as a template for other geckOS Rust applications

**NOTE**: This project is in turn based on the template
[`cortex-m-quickstart`](cortex-m-quickstart), which is set up to work on
Cortex-M devices without an operating system, a much more usual and general
setting. For more information on building programs for Cortex-M devices,
consult its tutorial. Additionally, [the embedded Rust book](book) covers
flashing, running and debugging
programs.

[book]: https://rust-embedded.github.io/book
[cortex-m-quickstart]: https://github.com/rust-embedded/cortex-m-quickstart

You'll need to identify some characteristics of the target target device as these will be used to configure the project:

- The ARM core. e.g. Cortex-M3.
- How much Flash memory and RAM does the target device has? e.g. 256 KiB of
  flash and 32 KiB of RAM.
- Where are flash memory and RAM mapped in the address space? e.g. RAM is
  commonly located at address `0x2000_0000`.

You can find this information in the data sheet or the reference manual of your
device.

This project is configured to run on the STACK device, which as of writing is
currently being developed on a Silicon Labs starter kit 3400A board. This board
features an EFM32HG322F64 microcontroller with an ARM Cortex-M0+, 64 KiB flash
and 8 KiB RAM.

1. Change the configuration of the package in the manifest (`.toml` file).
2. Set a default compilation target. There are four options as mentioned at the
   bottom of `.cargo/config`. For the EFM32HG322F64, which has a Cortex-M0+
   core, we'll pick the `thumbv6em-none-eabi` target.

``` console
$ tail -n6 .cargo/config
```

``` toml
[build]
# Pick ONE of these compilation targets
# target = "thumbv6m-none-eabi"    # Cortex-M0 and Cortex-M0+
# target = "thumbv7m-none-eabi"    # Cortex-M3
# target = "thumbv7em-none-eabi"   # Cortex-M4 and Cortex-M7 (no FPU)
target = "thumbv7em-none-eabihf" # Cortex-M4F and Cortex-M7F (with FPU)
```

3. Enter the memory region information into the `memory.x` file. Remember that
   flash origin has to sit somewhere above kernel code, and you have to link
   geckOS's bootloader so that it jumps to the value you set it to. Similarly,
   RAM origin has to sit somewhere above the RAM region used by the kernel.

``` console
$ cat memory.x
MEMORY
{
  /* Adjust these memory regions to match your device memory layout */
  FLASH : ORIGIN = 0x0000a000, LENGTH = 0x6000
  RAM : ORIGIN = 0x20001000, LENGTH = 0x1000
}
```
