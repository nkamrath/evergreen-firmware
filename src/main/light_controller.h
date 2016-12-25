#ifndef LIGHT_CONTROLLER_H
#define LIGHT_CONTROLLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void LightController_Refresh(void);
void LightController_On(void);
void LightController_Off(void);
void LightController_MotionTriggerOn(void);
void LightController_MotionTriggerOff(void);
void LightController_SetMotionOnTime(uint32_t seconds);
void LightController_MotionDetected(void);


#endif