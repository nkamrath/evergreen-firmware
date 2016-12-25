#include "light_controller.h"
#include "pin.h"

bool light_state = false;
bool motion_trigger_on = true;
volatile uint32_t motion_trigger_on_time_seconds = 10;
uint32_t last_trigger_time_seconds = 0;

void LightController_Refresh(void)
{
	if(motion_trigger_on)
	{
		//figure out if timeout since last motion has occurred to determine light state
		if(((xTaskGetTickCount() / portTICK_RATE_MS) / 1000) > (last_trigger_time_seconds + motion_trigger_on_time_seconds))
		{
			light_state = false;
		}
		else
		{
			light_state = true;
		}
	}
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

void LightController_SetMotionOnTime(uint32_t seconds)
{
	motion_trigger_on_time_seconds = seconds;
}

void LightController_MotionDetected(void)
{
	last_trigger_time_seconds = (xTaskGetTickCount() / portTICK_RATE_MS) / 1000;
}