/*******************************************************************************
 * Copyright (c) 2021, Buildstorm Pvt Ltd
 *
 * Filename:     app_config.h
 *
 * Description:
 *               Contains macros for the application
 *******************************************************************************/

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#include "stdbool.h"
#include "stdint.h"
#include "lib_config.h"

//---------------------------Debug CONFIG---------------------------/
/* These are just for info they are already defined in lib_config.h
typedef enum
{
    PRINT_LEVEL_NONE = 0,
    PRINT_LEVEL_ERROR = 1,
    PRINT_LEVEL_INFO = 2,
    PRINT_LEVEL_DEBUG = 3,
    PRINT_LEVEL_MAX = 4
} logLevels_et;

#define LOG_LEVEL_MAPPING  \
    {                      \
        'n', 'e', '0', 'i', 'd' \
    }

typedef enum
{
    LIB_MODULE_MAIN_MENU,
    LIB_MODULE_SYSTEM,
    LIB_MODULE_BLE,
    LIB_MODULE_WIFI,
    LIB_MODULE_MQTT,
    LIB_MODULE_AWS,
    LIB_MODULE_JOBS,
    LIB_MODULE_GPIO,
    LIB_MODULE_FLASH,
    DRV_MODULE_BLE,
    LIB_MODULE_JSON,
    LIB_MODULE_HTTP,
    LIB_MODULE_OTA,
    LIB_MODULE_UART,
    LIB_MODULE_MAX
} menusLibModule_et;
*/

typedef enum
{
    APP_MODULE_MAIN = LIB_MODULE_MAX,
    MODULES_MAX
} appModules_et;

#define DEFAULT_LOG_LEVELS {                                                           \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_MAIN_MENU  */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_SYSTEM     */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_BLE        */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_WIFI       */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_MQTT       */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_AWS        */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_JOBS       */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_GPIO       */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_FLASH      */                                     \
    PRINT_LEVEL_ERROR, /* DRV_MODULE_BLE        */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_JSON       */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_HTTP       */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_OTA        */                                     \
    PRINT_LEVEL_ERROR, /* LIB_MODULE_UART       */                                     \
    PRINT_LEVEL_DEBUG, /* APP_MODULE_MAIN       **Add application modules from here */ \
};

//-------------------------APP CONFIG--------------------------/

#define DEVICE_NAME_PREFIX "esp32"

#define TASK_APP_PRIORITY 4
#define TASK_APP_STACK_SIZE (5 * 1024) // reduce it to 4096

#define TEST_WIFI_SSID "buildstorm"
#define TEST_WIFI_PASSWORD "wifipassword

#define AWS_IOT_MQTT_HOST "your-aws-host.amazonaws.com"
#define AWS_IOT_MQTT_PORT 8883
#define AWS_PROVISION_TEMPLATE_NAME "your-fleet-prov-template"

#define SHADWO_KEY_LED0 "LED0"
#define SHADWO_KEY_LED1 "LED1"

#define LED0_PIN 2


#define APP_VERSION "1.0.0"



#endif
