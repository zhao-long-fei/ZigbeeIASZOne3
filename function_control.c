#include "app/framework/include/af.h"
#include "function_control.h"

struct serial_command  recv_buf;
struct report_command  report_mcu;
void data_initialize(void)
{
  recv_buf.data_length = 0;                 //结构体初始化后续可以优化为宏定义
  recv_buf.temp_length = 0;
}

void Serial_Recv_Data(void)
{
  uint8_t bytes = 0;
  unsigned char * str;
  bytes = emberSerialReadAvailable(COM_USART1);
  if(bytes){
      emberSerialReadData(COM_USART1,
                          &recv_buf.cmd[recv_buf.data_length],
                          bytes,
                          NULL);
      recv_buf.data_length += bytes;

#if 1
  if(recv_buf.data_length>10) {
    uint8_t m;
    emberAfCorePrintln("bytes %x ",bytes);
    for(m=0;m<recv_buf.data_length;m++) {
        emberAfCorePrintln("recv_buf.cmd[%d]:%x ", m,recv_buf.cmd[m]);
    }
  }
#endif
     
      if(recv_buf.data_length > 2){
         str = &recv_buf.cmd[recv_buf.temp_length];
         while(recv_buf.temp_length <recv_buf.data_length){     //处理过的长度  < 接收的数据
             if(str[0] == HEAD_CODE)
             {
               if((recv_buf.temp_length+(str[1]+2)) <= recv_buf.data_length)
               {                                                //收到的剩余数据符合至少有一帧
                  emberAfCorePrintln("recv_buf->temp_length=%x recv_buf->data_length=%x",recv_buf.temp_length,recv_buf.data_length);
                  static int ret = 0 ;
                  if(MultiIRCountSumCheckCommand(str) != str[str[1]+1]){                                      //计算校验和
                      str++;                                        //中间帧错误继续往后处理。
                      recv_buf.temp_length++;                       //校验和错误
                      emberAfCorePrintln("----log4---- ");
                  } else {
                                                                    //数据帧为正确的
                      //在这里设置一个检查pid 的标志位，如果flag不满足，则去写pid.采用事件的形式
                      emberAfCorePrintln("----log3---- ");
                      ret = MultiIRDataReportProcessFunction(&report_mcu,&recv_buf,&str);

                      if (ret) {                                    //处理完毕跳出循环
                          break;
                      }
                  }

               } else {
                   break;
               }
             } else {                                           //帧头错误
               str++;
               recv_buf.temp_length++;
               emberAfCorePrintln("-----log5----- ");
             }
         }
      }

  }

}
/***************************************************************
 * @user MultiIR computes checksum commands.
 * ptr[1]:后续数据长度。ptr[1]+1:需要计算校验和的长度。
 *
 * return:Check SUM。
 *
 ***************************************************************/
int MultiIRCountSumCheckCommand(unsigned char *ptr)
{
  uint8_t i = 0;
  int SUM = 0;
  for(i=0;i<ptr[1]+1;i++)
  {
      SUM += ptr[i];
      //emberAfCorePrintln("ptr[%d] %x",i,ptr[i] );
  }
  SUM = SUM%256; 
  emberAfCorePrintln("check %x",SUM );
  return SUM;
}

uint8_t MultiIRCammandBytProcessFunction(serial_report_command *report_mcu,
                                         serial_recv_command *recv_buf,
                                         unsigned char **ptr)
{
  int offset = 0;
  offset = (*ptr)[1] +2;                                          //偏移数据的总长度
  if (recv_buf->temp_length == recv_buf->data_length) {           //数据处理完毕
      memset(report_mcu->cmd,0,sizeof(report_mcu->cmd));
      memset(recv_buf->cmd,0,sizeof(recv_buf->cmd));
      recv_buf->data_length = 0;                                  //数据清零
      recv_buf->temp_length = 0;
      //emberAfCorePrintln("11111111111111111111111111111111 %d",offset );
      return 1;
  }
  //*i += offset;                                                   //一帧处理完毕，后面还有数据未处理
  *ptr += offset;
  memset(report_mcu->cmd,0,sizeof(report_mcu->cmd));
  return 0;
}

uint8_t MultiIRDataReportProcessFunction(serial_report_command *report_process,
                                         serial_recv_command *recv_buf,
                                         unsigned char **ptr)
{
  int offset = 0;
  offset = (*ptr)[1] +2;                                          //偏移数据的总长度
  memcpy(&report_process->cmd[0],*ptr,offset);                    //上报时用report_mcu.cmd
  report_process->data_length = offset;                           //函数内这两个数的作用相同
  recv_buf->temp_length += offset;                                //记录现在处理到的长度
  
  /**这个地方要用事件设置20MS的低电平**/
  emberEventControlSetDelayMS(StartLowLevelEventControl,0);       //message to mcu. 只要有数据来就要ACK
  switch ((*ptr)[2])
  {
  case DATA_REPORT_TYPE:                                          //数据上报类型
    ReportMessageToGateway(report_process);                       //message to gateway 上报网关要区分是主动下发，还是主动上报。
    break;
  case NETWORK_TYPE:                                              //使能入网事件  入网状态再次入网为离网然后入网
    NetworkSteeringOperation(report_process);
    break;
  case LED_TYPE:                                                  //带串口的mcu 灯是有mcu控制。
    /* code */
    break;
  case ALARM_TYPE:
    /* code */
    break;
  default:
    break;
  }
  

  

  emberAfCorePrintln(" ---------------------------log1------------------------ ");
  emberAfCorePrintln("recv_buf->temp_length %x",recv_buf->temp_length);

  if (recv_buf->temp_length == recv_buf->data_length) {           //数据处理完毕
      //memset(report_process->cmd,0,sizeof(report_process->cmd));
      memset(recv_buf->cmd,0,sizeof(recv_buf->cmd));
      recv_buf->data_length = 0;
      recv_buf->temp_length = 0;

      return 1;
  }
  *ptr += offset;
  memset(report_process->cmd,0,sizeof(report_process->cmd));

  return 0;
}

void ReportMessgeToMCU(serial_report_command *report_mcu)
{
  int8u i;
  int SUM = 0;
  halInternalResetWatchDog();
  report_mcu->cmd[0] = MODULE_CMD_HEADER;                                       //改变帧头，其他数据不动
  for(i = 0;i < report_mcu->data_length-1;i++)
  {
      SUM += report_mcu->cmd[i];
  }
  report_mcu->cmd[report_mcu->data_length-1]   = SUM%256;                       //重新计算校验和
  WakeUpMessageToMCU();                                                         //发送叫醒数据
  emberSerialWriteData(COM_USART1, report_mcu->cmd, report_mcu->data_length);   //数据发送
}

void WakeUpMessageToMCU(void)
{
  static uint8_t buffer[2] = {0xAA,0xAA};

  emberSerialWriteData(COM_USART1, buffer, sizeof(buffer));   //数据发送
}

void StartLowLevelEventHandler(void)
{
  emberEventControlSetInactive(StartLowLevelEventControl);
  GPIO_PinOutClear(GPIO_OUT_PORT, GPIO_OUT_PIN);
  emberEventControlSetDelayMS(StopLowLevelEventControl,20);                     //20ms后IO置高
}

void StopLowLevelEventHandler(void)
{
  emberEventControlSetInactive(StopLowLevelEventControl);
  GPIO_PinOutSet(GPIO_OUT_PORT, GPIO_OUT_PIN);
  emberEventControlSetDelayMS(ReportMessgeToMCUEventControl,0);                 //IO置高后开始发送ACK
}

void ReportMessgeToMCUEventHandler(void)
{
  emberEventControlSetInactive(ReportMessgeToMCUEventControl);
  ReportMessgeToMCU(&report_mcu);
  
  emberAfCorePrintln(" ---------------------------log2------------------------ ");
  emberAfCorePrintln("report_mcu.data_length=%d ,report_mcu.cmd[1]=%d ",report_mcu.data_length,report_mcu.cmd[1]);
}


void ReportMessageToGateway(serial_report_command *report_message)
{
  if (report_message->cmd[8]==MotionSensor)                               //区分设备类型的原因，其它设备的函数后续也要放在这里。
  { 
    uint8_t data[2] = {0,0};
    
    if (report_message->cmd[9]&0x20)                                            //防拆报警
    {
     //对应上报状态对应位置1                                                     
      emberAfReadServerAttribute(1,                                             //读属性为取之前状态
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                              (uint8_t*)data,
                              8);    
      data[0] |= 0x04;                                                          //ZoneStatus第3位置1 报警
      ZoneStatus = (((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00)) ;      
      emberAfCorePrintln("data[0]=%x data[1] %x.",data[0],data[1]);
      
      emberAfWriteServerAttribute(1,                                            //此属性不勾选reporting.不会report，报警信息。
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE); 
      
    }else{                                                                      //取消报警
      //对应上报状态对应位置0
      emberAfReadServerAttribute(1,                                             //读属性为取之前状态
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                              (uint8_t*)data,
                              8);    
      data[0] &= 0xFB;                                                          //ZoneStatus第3位置0 取消报警
      ZoneStatus = (((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00));      
      emberAfWriteServerAttribute(1,
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);
      
    }

    if (report_message->cmd[9]&0x10)                                            //阈值报警
    {
      //对应上报状态对应位置1                                                     
      emberAfReadServerAttribute(1,                                             //读属性为取之前状态
                                ZCL_IAS_ZONE_CLUSTER_ID,
                                ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                (uint8_t*)data,
                                8);
      data[0] |= 0x01;                                                          //ZoneStatus最低位置1 报警
      ZoneStatus = (((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00));      
      emberAfCorePrintln("data[0]=%x data[1] %x.",data[0],data[1]);
      emberAfCorePrintln("cmd[9]=0x10 ZoneStatus is %2x.",ZoneStatus);
      emberAfWriteServerAttribute(1,
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);
    }else{                                                                      //取消报警
      //对应上报状态对应位置0
      emberAfReadServerAttribute(1,                                             //读属性为取之前状态
                                ZCL_IAS_ZONE_CLUSTER_ID,
                                ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                (uint8_t*)data,
                                8);
      data[0] &= 0xFE;                                                          //ZoneStatus最低位置0 取消报警
      ZoneStatus = (((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00)); 
      emberAfWriteServerAttribute(1,
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);     
    }

    if (report_message->cmd[9]&0x40)                                            //低压报警
    {
      //对应上报状态对应位置1                                                     
      emberAfReadServerAttribute(1,                                             //读属性为取之前状态
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                              (uint8_t*)data,
                              8);
      data[0] |= 0x08;                                                          //ZoneStatus第四位置1 低压报警
      ZoneStatus = (((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00));
      emberAfWriteServerAttribute(1,
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);     
    }else{                                                                      //取消报警
      //对应上报状态对应位置0
      emberAfReadServerAttribute(1,                                             //读属性为取之前状态
                              ZCL_IAS_ZONE_CLUSTER_ID,
                              ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                              (uint8_t*)data,
                              8);
      data[0] &= 0xF7;                                                          //ZoneStatus第四位置0 取消报警
      ZoneStatus = (((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00));    
      emberAfWriteServerAttribute(1,
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);  
    }
    emberEventControlSetDelayMS(SentZoneStatusEventControl,0);                  //开始发送命令EVENT
    //麦乐克的muc协议不存在上报电压的百分比
    if (report_message->cmd[9]&0x04)                                            //有电压信息
    {
      uint8_t voltage;
      voltage = report_message->cmd[10];                                        //report_message->cmd[10] 数据2
      emberAfWriteServerAttribute(1,
                              ZCL_POWER_CONFIG_CLUSTER_ID,
                              ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID,
                              (uint8_t*)&voltage,
                              ZCL_DATA8_ATTRIBUTE_TYPE);
    } else {                                                                    //无电压信息
      /*什么都不做*/
    }
  }
//后续不同设备需要上报网关在下面添加上报网关的信息命令

  
/*
  if ((report_message->cmd[8]==MotionSensor)&&(report_message->cmd[9]&0x20)) //人体移动 移动防拆报警
  { //对应上报状态对应位置1
    uint8_t data[2] = {0,0};
    emberAfReadServerAttribute(1,
                            ZCL_IAS_ZONE_CLUSTER_ID,
                            ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                            (uint8_t*)data,
                            8);    
    ZoneStatus = (((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00)) || 0x0001;      //ZoneStatus最低位置1 报警
    emberEventControlSetDelayMS(SentZoneStatusEventControl,0); 
     //报警后什么时间关闭报警，是测试模式还是正常模式该怎么设计， 暂时未定
  }
  if ((report_message->cmd[8]==MotionSensor)&&(report_message->cmd[9]&0x04)) //人体移动 有电压信息 数据2表示电压百分比
  {
    uint8_t voltage;
    voltage = report_message->cmd[10];                                         //report_message->cmd[10] 数据2
    emberAfWriteServerAttribute(1,
                            ZCL_POWER_CONFIG_CLUSTER_ID,
                            ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID,
                            (uint8_t*)&voltage,
                            ZCL_DATA8_ATTRIBUTE_TYPE);
  }
*/

}

void NetworkSteeringOperation(serial_report_command *report_message)
{
  if (report_message->cmd[8]==MotionSensor)
  {

    if (report_message->cmd[9]==0x02)                                           //按键入网                                   
    {
      if(emberAfNetworkState() == EMBER_JOINED_NETWORK) {                       //在网状态
          emberLeaveNetwork(); //离开网络
      } 

      emberEventControlSetActive(emberAfKeepConnectEventControl);//开启入网事件
      emberEventControlSetActive(emberAfLedJoinNetworkStatusEventControl);//入网灯闪烁事件

 
    }else{                                                                     

    }

  }

}

void emberAfKeepConnectEventControlHandler(void)
{
  
  emberEventControlSetInactive(emberAfKeepConnectEventControl);
  if (networkJoinAttempts < REJOIN_ATTEMPTS)                   //重复入网3次
  {
      networkJoinAttempts++;
      emberAfPluginNetworkSteeringStart();
      emberEventControlSetDelayQS(emberAfKeepConnectEventControl,QS_BETWEEN_JOIN_ATTEMPTS);
  } else {                                      //大于3次入网失败，关绿灯        
      networkJoinAttempts =0;
      halClearLed(0);
      emberEventControlSetInactive(emberAfLedJoinNetworkStatusEventControl);
  }

}

void emberAfLedJoinNetworkStatusEventHandler(void)
{
  emberEventControlSetInactive(emberAfLedJoinNetworkStatusEventControl);
  halToggleLed(0);
  emberEventControlSetDelayMS(emberAfLedJoinNetworkStatusEventControl,500);

}
