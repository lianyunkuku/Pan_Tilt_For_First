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

motor_data_rx_t roll_data,yaw_data;

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
		
void can_motorTask04(void const * argument)
{
  can_filter_init();
	motorData_Init(&roll_data);
	motorData_Init(&yaw_data);
  for(;;)
  {
		
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
					roll_data.time_us=HAL_GetTick();
            break;
        }
        default:
        {
            break;
        }
     }
}