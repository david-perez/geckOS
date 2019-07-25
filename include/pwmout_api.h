#ifndef PWMOUT_API_H
#define PWMOUT_API_H

#include <stdint.h>
#include <stdbool.h>

bool pwmout_disable_channel_route();
void pwmout_init();
void pwmout_write(float value);
void pwmout_period(float seconds);

#endif // PWMOUT_API_H
