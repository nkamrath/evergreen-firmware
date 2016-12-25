#ifndef __PINS__H
#define __PINS__H

#include "driver/gpio.h"
#include "pin_defs.h"
#include <stdint.h>

bool Pin_Init(uint32_t pin_number, gpio_config_t* config);

void Pin_SetOutput(uint64_t mask);
void Pin_ClearOutput(uint64_t mask);
bool Pin_ReadInput(uint64_t mask);

#endif