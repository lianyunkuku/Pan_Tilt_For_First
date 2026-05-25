#ifndef __BMI088TASK_H_
#define __BMI088TASK_H_

#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"
#include "BMI088Middleware.h"
#include "BMI088driver.h"
#include "BMI088reg.h"
#include "ist8310driver.h"
#include "ist8310driver_middleware.h"
#include "AHRS.h"
typedef struct angle_data_t {
	bmi088_real_data_t BMIdata;
	ist8310_real_data_t ISTdata;
	fp32 pitch,roll,yaw;
	fp32 quta[4];
}angle_data_t;

void Bmi088Task(void const * argument);
	

#endif
