#include "libk.c"

void test_neopixels() {
    // Test a couple of custom rgb values.
    svc_rgb(255, 255, 0);
    svc_sleep(1000);
    svc_rgb(0, 255, 255);
    svc_sleep(1000);

    // Colour each of the pixels with all 7 colours.
    for (uint8_t i = 1; i < 8; i++) {
        for (uint8_t pixel = 1; pixel <= 9; pixel++) {
            svc_pixel(i, pixel);
            svc_sleep(100);
        }
    }

    // Colour all pixels with all 7 colours.
    for (uint8_t i = 1; i < 8; i++) {
        svc_colour(i);
        svc_sleep(100);
    }

    // Flash all pixels with all 7 colours.
    for (uint8_t i = 1; i < 8; i++) {
        svc_flash(i, 300);
    }

    svc_colour(0);
}

void test_sounder() {
    // Test an A440 for a couple of seconds.
    svc_tone(440);
    svc_sleep(5000);

    // A C-Major scale from C3 to C4.
    float notes[8] = {261.62, 293.66, 329.62, 349.22, 391.99, 440, 493.88, 523.25};

    for (int i = 0; i < 8; i++) svc_beep(notes[i], 500);

    // Play it in reverse.
    for (int i = 7; i >= 0; i--) svc_beep(notes[i], 500);
}

int main() {
    test_neopixels();
    test_sounder();

    while (true) {
    }
}
