/*******************************************************************************
 *
 * Filename:     app_main.c
 *
 * Description:
 *               main entry of the application .
 *******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lib_system.h"
#include "lib_aws.h"
#include "lib_ble.h"
#include "lib_gpio.h"
#include "lib_flash.h"
#include "lib_delay.h"
#include "lib_print.h"
#include "lib_utils.h"
#include "app_config.h"
#include "lib_shadow.h"

#define thisModule APP_MODULE_MAIN

/* Certificates ---------------------------------------------------------*/
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");

/* Variables -----------------------------------------------------------------*/
static logLevels_et gDefaultLogLevels_ae[MODULES_MAX] = DEFAULT_LOG_LEVELS;


void classicShadowUpdateCallBack(const char *pKeyStr,const void *pValue);
void statusShadowUpdateCallBack(const char *pKeyStr, const void *pValue);


#define MAX_TYPES_OF_SHADOWS 8

awsShadow_st classicShadowElements[] ={
                        {SHADOW_VALUE_TYPE_INT,      "FV",        value_e:{val_i32:0},   FALSE, TRUE, SHADOW_UPDATE_TYPE_ALL},
                     };

awsShadow_st statusShadowElements[] ={
                        {SHADOW_VALUE_TYPE_FLOAT,      "s01",          value_e:{val_f32:3.1},     FALSE, TRUE, SHADOW_UPDATE_TYPE_ALL},
                        {SHADOW_VALUE_TYPE_FLOAT,      "s02",          value_e:{val_f32:3.1},     FALSE, TRUE, SHADOW_UPDATE_TYPE_ALL},
                     };
shadowConfigTable_st shadowTable[MAX_TYPES_OF_SHADOWS]=
{
   {
      ptrShadowName:NULL,
      maxElementCount_u8:(sizeof(classicShadowElements)/sizeof(classicShadowElements[0])),
	  callbackHandler:classicShadowUpdateCallBack,
      pShadowElementsTable: classicShadowElements,
    },

    {
      ptrShadowName:"STATUS",
      maxElementCount_u8:(sizeof(statusShadowElements)/sizeof(statusShadowElements[0])),
	  callbackHandler:statusShadowUpdateCallBack,
      pShadowElementsTable: statusShadowElements,
    },

};

void classicShadowUpdateCallBack(const char *pKeyStr, const void *pValue)
{
   print_verbose("Classic shadow: %s",pKeyStr);
     SHADOW_update(0, (char *)pKeyStr,(void *)pValue,SHADOW_UPDATE_TYPE_REPORTED); // index 0->Classic
}


void statusShadowUpdateCallBack(const char *pKeyStr, const void *pValue)
{
  print_verbose("Status shadow: %s",pKeyStr);
  SHADOW_update(1, (char *)pKeyStr,(void *)pValue,SHADOW_UPDATE_TYPE_REPORTED);//index 1->Status

}

void app_eventsCallBackHandler(systemEvents_et event_e)
{
    switch (event_e)
    {
    case EVENT_WIFI_CONNECTED:
        print_info("EVENT_WIFI_CONNECTED");
        break;
    case EVENT_WIFI_DISCONNECTED:
        print_info("EVENT_WIFI_DISCONNECTED");
        break;

    case EVENT_AWS_CONNECTED:
        print_info("EVENT_AWS_CONNECTED");
        break;
    case EVENT_AWS_DISCONNECTED:
        print_info("EVENT_AWS_DISCONNECTED");
        break;

    case EVENT_BLE_CONNECTED:
        print_info("EVENT_BLE_CONNECTED");
        break;
    case EVENT_BLE_AUTHENTICATED:
        print_info("EVENT_BLE_AUTHENTICATED");
        break;
    case EVENT_BLE_DISCONNECTED:
        print_info("EVENT_BLE_DISCONNECTED");
        break;

    default:
        break;
    }
}


void app_task(void *param)
{
    uint32_t nextMsgTime_u32 = 0;

    while (1)
    {
        switch (SYSTEM_getMode())
        {
        case SYSTEM_MODE_DEVICE_CONFIG:
            if (millis() > nextMsgTime_u32)
            {
                nextMsgTime_u32 = millis() + 2000;

                if (FLASH_isDeviceRegistered())
                {
                    print_info("Device successfully provisioned");
                }
                else
                {
                    print_info("Device is not provisioned");
                }
            }
            break;

        case SYSTEM_MODE_NORMAL:
            if (AWS_isConnected())
            {

            }
            break;

        case SYSTEM_MODE_OTA:
            break;

        default:
            break;
        }

        TASK_DELAY_MS(200);
    }
}


/**
 * @brief    entry point of the project
 * @param    None
 * @return   None
 */
void app_main()
{
    systemInitConfig_st sysConfig = {
        .pLogLevels_e = gDefaultLogLevels_ae,
        .logModulesCount_u8 = MODULES_MAX,
        .systemEventCallbackHandler = app_eventsCallBackHandler,
        .pDeviceNamePrefixStr = DEVICE_NAME_PREFIX,
        .pAppVersionStr = APP_VERSION,

        .s_awsConfig = {
             .maxShadowCount_u8 = 6,
             .maxPublishMessagesToStore_u8 = 10,
             .maxSubscribeMessagesToStore_u8 = 6,
             .maxSubscribeTopics_u8 = 6,
             .maxJobs_u8 = 5,
             .pRootCaStr = (char *)aws_root_ca_pem_start}
        };

    if (SYSTEM_init(&sysConfig) == TRUE)
    {
        BLE_init();
        SYSTEM_start();

        if (SYSTEM_getMode() == SYSTEM_MODE_NORMAL)
        {

            SHADOW_register(shadowTable, MAX_TYPES_OF_SHADOWS);
        }

        BaseType_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);
        if (pdPASS != err)
        {
            print_error("\n Error 0x%X in creating app_task \n restarting system\n\r\n\r", err);
            fflush(stdout);
            esp_restart();
        }
    }
    else
    {
        while(1)
        {
            print_verbose("System Init failed, verify the init config ....");
            TASK_DELAY_MS(5000);
        }
    }
}
