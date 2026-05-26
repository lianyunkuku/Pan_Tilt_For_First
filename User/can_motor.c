#include "can_motor.h"

#define get_motor_measure(ptr, data)                                    \
    {                                                                   \
			  (ptr)->last_rpm = (ptr)->speed_rpm;                             \
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
fp32 roll_pk[3]={80.0f,0.161f,30.0f},
		 roll_sk[3]={700.0f,0.005f,800.0f},
		 yaw_pk[3]={90.0f,0.025f,7.0f},
		 yaw_sk[3]={800.0f,0.150f,170.0f};
		
int16_t get_current[2];	
bool_t IF_SWITCHED=0;
		 
void motorData_Init(motor_data_rx_t *ptr){
	ptr->ecd=0;
	ptr->last_ecd=0;
	ptr->last_rpm=0;
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

void PID_clear_t(void){
	PID_clear(&Roll_ppid);
	PID_clear(&Yaw_ppid);
	PID_clear(&Roll_spid);
	PID_clear(&Yaw_spid);
}

fp32 sin_wave(void){
	static fp32 t = 0.0f;
	const fp32 dt = 0.001f;
	const fp32 omega = 4.0f*PI;
	const float amp = 0.2f;
	fp32 val=amp*sinf(omega*t);
	t+=dt;
	if(t>=0.5f){
		t=0.0f;
	}
	return val;
}

extern nuc_receive_data_t	nuc_receive_data;
extern cmd_id_queue_t cmd_id_queue;
extern nuc_transmit_data_t 	nuc_transmit_data;

extern usb_cdc_data_t usb_cdc_data;

void usbTask01(void const * argument)
{ /*******Below is code for VOFA debug*******/
	/*uint8_t tx_buf[20]={0};
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
  */
	/*******************************************/
	
	/*****Below is for nuc contact*****/
	uint16_t nuc_off_cnt=0;
	cmd_id_init();
	cmd_id_task_create(GIMBAL_AND_CONFIG_SEND_ID,999);
	for(;;){
		
		if(usb_cdc_data.usb_cdc_rx_flag==1){
			gimbal_state=GIMBAL_AUTO;
			IF_SWITCHED=1;
			nuc_off_cnt=0;
		}
		else{
			nuc_off_cnt++;
			if(nuc_off_cnt>=1000){
				gimbal_state=GIMBAL_SAFE;
				IF_SWITCHED=1; 
				nuc_off_cnt=0;
			}
		}
		Nuc_data_unpacked();
		cmd_id_queue_handle();
		if(nuc_receive_data.aim_data_received.success==1){
			
			lock_angle[0]=nuc_receive_data.aim_data_received.pitch;
			lock_angle[1]=nuc_receive_data.aim_data_received.yaw;
			
		}
		vTaskDelay(1);
	}
	
	/**********************************/
}

	fp32 err[2],kf[2]={0,0};
void PIDcalcTask03(void const * argument)
{	
	fp32 set_speed[2],now_speed[2];

	uint8_t stuck_cnt[2]={0,0};
	PID_init(&Roll_ppid,PID_POSITION,roll_pk,560,30);
	PID_init(&Roll_spid,PID_POSITION,roll_sk,11000,3000);
	PID_init(&Yaw_ppid,PID_POSITION,yaw_pk,100,5);
	PID_init(&Yaw_spid,PID_POSITION,yaw_sk,25000,15000);
  for(;;)
  {
		switch(gimbal_state){
			case GIMBAL_SAFE:
				if(IF_SWITCHED){
					PID_clear_t();
					IF_SWITCHED=0;
				}	
				break;
			case GIMBAL_DEBUG:
			case GIMBAL_AUTO:	
				if(IF_SWITCHED){
					PID_clear_t();
					IF_SWITCHED=0;
				}	
				
				lock_angle[0]=lock_angle[1]=sin_wave();
				
				for(int i=0;i<2;i++){
					err[i]=lock_angle[i]-now_angle[i];
			    if(err[i]<=0.003f&&err[i]>=-0.003f){ 
						err[i]=0;
						kf[i]=0;
					}else if(err[i]<=0.09f&&err[i]>=-0.09f){
						kf[i]=err[i]*(-5000.0f);
					}else {
						
						kf[i]=0;
					}
				}
				if(roll_data.speed_rpm<=1&&roll_data.speed_rpm>=-1&&(roll_data.given_current>=10000||roll_data.given_current<=-10000)){
					stuck_cnt[0]++;
					if(stuck_cnt[0]>=100){
						roll_data.state=MOTOR_STUCK;
						stuck_cnt[0]=0;
						IF_SWITCHED=1;
					}
				}else{
					stuck_cnt[0]=0;
				}
				if(yaw_data.speed_rpm<=1&&yaw_data.speed_rpm>=-1&&(yaw_data.given_current>=10000||yaw_data.given_current<=-10000)){
					stuck_cnt[1]++;
					if(stuck_cnt[1]>=100){
						yaw_data.state=MOTOR_STUCK;
						stuck_cnt[1]=0;
						IF_SWITCHED=1;
					}
				}else{
					stuck_cnt[1]=0;
				}
				set_speed[0]=PID_calc(&Roll_ppid,0,err[0]);
			  set_speed[1]=PID_calc(&Yaw_ppid,0,err[1]);
				now_speed[0]=(0.7*roll_data.speed_rpm+0.3*roll_data.last_rpm)/60.0f*2.0f*PI;
				now_speed[1]=(0.7*yaw_data.speed_rpm+0.3*yaw_data.last_rpm)/60.0f*2.0f*PI;
				get_current[0]=PID_calc(&Roll_spid,now_speed[0],set_speed[0])+kf[0];
			  get_current[1]=PID_calc(&Yaw_spid,now_speed[1],set_speed[1])+kf[1];
				break;

			default:
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
