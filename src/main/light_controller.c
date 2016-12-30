#include "light_controller.h"
#include "pin.h"

bool light_state = false;
bool motion_trigger_on = true;
volatile uint32_t motion_trigger_on_time_ticks = 10 * portTICK_RATE_MS * 1000;
uint32_t last_trigger_time_ticks = 0;

void LightController_Refresh(void)
{
	if(motion_trigger_on)
	{
		//figure out if timeout since last motion has occurred to determine light state
		//first, figure out if 32bit overflow is an issue
		uint32_t overflow = 0xffffffff - last_trigger_time_ticks;
		if(overflow < motion_trigger_on_time_ticks)
		{
			uint32_t current_ticks = xTaskGetTickCount();
			if(current_ticks < last_trigger_time_ticks && current_ticks > overflow)
			{
				light_state = false;
			}
			else
			{
				light_state = true;
			}
		}
		else if(xTaskGetTickCount() > (last_trigger_time_ticks + motion_trigger_on_time_ticks))
		{
			light_state = false;
		}
		else
		{
			light_state = true;
		}
	}
	//change the relay state
	if(light_state)
	{
		Pin_SetOutput(LIGHT1_RELAY_PIN_MASK);
	}
	else
	{
		Pin_ClearOutput(LIGHT1_RELAY_PIN_MASK);
	}
	
}

void LightController_On(void)
{
	light_state = true;
	LightController_Refresh();
}

void LightController_Off(void)
{
	light_state = false;
	LightController_Refresh();
}

bool LightController_GetState(void)
{
	return light_state;
}

void LightController_MotionTriggerOn(void)
{
	motion_trigger_on = true;
	LightController_Refresh();
}

void LightController_MotionTriggerOff(void)
{
	motion_trigger_on = false;
	LightController_Refresh();
}

bool LightController_GetMotionTriggerState(void)
{
	return motion_trigger_on;
}

void LightController_SetMotionOnTime(uint32_t seconds)
{
	motion_trigger_on_time_ticks = seconds * portTICK_RATE_MS * 1000;
}

uint32_t LightController_GetMotionOnTime(void)
{
	return (motion_trigger_on_time_ticks / (portTICK_RATE_MS * 1000));
}

void LightController_MotionDetected(void)
{
	last_trigger_time_ticks = xTaskGetTickCount();
}