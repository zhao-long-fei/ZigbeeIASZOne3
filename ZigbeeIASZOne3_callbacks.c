/***************************************************************************//**
 * @file
 * @brief Callback implementation for ZigbeeMinimal sample application.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "em_iadc.h"
#include "app/framework/plugin/ias-zone-server/ias-zone-server.h"
#include "function_control.h"
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL


EmberEventControl customInterruptEventControl;
EmberEventControl IADCCollectEventControl;



#define RISING_EDGE_STATUS      0
#define FALLING_EDGE_STATUS     1
#define DelayTimer              20*1000

// Set CLK_ADC to 10kHz (this corresponds to a sample rate of 1ksps)
#define CLK_SRC_IADC_FREQ       1000000 // CLK_SRC_IADC
#define CLK_ADC_FREQ            10000   // CLK_ADC
// When changing GPIO port/pins above, make sure to change xBUSALLOC macro's
// accordingly.
#define IADC_INPUT_BUS          CDBUSALLOC
#define IADC_INPUT_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0

// Stores latest ADC sample and converts to volts
static volatile IADC_Result_t sample;
static volatile double singleResult;

uint8_t  mode;
uint8_t  EdgeTrigger;
bool HavePeople;
uint8_t networkJoinAttempts = 0;          //入网尝试次数
uint16_t ZoneStatus;


void IadcInitialize(void);
/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
  // This value is ignored by the framework.
  if(status == EMBER_NETWORK_UP) {                                        //入网成功，要告诉MCU.
    halClearLed(0);
    networkJoinAttempts = 0  ;                                            //入网成功后尝试次数清零。
    emberEventControlSetInactive(emberAfLedJoinNetworkStatusEventControl);
  }
  return false;
}

/** @brief Complete
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  emberAfCorePrintln("%p network %p: 0x%X", "Join", "complete", status);
}
void emberAfMainTickCallback(void)
{
  Serial_Recv_Data();
}


/** @brief Ok To Sleep
 *
 * This function is called by the Idle/Sleep plugin before sleeping. It is
 * called with interrupts disabled. The application should return true if the
 * device may sleep or false otherwise.
 *
 * @param durationMs The maximum duration in milliseconds that the device will
 * sleep. Ver.: always
 */
bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs)
{
  /*关闭看门狗，去睡觉*/
  halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);

  /*开启串口中断*/
  GPIO_ExtIntConfig(gpioPortB,          //GPIO_Port_TypeDef port
                         1,             //unsigned int pin
                         0,             //unsigned int intNo
                         true,         //bool risingEdge
                         true,          //bool fallingEdge
                         true);
  //NVIC_EnableIRQ(USART1_RX_IRQn);
  return true;
}


/** @brief Wake Up
 *
 * This function is called by the Idle/Sleep plugin after sleeping.
 *
 * @param durationMs The duration in milliseconds that the device slept.
 * Ver.: always
 */
void emberAfPluginIdleSleepWakeUpCallback(uint32_t durationMs)
{
  /**/
  /*醒来，开启看门狗*/
  halInternalEnableWatchDog();

  /*关闭串口中断，进行协议交换*/
  GPIO_ExtIntConfig(gpioPortB,          //GPIO_Port_TypeDef port
                         1,             //unsigned int pin
                         0,             //unsigned int intNo
                         true,         //bool risingEdge
                         true,          //bool fallingEdge
                         false);
  //NVIC_DisableIRQ(USART1_RX_IRQn);

}
/** @brief Called whenever the GPIO sensor detects a change in state.
 *
 * @param newSensorState The new state of the sensor based alarm
 * (EMBER_AF_PLUGIN_GPIO_SENSOR_ACTIVE or
 * EMBER_AF_PLUGIN_GPIO_SENSOR_NOT_ACTIVE)  Ver.: always
 */
void emberAfPluginGpioSensorStateChangedCallback(uint8_t newSensorState)
{
  //如果采用串口通信这段代码要注释掉。

  emberAfCorePrintln("emberAfPluginGpioSensorStateChangedCallback.%d",newSensorState);
#if 0  
  uint8_t state = 0;
  uint8_t data[2] = {0,0};
  uint8_t ieeeAddress[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  //IADC_command(IADC0, iadcCmdStartSingle);
  emberAfReadServerAttribute(1,
                             ZCL_IAS_ZONE_CLUSTER_ID,
                             ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID,
                             (uint8_t*)ieeeAddress,
                             8);
  emberAfReadServerAttribute(1,
                             ZCL_IAS_ZONE_CLUSTER_ID,
                             ZCL_ZONE_STATE_ATTRIBUTE_ID,
                             (uint8_t*)&state,
                             8);
#if 0
  for (int i = 0; i < 8; i++) {
    if (ieeeAddress[i] != 0) {
        emberAfCorePrintln("addr%d=%4x",i,ieeeAddress[i]);
    }
  }
#endif
  emberAfCorePrintln("state=%x ",state);
  emberAfReadServerAttribute(1,
                             ZCL_IAS_ZONE_CLUSTER_ID,
                             ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                             (uint8_t*)data,
                             8);
  EdgeTrigger = newSensorState;                                   //0:RISING_EDGE_STATUS  1：FALLING_EDGE_STATUS
  if (RISING_EDGE_STATUS==newSensorState) {                       //上升沿触发 检测到有人
      data[0] |= 0x01;                                            //alarm
      emberAfWriteServerAttribute(1,
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);
      ZoneStatus = ((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00);
      if((1==mode)&&(false==HavePeople)){//正常模式
                                         //检测到人离开才去发指令
          emberEventControlSetDelayMS(SentZoneStatusEventControl,0);
      }else if(0==mode){//测试模式
          emberEventControlSetDelayMS(SentZoneStatusEventControl,0);      //检测到有人 就去下发命令
      }

  }else if(FALLING_EDGE_STATUS==newSensorState){                          //下降沿触发 检测到有人离开
      data[0] &= 0x0;                                                     //not alarm
      emberAfWriteServerAttribute(1,
                                  ZCL_IAS_ZONE_CLUSTER_ID,
                                  ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                                  (uint8_t*)data,
                                  ZCL_BITMAP16_ATTRIBUTE_TYPE);
      ZoneStatus = ((data[0]) & 0x00ff) +(data[1]<<8 & 0xff00);
      emberEventControlSetDelayMS(SentZoneStatusEventControl,DelayTimer);  //DelayTimer后上报有人离开
  }


  emberAfCorePrintln("data[0]=%x data[1]=%x data16_t=%x ",data[0],data[1],ZoneStatus);
#endif
}

void SentZoneStatusEventHandler(void)
{
  emberEventControlSetInactive(SentZoneStatusEventControl);
  emberAfFillCommandIasZoneClusterZoneStatusChangeNotification(
                                                                ZoneStatus,
                                                                0, // extended status, must be zero per spec
                                                                emberAfPluginIasZoneServerGetZoneId(1),
                                                                0); // called "delay" in the spec
  emberAfSetCommandEndpoints(1, 1);
  emberAfSendCommandUnicastToBindings();

#if 1
   if(EdgeTrigger == FALLING_EDGE_STATUS){
       HavePeople = false;  //人已经离开
   }else if(EdgeTrigger == RISING_EDGE_STATUS){
       HavePeople = true;   //检测到有人
   }
#endif
}

void emberAfMainInitCallback(void)
{

  GPIO_PinModeSet(GPIO_INTERRUPT_PORT, GPIO_INTERRUPT_PIN,gpioModeInputPull,1); //1:DOUT  1:默认高电平  0：默认低电平
  GPIO_PinModeSet(GPIO_OUT_PORT,GPIO_OUT_PIN,gpioModePushPull,1);               //叫醒设备 20ms
  /*设置串口中断*/
  GPIO_PinModeSet(gpioPortB, 1,gpioModeInputPull,0);
  GPIO_ExtIntConfig(gpioPortB,          //GPIO_Port_TypeDef port
                         1,             //unsigned int pin
                         0,             //unsigned int intNo
                         true,          //bool risingEdge
                         true,          //bool fallingEdge
                         true);         //bool enable
  GPIOINT_Init();
  CMU_ClockEnable(cmuClock_GPIO, true);
  IadcInitialize();
  emberEventControlSetDelayMS(IADCCollectEventControl,5000);
  HavePeople=false;   //上电初始化为无人

  data_initialize();  //串口接收相关数据初始化
}

void IadcInitialize(void)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;


  // Reset IADC to reset configuration in case it has been modified
  IADC_reset(IADC0);
  // Configure IADC clock source for use while in EM2
  // Note that HFRCOEM23 is the lowest frequency source available for the IADC
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_HFRCOEM23);

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;
  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0,CLK_SRC_IADC_FREQ, 0);

  // Configuration 0 is used by both scan and single conversions by default
  // Use unbuffered AVDD as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                     CLK_ADC_FREQ,
                                                                     0,
                                                                     iadcCfgModeNormal,
                                                                     init.srcClkPrescale);
  // Single initialization
  initSingle.dataValidLevel = _IADC_SCANFIFOCFG_DVL_VALID1;
  // Set conversions to run one,continuously
  initSingle.triggerAction = iadcTriggerActionOnce;
  // Configure Input sources for single ended conversion
  initSingleInput.posInput = iadcPosInputPortCPin4;
  initSingleInput.negInput = iadcNegInputGnd;

  // Initialize the ADC peripheral
  IADC_init(IADC0, &init, &initAllConfigs);
  // Initialize Scan
  IADC_initSingle(IADC0, &initSingle,&initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_BUS |= IADC_INPUT_BUSALLOC;

  // Enable interrupts on data valid level
  IADC_enableInt(IADC0, IADC_IF_SINGLEFIFODVL);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  uint8_t voltage;

  // Read data from the FIFO
  sample = IADC_pullSingleFifoResult(IADC0);
  emberAfCorePrintln("sample.data=%d ",sample.data);
  // For single-ended the result range is 0 to +Vref, i.e., 12 bits for the
  // conversion value.
  singleResult = sample.data * 3.3 / 0xFFF;
  emberAfCorePrintln("singleResult=%d ",(int)singleResult*10);
  //emberAfCorePrintln("singleResult1=%e ",singleResult);
  IADC_clearInt(IADC0, IADC_IF_SINGLEFIFODVL);
  voltage = sample.data*200/0xFFF;
  emberAfWriteServerAttribute(1,
                             ZCL_POWER_CONFIG_CLUSTER_ID,
                             ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID,
                             (uint8_t*)&voltage,
                             ZCL_DATA8_ATTRIBUTE_TYPE);
}

void IADCCollectEventHandler(void)
{
  emberEventControlSetInactive(IADCCollectEventControl);
  IADC_command(IADC0, iadcCmdStartSingle);
  emberEventControlSetDelayMS(IADCCollectEventControl,50000); //先取消电量的周期性上报。
}

















/** @brief Data Ready
 *
 * This function is called whenever the battery monitor has generated a new
 * valid battery level.
 *
 * @param batteryVoltageMilliV The battery voltage, in milli Volts  Ver.: always
 */
/*
void emberAfPluginBatteryMonitorDataReadyCallback(uint16_t batteryVoltageMilliV)
{
  emberAfCorePrintln("----------------------------emberAfPluginBatteryMonitorDataReadyCallback---------------------------- %d",batteryVoltageMilliV);

}

*/
