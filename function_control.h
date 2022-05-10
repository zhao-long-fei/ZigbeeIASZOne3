#ifndef function_control__H
#define function_control__H
#include "app/framework/include/af.h"

#define RS485_CMD_LENGTH            128      //串口命令最大长度
#define HEAD_CODE                   0xFA     //主板-->模组   0XFA

#define DATA_REPORT_TYPE            0x01
#define NETWORK_TYPE                0x02
#define LED_TYPE                    0x03
#define ALARM_TYPE                  0x04
#define INFORM_TYPE                 0x0A

#define MODULE_CMD_HEADER           0xF5
#define MCU_HEADER                  0xFA

#define MotionSensor                0x21    //人体移动探测器device


#define GPIO_INTERRUPT_PORT gpioPortA                                           //唤醒输入
#define GPIO_INTERRUPT_PIN  (4U)
#define GPIO_OUT_PORT       gpioPortD                                           //唤醒输出
#define GPIO_OUT_PIN        (4U)

//入网配网相关定义。
#define SECONDS_BETWEEN_JOIN_ATTEMPTS 20
#define QS_BETWEEN_JOIN_ATTEMPTS      (SECONDS_BETWEEN_JOIN_ATTEMPTS * 4)
#define REJOIN_ATTEMPTS  3                         //重复入网次数



typedef struct serial_command{
  uint8_t data_length;        //数据长度
  uint8_t temp_length;        //数据当前处理的位置
  uint8_t data_pos;
  unsigned char cmd[RS485_CMD_LENGTH];
}serial_recv_command;

typedef struct report_command{
  uint8_t data_length;
  unsigned char cmd[RS485_CMD_LENGTH];
}serial_report_command;


void data_initialize(void);
void Serial_Recv_Data(void);
int MultiIRCountSumCheckCommand(unsigned char *ptr);
uint8_t MultiIRCammandBytProcessFunction(serial_report_command *report_mcu,serial_recv_command *recv_buf,unsigned char **ptr);
uint8_t MultiIRDataReportProcessFunction(serial_report_command *report_mcu,serial_recv_command *recv_buf,unsigned char **ptr);
void ReportMessgeToMCU(serial_report_command *report_mcu);
void WakeUpMessageToMCU(void);
void ReportMessageToGateway(serial_report_command *report_message);
void NetworkSteeringOperation(serial_report_command *report_message);

EmberEventControl StartLowLevelEventControl;
EmberEventControl StopLowLevelEventControl;
EmberEventControl ReportMessgeToMCUEventControl;
EmberEventControl SentZoneStatusEventControl;
EmberEventControl emberAfKeepConnectEventControl;
EmberEventControl emberAfLedJoinNetworkStatusEventControl;

extern uint16_t ZoneStatus;
extern uint8_t networkJoinAttempts;

#endif
