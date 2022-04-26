// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef SILABS_AF_ENDPOINT_CONFIG
#define SILABS_AF_ENDPOINT_CONFIG


// Fixed number of defined endpoints
#define FIXED_ENDPOINT_COUNT (1)


// Generated defaults
#if BIGENDIAN_CPU
#define GENERATED_DEFAULTS { \
0x00, 0x00, 0x38, 0x40 /* 0,Default value: Poll Control,check-in interval */, \
0x00, 0x00, 0x00, 0x24 /* 4,Default value: Poll Control,long poll interval */, \
  }
#else // ! BIGENDIAN_CPU
#define GENERATED_DEFAULTS { \
0x40, 0x38, 0x00, 0x00 /* 0,Default value: Poll Control,check-in interval */, \
0x24, 0x00, 0x00, 0x00 /* 4,Default value: Poll Control,long poll interval */, \
  }
#endif // BIGENDIAN_CPU




// Generated attributes
#define GENERATED_ATTRIBUTES { \
    { 0x0000, ZCL_INT8U_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)0x08 } }, /* 0 / Basic / ZCL version*/\
    { 0x0007, ZCL_ENUM8_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)0x00 } }, /* 1 / Basic / power source*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)3 } }, /* 2 / Basic / cluster revision*/\
    { 0x0021, ZCL_INT8U_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)0x00 } }, /* 3 / Power Configuration / battery percentage remaining*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)2 } }, /* 4 / Power Configuration / cluster revision*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_CLIENT), { (uint8_t*)2 } }, /* 5 / Identify / cluster revision*/\
    { 0x0000, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_WRITABLE), { (uint8_t*)0x0000 } }, /* 6 / Identify / identify time*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)2 } }, /* 7 / Identify / cluster revision*/\
    { 0x0000, ZCL_INT32U_ATTRIBUTE_TYPE, 4, (ATTRIBUTE_MASK_WRITABLE), { (uint8_t*)&(generatedDefaults[0]) } }, /* 8 / Poll Control / check-in interval*/\
    { 0x0001, ZCL_INT32U_ATTRIBUTE_TYPE, 4, (0x00), { (uint8_t*)&(generatedDefaults[4]) } }, /* 9 / Poll Control / long poll interval*/\
    { 0x0002, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)0x0002 } }, /* 10 / Poll Control / short poll interval*/\
    { 0x0003, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_WRITABLE), { (uint8_t*)0x0028 } }, /* 11 / Poll Control / fast poll timeout*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)3 } }, /* 12 / Poll Control / cluster revision*/\
    { 0x0000, ZCL_ENUM8_ATTRIBUTE_TYPE, 1, (0x00), { (uint8_t*)0x00 } }, /* 13 / IAS Zone / zone state*/\
    { 0x0001, ZCL_ENUM16_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)0x000D } }, /* 14 / IAS Zone / zone type*/\
    { 0x0002, ZCL_BITMAP16_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)0x0000 } }, /* 15 / IAS Zone / zone status*/\
    { 0x0010, ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE, 8, (ATTRIBUTE_MASK_WRITABLE), { NULL } }, /* 16 / IAS Zone / IAS CIE address*/\
    { 0x0011, ZCL_INT8U_ATTRIBUTE_TYPE, 1, (0x00), { (uint8_t*)0xFF } }, /* 17 / IAS Zone / Zone ID*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)2 } }, /* 18 / IAS Zone / cluster revision*/\
  }


// Cluster function static arrays
#define GENERATED_FUNCTION_ARRAYS \
const EmberAfGenericClusterFunction emberAfFuncArrayPollControlClusterServer[] = { (EmberAfGenericClusterFunction)emberAfPollControlClusterServerInitCallback,(EmberAfGenericClusterFunction)emberAfPollControlClusterServerAttributeChangedCallback,(EmberAfGenericClusterFunction)emberAfPollControlClusterServerPreAttributeChangedCallback}; \
const EmberAfGenericClusterFunction emberAfFuncArrayIasZoneClusterServer[] = { (EmberAfGenericClusterFunction)emberAfIasZoneClusterServerInitCallback,(EmberAfGenericClusterFunction)emberAfIasZoneClusterServerMessageSentCallback,(EmberAfGenericClusterFunction)emberAfIasZoneClusterServerPreAttributeChangedCallback}; \


// Clusters definitions
#define GENERATED_CLUSTERS { \
    { 0x0000, (EmberAfAttributeMetadata*)&(generatedAttributes[0]), 3, 0, (CLUSTER_MASK_SERVER), NULL,  },    \
    { 0x0001, (EmberAfAttributeMetadata*)&(generatedAttributes[3]), 2, 0, (CLUSTER_MASK_SERVER), NULL,  },    \
    { 0x0003, (EmberAfAttributeMetadata*)&(generatedAttributes[5]), 1, 2, (CLUSTER_MASK_CLIENT), NULL,  },    \
    { 0x0003, (EmberAfAttributeMetadata*)&(generatedAttributes[6]), 2, 4, (CLUSTER_MASK_SERVER), NULL,  },    \
    { 0x0020, (EmberAfAttributeMetadata*)&(generatedAttributes[8]), 5, 14, (CLUSTER_MASK_SERVER| CLUSTER_MASK_INIT_FUNCTION| CLUSTER_MASK_ATTRIBUTE_CHANGED_FUNCTION| CLUSTER_MASK_PRE_ATTRIBUTE_CHANGED_FUNCTION), emberAfFuncArrayPollControlClusterServer, },    \
    { 0x0500, (EmberAfAttributeMetadata*)&(generatedAttributes[13]), 6, 16, (CLUSTER_MASK_SERVER| CLUSTER_MASK_INIT_FUNCTION| CLUSTER_MASK_MESSAGE_SENT_FUNCTION| CLUSTER_MASK_PRE_ATTRIBUTE_CHANGED_FUNCTION), emberAfFuncArrayIasZoneClusterServer, },    \
  }


// Endpoint types
#define GENERATED_ENDPOINT_TYPES {        \
    { (EmberAfCluster*)&(generatedClusters[0]), 6, 36 }, \
  }


// Cluster manufacturer codes
#define GENERATED_CLUSTER_MANUFACTURER_CODES {      \
{0x00, 0x00} \
  }
#define GENERATED_CLUSTER_MANUFACTURER_CODE_COUNT (0)

// Attribute manufacturer codes
#define GENERATED_ATTRIBUTE_MANUFACTURER_CODES {      \
{0x00, 0x00} \
  }
#define GENERATED_ATTRIBUTE_MANUFACTURER_CODE_COUNT (0)


// Largest attribute size is needed for various buffers
#define ATTRIBUTE_LARGEST (8)
// Total size of singleton attributes
#define ATTRIBUTE_SINGLETONS_SIZE (7)

// Total size of attribute storage
#define ATTRIBUTE_MAX_SIZE 36

// Array of endpoints that are supported
#define FIXED_ENDPOINT_ARRAY { 1 }

// Array of profile ids
#define FIXED_PROFILE_IDS { 260 }

// Array of device ids
#define FIXED_DEVICE_IDS { 1026 }

// Array of device versions
#define FIXED_DEVICE_VERSIONS { 1 }

// Array of endpoint types supported on each endpoint
#define FIXED_ENDPOINT_TYPES { 0 }

// Array of networks supported on each endpoint
#define FIXED_NETWORKS { 0 }


#define EMBER_AF_GENERATED_PLUGIN_INIT_FUNCTION_DECLARATIONS \
  void emberAfPluginGpioSensorInitCallback(void); \
  void emberAfPluginIdleSleepInitCallback(void); \
  void emberAfPluginCountersInitCallback(void); \


#define EMBER_AF_GENERATED_PLUGIN_INIT_FUNCTION_CALLS \
  emberAfPluginGpioSensorInitCallback(); \
  emberAfPluginIdleSleepInitCallback(); \
  emberAfPluginCountersInitCallback(); \


#define EMBER_AF_GENERATED_PLUGIN_STACK_STATUS_FUNCTION_DECLARATIONS \
  void emberAfPluginEndDeviceSupportStackStatusCallback(EmberStatus status); \
  void emberAfPluginPollControlServerStackStatusCallback(EmberStatus status); \
  void emberAfPluginNetworkSteeringStackStatusCallback(EmberStatus status); \
  void emberAfPluginIasZoneServerStackStatusCallback(EmberStatus status); \


#define EMBER_AF_GENERATED_PLUGIN_STACK_STATUS_FUNCTION_CALLS \
  emberAfPluginEndDeviceSupportStackStatusCallback(status); \
  emberAfPluginPollControlServerStackStatusCallback(status); \
  emberAfPluginNetworkSteeringStackStatusCallback(status); \
  emberAfPluginIasZoneServerStackStatusCallback(status); \


// Generated data for the command discovery
#define GENERATED_COMMANDS { \
    { 0x0003, 0x00, COMMAND_MASK_OUTGOING_CLIENT }, /* Identify / Identify */ \
    { 0x0003, 0x00, COMMAND_MASK_OUTGOING_SERVER }, /* Identify / IdentifyQueryResponse */ \
    { 0x0003, 0x01, COMMAND_MASK_OUTGOING_CLIENT }, /* Identify / IdentifyQuery */ \
    { 0x0020, 0x00, COMMAND_MASK_OUTGOING_SERVER }, /* Poll Control / CheckIn */ \
    { 0x0020, 0x00, COMMAND_MASK_INCOMING_SERVER }, /* Poll Control / CheckInResponse */ \
    { 0x0020, 0x01, COMMAND_MASK_INCOMING_SERVER }, /* Poll Control / FastPollStop */ \
    { 0x0020, 0x02, COMMAND_MASK_INCOMING_SERVER }, /* Poll Control / SetLongPollInterval */ \
    { 0x0020, 0x03, COMMAND_MASK_INCOMING_SERVER }, /* Poll Control / SetShortPollInterval */ \
    { 0x0500, 0x00, COMMAND_MASK_OUTGOING_SERVER }, /* IAS Zone / ZoneStatusChangeNotification */ \
    { 0x0500, 0x00, COMMAND_MASK_INCOMING_SERVER }, /* IAS Zone / ZoneEnrollResponse */ \
    { 0x0500, 0x01, COMMAND_MASK_OUTGOING_SERVER }, /* IAS Zone / ZoneEnrollRequest */ \
  }
#define EMBER_AF_GENERATED_COMMAND_COUNT (11)

// Command manufacturer codes
#define GENERATED_COMMAND_MANUFACTURER_CODES {      \
{0x00, 0x00} \
  }
#define GENERATED_COMMAND_MANUFACTURER_CODE_COUNT (0)


// Generated reporting configuration defaults
#define EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS {\
  { EMBER_ZCL_REPORTING_DIRECTION_REPORTED, 1, 0x0001, 0x0021, CLUSTER_MASK_SERVER, 0x0000, 1, 65534, 0 }, \
}
#define EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE (1)
#endif // SILABS_AF_ENDPOINT_CONFIG
