#ifndef __CAN_MOTOR_H_
#define __CAN_MOTOR_H_

#include "main.h"
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"
#include "freertos.h"
#include "bsp_can.h"

#define ROLL_MOTOR_ID 0x201
#define ROLL_MOTOR_TX 0x200
#define YAW_MOTOR_ID_DEFAULT 0x205
#define YAW_MOTOR_TX 0x1FE
#define MOTOR_OFFLINE 0xff
enum GIMBAL_STATE{
	GIMBAL_SAFE=0x11,
	GIMBAL_DEBUG=0x45,
	GIMBAL_AUTO=0x14
};


enum MOTOR_STATE{
	MOTOR_NO_ERROR=0,
	MOTOR_FLASH_ERROR=1,
	MOTOR_VOLT_EXCEED=2,
	MOTOR_OUT_CONTROL=3,
	MOTOR_MISS_SENSOR=4,
	MOTOR_TEMP_ABNORMAL=5,
	MOTOR_STUCK=6,
	MOTOR_CALIBRATE_FAIL=7,
	MOTOR_OVBERHEAT=8
};

typedef struct motor_data_rx{
	  uint16_t ecd;
    int16_t speed_rpm;
    int16_t given_current;
    uint8_t temperate;
    int16_t last_ecd;
	  uint8_t state;
		uint32_t time_us;
}motor_data_rx_t;

void can_motorTask04(void const * argument);

#endif