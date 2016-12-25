#include "pin.h"


bool Pin_Init(uint32_t pin_number, gpio_config_t* config)
{
	gpio_config(config);
	return true;
}

void Pin_SetOutput(uint64_t mask)
{
	GPIO.out_w1ts = (mask & 0xffffffff);
	GPIO.out1_w1ts.data = ((mask >> 32UL) & 0xffffffff);
}

void Pin_ClearOutput(uint64_t mask)
{
	GPIO.out_w1tc = (mask & 0xffffffff);
	GPIO.out1_w1tc.data = ((mask >> 32UL) & 0xffffffff);
}

bool Pin_ReadInput(uint64_t mask)
{
    if(GPIO.in & mask)
    {
    	return true;
    }
    return false;
}