#include "pwmout_api.h"
#include "em_cmu.h"
#include "em_timer.h"

#define EFM32_HFXO_FREQ         (24000000UL)
#define REFERENCE_FREQUENCY     EFM32_HFXO_FREQ

#define PWM_TIMER       TIMER0

// In STK3400, PA1 is routed to timer 0 via channel 1 location 0.
// See the datasheet for your board.
#define OUTPUT_PORT gpioPortA
#define OUTPUT_PIN 1
#define LOCATION TIMER_ROUTE_LOCATION_LOC0
#define PWM_CH 1
#define PWM_CH_PEN TIMER_ROUTE_CC1PEN
#define PWM_FREQ 10000

static uint32_t pwm_prescaler_div;

void pwmout_init() {
    // Enable clock for TIMER0 module.
    CMU_ClockEnable(cmuClock_TIMER0, true);

    // Set GPIO pin as output.
    GPIO_PinModeSet(OUTPUT_PORT, OUTPUT_PIN, gpioModePushPull, 0);

    // Configure CC channel parameters.
    TIMER_InitCC_TypeDef timerCCInit = {
        .eventCtrl  = timerEventEveryEdge,
        .edge       = timerEdgeBoth,
        .prsSel     = timerPRSSELCh0,
        .cufoa      = timerOutputActionNone,
        .cofoa      = timerOutputActionNone,
        .cmoa       = timerOutputActionToggle,
        .mode       = timerCCModePWM,
        .filter     = false,
        .prsInput   = false,
        .coist      = false,
        .outInvert  = false,
    };
    TIMER_InitCC(TIMER0, 1, &timerCCInit);

    // Route CC0 to location and enable pin.
    TIMER0->ROUTE |= (PWM_CH_PEN | LOCATION);

    // Configure timer parameters, and initialize it.
    TIMER_Init_TypeDef timerInit =
    {
        .enable     = true,
        .debugRun   = true,
        .prescale   = timerPrescale64,
        .clkSel     = timerClkSelHFPerClk,
        .fallAction = timerInputActionNone,
        .riseAction = timerInputActionNone,
        .mode       = timerModeUp,
        .dmaClrAct  = false,
        .quadModeX4 = false,
        .oneShot    = false,
        .sync       = false,
    };
    TIMER_Init(TIMER0, &timerInit);

    // Set default 20ms frequency and 0ms pulse width.
    pwmout_period(0.02);
}

void pwmout_write_channel(uint32_t channel, float value) {
    uint32_t width_cycles = 0;
    if (value < 0.0f) {
        width_cycles = 0;
    } else if (value >= 1.0f) {
        width_cycles = PWM_TIMER->TOPB + 1;
    } else {
       width_cycles = (uint16_t)((float)PWM_TIMER->TOPB * value);
    }

    TIMER_CompareBufSet(PWM_TIMER, channel, width_cycles);
}

void pwmout_write(float value) {
    pwmout_write_channel(PWM_CH, value);
}

float pwmout_calculate_duty(uint32_t width_cycles, uint32_t period_cycles) {
    if(width_cycles > period_cycles) {
        return 1.0f;
    } else if (width_cycles == 0) {
        return 0.0f;
    } else {
        return (float) width_cycles / (float) period_cycles;
    }
}

// Set the PWM period, keeping the duty cycle the same.
void pwmout_period(float seconds) {
    // Find the lowest prescaler divider possible.
    // This gives us max resolution for a given period.

    // The value of the top register if prescaler is set to 0.
    uint32_t cycles = (uint32_t) REFERENCE_FREQUENCY * seconds;
    pwm_prescaler_div = 0;

    // The top register is only 16 bits, so we keep dividing till we are below 0xFFFF.
    while (cycles > 0xFFFF) {
        cycles /= 2;
        pwm_prescaler_div++;

        // Max pwm_prescaler_div supported is 10.
        if (pwm_prescaler_div > 10) {
            pwm_prescaler_div = 10;
            cycles = 0xFFFF; // Set it to max possible value.
            break;
        }
    }

    // Check if anything changed.
    if(((PWM_TIMER->CTRL & _TIMER_CTRL_PRESC_MASK) == (pwm_prescaler_div << _TIMER_CTRL_PRESC_SHIFT)) && (TIMER_TopGet(PWM_TIMER) == cycles)) return;

    // Save previous period for recalculation of duty cycles.
    uint32_t previous_period_cycles = PWM_TIMER->TOPB;

    // Set prescaler.
    PWM_TIMER->CTRL = (PWM_TIMER->CTRL & ~_TIMER_CTRL_PRESC_MASK) | (pwm_prescaler_div << _TIMER_CTRL_PRESC_SHIFT);

    // Set Top Value, which controls the PWM period.
    TIMER_TopBufSet(PWM_TIMER, cycles);

    // For each active channel, re-calculate the compare value.
    pwmout_write_channel(PWM_CH, pwmout_calculate_duty(PWM_TIMER->CC[PWM_CH].CCVB, previous_period_cycles));
}
