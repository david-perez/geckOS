#include <math.h>
#include <stdio.h>
#include "temperature.h"
#include "em_cmu.h"
#include "em_adc.h"

/* The temperature gradient for the internal temperature sensor built into the */
/* ADC is in the device's datasheet. */
#define TEMP_GRAD       ((float) -6.27)

static uint32_t temp_offset;

void temperature_init() {
    CMU_ClockEnable(cmuClock_ADC0, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_HFPER, true);

    uint8_t prod_rev = (DEVINFO->PART & _DEVINFO_PART_PROD_REV_MASK) >>
        _DEVINFO_PART_PROD_REV_SHIFT;
    if ((prod_rev == 16) || (prod_rev == 17)) temp_offset = 112;
    else temp_offset = 0;

    ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
    init.timebase = ADC_TimebaseCalc(0);
    init.prescale = ADC_PrescaleCalc(400000, 0);
    ADC_Init(ADC0, &init);

    ADC_InitSingle_TypeDef sInit = ADC_INITSINGLE_DEFAULT;
    /* Select we want to read from the internal temperature sensor built into */
    /* the ADC. */
    sInit.input = adcSingleInputTemp;
    ADC_InitSingle(ADC0, &sInit);

    /* There is no need to set up interrupt handling for the ADC0 request line, */
    /* since we can continually poll the ADC0 status register to determine when */
    /* the conversion has ended, and we do not need to do anything in the */
    /* meantime. */
}

int32_t temperature_get() {
    // Start an ADC conversion in single mode.
    ADC_Start(ADC0, adcStartSingle);

    // Wait while the conversion is taking place.
    while (ADC0->STATUS & ADC_STATUS_SINGLEACT) {
    }

    /* Read the value and transform it into a centigrade temperature. */
    /* For a reference on the temperature conversion formula, see section */
    /* 23.3.4.2 from the EFM32HG reference manual. */
    uint32_t sample = ADC_DataSingleGet(ADC0) + temp_offset;
    float cal_temp_0 = (float)((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >>
            _DEVINFO_CAL_TEMP_SHIFT);
    float cal_value_0 = (float)((DEVINFO->ADC0CAL2 &
                _DEVINFO_ADC0CAL2_TEMP1V25_MASK) >>
            _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT);
    float t_float = cal_temp_0 - ((cal_value_0 - sample) / TEMP_GRAD);

    return (int32_t)(t_float + 0.5);
}
