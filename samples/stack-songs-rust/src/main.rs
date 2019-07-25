#![no_std]
#![no_main]

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

// You can put a breakpoint on `rust_begin_unwind` to catch panics.
extern crate panic_halt;

use cty;
use cortex_m_rt::entry;

include!("notes.rs");

// Import songs we want to play.
include!("mario_bros.rs");
include!("happy_birthday.rs");
include!("marcha_real.rs");

#[derive(Copy, Clone)]
enum Colour {
    Off = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Yellow = 6,
    White = 7
}

static mut odd: bool = true;

fn spanish_flag(note: Note) {
    if note == Note::Z { return; }

    unsafe {
        if odd {
            for i in 1..=3 { svc_pixel(Colour::Red as u8, i); }
            for i in 4..=6 { svc_pixel(Colour::Yellow as u8, i); }
            for i in 7..=9 { svc_pixel(Colour::Red as u8, i); }
        } else {
            svc_pixel(Colour::Red as u8, 3);
            svc_pixel(Colour::Red as u8, 6);
            svc_pixel(Colour::Red as u8, 9);
            svc_pixel(Colour::Red as u8, 1);
            svc_pixel(Colour::Red as u8, 4);
            svc_pixel(Colour::Red as u8, 7);
            svc_pixel(Colour::Yellow as u8, 2);
            svc_pixel(Colour::Yellow as u8, 5);
            svc_pixel(Colour::Yellow as u8, 8);
        }

        odd = !odd;
    }
}

const NUM_NEOPIXELS: u8 = 9;
static mut led_index:u8 = 0;
static mut colour_index:u8 = 0;

fn pixel_frenzy(note: Note) {
    if note == Note::Z { return; }

    unsafe {
        svc_pixel(colour_index + 1, led_index + 1);
        led_index = (led_index + 1) % NUM_NEOPIXELS;
        colour_index = (colour_index + 1) % 7;
    }
}

fn happy_birthday_leds(note: Note) {
    unsafe {
        match note {
            Note::G3 => { svc_colour(Colour::Blue as u8); }
            Note::A3 => { svc_colour(Colour::Green as u8); }
            Note::C4 => { svc_colour(Colour::Cyan as u8); }
            Note::B3 => { svc_colour(Colour::Red as u8); }
            Note::D4 => { svc_colour(Colour::Magenta as u8); }
            Note::F4 => { svc_colour(Colour::Yellow as u8); }
            Note::E4 => { svc_colour(Colour::White as u8); }
            _ => { svc_colour(Colour::Off as u8); }
        }
    }
}

fn leds_off(note: Note) {
    unsafe { svc_colour(0); }
}

fn play_song(melody: &[Note], tempo: &[u16], slowdown:u16, f: fn(Note), g: fn(Note)) {
    for it in melody.iter().zip(tempo.iter()) {
        let (note, duration) = it;
        let milliseconds = duration * slowdown;
        f(*note);
        unsafe { svc_beep(*note as u16, milliseconds); }

        let pause_between_notes = duration * slowdown / 2;
        unsafe {
            g(*note);
            svc_sleep(pause_between_notes);
        }
    }
}

fn play_marcha_real() {
    play_song(&marcha_real_1_3, &tempo_marcha_real_1_3, 50, spanish_flag, leds_off);
    play_song(&marcha_real_4, &tempo_marcha_real_4, 50, spanish_flag, leds_off);
    play_song(&marcha_real_1_3, &tempo_marcha_real_1_3, 50, spanish_flag, leds_off);
    play_song(&marcha_real_5, &tempo_marcha_real_5, 50, spanish_flag, leds_off);
    play_song(&marcha_real_6_9, &tempo_marcha_real_6_9, 50, spanish_flag, leds_off);

    // Make the end a little bit more solemn.
    play_song(&marcha_real_6_9, &tempo_marcha_real_6_9, 60, spanish_flag, leds_off);
}

#[entry]
fn main() -> ! {
    play_marcha_real();
    unsafe { svc_sleep(4000); }
    play_song(&melody_mario_bros, &tempo_mario_bros, 10, pixel_frenzy, leds_off);
    unsafe { svc_sleep(4000); }
    play_song(&melody_happy_birthday, &tempo_happy_birthday, 50, happy_birthday_leds, leds_off);

    loop {
    }
}
