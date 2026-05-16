#include "bmi088task.h"
#include "math.h"


/*angle_data_t real_angle;

float yaw=0,roll=0,pitch=0;
uint8_t error;
bool_t upd=0;
void Bmi088Task(void const * argument){

	MX_USB_DEVICE_Init();
	real_angle.pitch=0,real_angle.roll=0,real_angle.yaw=0;
	error=BMI088_init();
	while(error!=BMI088_NO_ERROR){
		error=BMI088_init();
	}
	error=ist8310_init();
	while(error!=IST8310_NO_ERROR){
		error=ist8310_init();
	}
	BMI088_read(&real_angle.BMIdata.gyro[0],&real_angle.BMIdata.accel[0],&real_angle.BMIdata.temp);
	for(int i=0;i<10;i++){
	ist8310_read_mag(&real_angle.ISTdata.mag[0]);
	}
	AHRS_init(&real_angle.quta[0],&real_angle.BMIdata.accel[0],&real_angle.ISTdata.mag[0]);
	while(1){
		BMI088_read(&real_angle.BMIdata.gyro[0],&real_angle.BMIdata.accel[0],&real_angle.BMIdata.temp);
		ist8310_read_mag(&real_angle.ISTdata.mag[0]);
		upd=AHRS_update(&real_angle.quta[0],0.001f,&real_angle.BMIdata.gyro[0],&real_angle.BMIdata.accel[0],&real_angle.ISTdata.mag[0]);
		get_angle(&real_angle.quta[0],&real_angle.yaw,&real_angle.pitch,&real_angle.roll);
		real_angle.yaw*=(180/PI);
		real_angle.roll*=(180/PI);
		real_angle.pitch*=(180/PI);
		vTaskDelay(1);
	}
}
*/