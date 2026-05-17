#include "can_motor.h"

#define get_motor_measure(ptr, data)                                    \
    {                                                                   \
        (ptr)->last_ecd = (ptr)->ecd;                                   \
        (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);            \
        (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);      \
        (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);  \
        (ptr)->temperate = (data)[6];                             			\
			  (ptr)->state = (data)[7];                                       \
    }

		
		
		

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

static uint8_t roll_can_send_data[8];
static uint8_t yaw_can_send_data[8];
static CAN_TxHeaderTypeDef ROLL_TX;
static CAN_TxHeaderTypeDef YAW_TX;

extern uint16_t gimbal_state;		
extern fp32 lock_angle[2];		
extern volatile fp32 now_angle[2];		
motor_data_rx_t roll_data,yaw_data;
pid_type_def Roll_ppid,Roll_spid,Yaw_ppid,Yaw_spid;
fp32 roll_pk[3]={80.0f,0.10f,200.0f},
		 roll_sk[3]={900.0f,0.007f,700.0f},
		 yaw_pk[3]={52.0f,0.025f,0.7f},
		 yaw_sk[3]={900.0f,0.150f,1.7f};
		
int16_t get_current[2];	
					 
void motorData_Init(motor_data_rx_t *ptr){
	ptr->ecd=0;
	ptr->last_ecd=0;
	ptr->given_current=0;
	ptr->speed_rpm=0;
	ptr->state=MOTOR_OFFLINE;
}		

void CAN_send_CMD(uint32_t MOTOR_ID,CAN_TxHeaderTypeDef *TX,uint8_t *tx_buf,int16_t angle){
	uint32_t can_mailbox;
	TX->StdId=MOTOR_ID;
	TX->IDE=CAN_ID_STD;
	TX->RTR=CAN_RTR_DATA;
	TX->DLC=8;
	tx_buf[0]=(angle>>8);
	tx_buf[1]=angle;
	tx_buf[2]=(angle>>8);
	tx_buf[3]=angle;
	tx_buf[4]=(angle>>8);
	tx_buf[5]=angle;
	tx_buf[6]=(angle>>8);
	tx_buf[7]=angle;
	HAL_CAN_AddTxMessage(&hcan1,TX,tx_buf,&can_mailbox);
}

void MotorCheck(void){
	  uint32_t timestamp;
		timestamp=HAL_GetTick();
		if(timestamp-roll_data.time_us>=30){
			roll_data.state=MOTOR_OFFLINE;
		}
		if(timestamp-yaw_data.time_us>=30){
			yaw_data.state=MOTOR_OFFLINE;
		}
		if(roll_data.state!=MOTOR_NO_ERROR||yaw_data.state!=MOTOR_NO_ERROR){
			gimbal_state=GIMBAL_SAFE;
		}
}
	
void usbTask01(void const * argument)
{
	uint8_t tx_buf[20]={0};
	uint32_t temp=0;
	tx_buf[16]=0x00;
	tx_buf[17]=0x00;
	tx_buf[18]=0x80;
	tx_buf[19]=0x7f;
  for(;;)
  {	
		*(float *)&tx_buf[0]=lock_angle[0]*180.0f/PI;
		*(float *)&tx_buf[4]=lock_angle[1]*180.0f/PI;
		*(float *)&tx_buf[8]=now_angle[0]*180.0f/PI;
		*(float *)&tx_buf[12]=now_angle[1]*180.0f/PI;
		usb_data_send(tx_buf,20);
    osDelay(1);
  }

}


void PIDcalcTask03(void const * argument)
{	
	fp32 set_speed[2],now_speed[2];
	PID_init(&Roll_ppid,PID_POSITION,roll_pk,560,180);
	PID_init(&Roll_spid,PID_POSITION,roll_sk,11000,3000);
	PID_init(&Yaw_ppid,PID_POSITION,yaw_pk,15,5);
	PID_init(&Yaw_spid,PID_POSITION,yaw_sk,25000,15000);
  for(;;)
  {
		switch(gimbal_state){
			case GIMBAL_SAFE:
				break;
			case GIMBAL_DEBUG:
				set_speed[0]=PID_calc(&Roll_ppid,now_angle[0],lock_angle[0]);
			  set_speed[1]=PID_calc(&Yaw_ppid,now_angle[1],lock_angle[1]);
				now_speed[0]=roll_data.speed_rpm/60.0f*2.0f*PI;
				now_speed[1]=yaw_data.speed_rpm/60.0f*2.0f*PI;
				get_current[0]=PID_calc(&Roll_spid,now_speed[0],set_speed[0]);
			  get_current[1]=PID_calc(&Yaw_spid,now_speed[1],set_speed[1]);
				break;
			case GIMBAL_AUTO:
				break;
		}
    vTaskDelay(1);
  }

}

void can_motorTask04(void const * argument)
{
	
  can_filter_init();
	motorData_Init(&roll_data);
	motorData_Init(&yaw_data);
	
  for(;;)
  {	
		MotorCheck();
		switch(gimbal_state){
			case GIMBAL_SAFE:
				break;
			case GIMBAL_DEBUG:
				CAN_send_CMD(ROLL_MOTOR_TX,&ROLL_TX,roll_can_send_data,(int16_t)get_current[0]);
			  CAN_send_CMD(YAW_MOTOR_TX,&YAW_TX,yaw_can_send_data,(int16_t)get_current[1]);
				break;
			case GIMBAL_AUTO:
				break;
		}	
		vTaskDelay(1);
  }

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);

    switch (rx_header.StdId)
    {
        case ROLL_MOTOR_ID:
        {
            get_motor_measure(&roll_data, rx_data);
						roll_data.time_us=HAL_GetTick();
            break;
        }
				case YAW_MOTOR_ID_DEFAULT:
        {
            get_motor_measure(&yaw_data, rx_data);
						yaw_data.state=0;
					  yaw_data.time_us=HAL_GetTick();
            break;
        }
        default:
        {
            break;
        }
     }
}