/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file app_main.c
 * \brief app_main.c file.
 *
 * The app_main.c is the main entry of the application.
 *
 *
 */

/* Includes ------------------------------------------------------------------*/

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lib_system.h"
#include "app_config.h"

/* Macros ------------------------------------------------------------------*/

#define thisModule APP_MODULE_MAIN

/* Certificates ---------------------------------------------------------*/
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t thing_certificate_pem_crt_start[] asm("_binary_thing_certificate_pem_crt_start");
extern const uint8_t thing_private_pem_key_start[] asm("_binary_thing_private_pem_key_start");

/* Variables -----------------------------------------------------------------*/

void app_eventsCallBackHandler(systemEvents_et event_e)
{
    switch (event_e)
    {
    case EVENT_WIFI_CONNECTED:
        printf("\nEVENT_WIFI_CONNECTED");
        break;
    case EVENT_WIFI_DISCONNECTED:
        printf("\nEVENT_WIFI_DISCONNECTED");
        break;

    case EVENT_MQTT_CONNECTED:
        printf("\nEVENT_MQTT_CONNECTED");
        break;
    case EVENT_MQTT_DISCONNECTED:
        printf("\nEVENT_MQTT_DISCONNECTED");
        break;

    default:
        break;
    }
}

void app_task(void *param)
{
    mqttMsg_st awsMsg = {0};

    uint32_t nextAwsPublishTime_u32 = 0;
    uint8_t counter_u8 = 5;

    if (SYSTEM_getMode() == SYSTEM_MODE_NORMAL)
    {
        // subscribe to a topic
        AWS_subscribe(TEST_AWS_TOPIC_SUBSCRIBE, QOS0_AT_MOST_ONCE);
    }

    while (1)
    {
        switch (SYSTEM_getMode())
        {
        case SYSTEM_MODE_DEVICE_CONFIG:
            break;

        case SYSTEM_MODE_NORMAL:
            if (AWS_isConnected())
            {
                // publish message every 5 seconds
                if ((millis() > nextAwsPublishTime_u32) && counter_u8)
                {
                    nextAwsPublishTime_u32 = millis() + 5000;
                    awsMsg.payloadLen_u16 = sprintf(awsMsg.payloadStr, "Hello from device - counter: %d", counter_u8--);
                    awsMsg.topicLen_u8 = sprintf(awsMsg.topicStr, TEST_AWS_TOPIC_PUBLISH);

                    AWS_publish(&awsMsg);
                    print_verbose("PUB Message =>  topic:%s  payload:%s", awsMsg.topicStr, awsMsg.payloadStr);
                }

                // message received? print it
                if (AWS_subMsgRead(&awsMsg))
                {
                    print_verbose("SUB Message =>  topic:%s  payload:%s", awsMsg.topicStr, awsMsg.payloadStr);
                }
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
        .systemEventCallbackHandler = app_eventsCallBackHandler,
        .pAppVersionStr = APP_VERSION,
        .pWifiSsidStr = TEST_WIFI_SSID,
        .pWifiPwdStr = TEST_WIFI_PASSWORD,

        .s_mqttClientConfig = {
            .maxPubMsgToStore_u8 = 6,
            .maxSubMsgToStore_u8 = 4,
            .maxSubscribeTopics_u8 = 6,
            .maxJobs_u8 = 2,
            .pThingNameStr = AWS_THING_NAME,
            .pHostNameStr = AWS_IOT_MQTT_HOST,
            .port_u16 = AWS_IOT_MQTT_PORT,
            .pRootCaStr = (char *)aws_root_ca_pem_start,
            .pThingCertStr = (char *)thing_certificate_pem_crt_start,
            .pThingPrivateKeyStr = (char *)thing_private_pem_key_start,
        }};

    if (SYSTEM_init(&sysConfig) == TRUE)
    {
        SYSTEM_start();

        BaseType_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);
        if (pdPASS != err)
        {
            printf("\nError 0x%X in creating app_task \n restarting system\n\r\n\r", err);
            fflush(stdout);
            esp_restart();
        }
    }
    else
    {
        while (1)
        {
            printf("\nSystem Init failed, verify the init config ....");
            TASK_DELAY_MS(5000);
        }
    }
}
