#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#include "GPIO.h"
#include "Clock.h"
#include "Timer.h"
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void Error_Handler(void);



#ifdef __cplusplus
}
#endif

#endif 
