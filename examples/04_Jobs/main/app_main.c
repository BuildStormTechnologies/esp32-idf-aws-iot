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
#include "lib_jobs.h"
#include "lib_gpio.h"
#include "app_config.h"

/* Macros ------------------------------------------------------------------*/

#define thisModule APP_MODULE_MAIN
#define MAX_LED_BLINK_COUNT 5

/* Certificates ---------------------------------------------------------*/
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t claim_certificate_pem_crt_start[] asm("_binary_claim_certificate_pem_crt_start");
extern const uint8_t claim_private_pem_key_start[] asm("_binary_claim_private_pem_key_start");

/* Variables -----------------------------------------------------------------*/

static char gJobIdStr[LENGTH_JOB_ID] = {0};
static char gJobDocumentStr[LENGTH_JOB_DOCUMENT] = {0};
static bool gJobReceived_b8 = FALSE;

void app_eventsCallBackHandler(systemEvents_et event_e)
{
    switch (event_e)
    {
    case EVENT_WIFI_CONNECTED:
        printf("\r\nEVENT_WIFI_CONNECTED");
        break;
    case EVENT_WIFI_DISCONNECTED:
        printf("\r\nEVENT_WIFI_DISCONNECTED");
        break;

    case EVENT_MQTT_CONNECTED:
        printf("\r\nEVENT_MQTT_CONNECTED");
        break;
    case EVENT_MQTT_DISCONNECTED:
        printf("\r\nEVENT_MQTT_DISCONNECTED");
        break;

    default:
        break;
    }
}

jobsStatus_et app_jobHandlerLed(const job_st *ps_job)
{
    printf("\r\n%s : %s", ps_job->idStr, ps_job->documentStr);

    // copy gJobIdStr & gJobDocumentStr
    strcpy(gJobIdStr, ps_job->idStr);
    strcpy(gJobDocumentStr, ps_job->documentStr);

    // handle the operation in application task
    gJobReceived_b8 = true;

    return JOB_STATUS_IN_PROGRESS;
}

void app_jobCheck()
{
    // job received? perform operation
    if (gJobReceived_b8)
    {
        gJobReceived_b8 = FALSE;

        uint32_t countValue_u32 = 0;
        uint32_t onTime_u32 = 0;
        uint32_t offTime_u32 = 0;
        char countStr[4] = {0};
        char onTimeStr[4] = {0};
        char offTimeStr[4] = {0};

        // variable for extracting json key-value pair
        tagStructure_st jobKeyValuePair[3] = {
            {"count", countStr},
            {"onTime", onTimeStr},
            {"offTime", offTimeStr}};

        // parse job document to initialize count value
        if (JSON_processString(gJobDocumentStr, jobKeyValuePair, 3, FALSE))
        {
            countValue_u32 = util_getNumFromStringU32(countStr);
            onTime_u32 = util_getNumFromStringU32(onTimeStr);
            offTime_u32 = util_getNumFromStringU32(offTimeStr);

            printf("\r\nblink %ld times => ON:%ldms OFF:%ldms", countValue_u32, onTime_u32, offTime_u32);
            if (countValue_u32 < MAX_LED_BLINK_COUNT)
            {
                // blink 3 times
                for (uint32_t i = 0; i < countValue_u32; i++)
                {
                    if (onTime_u32)
                    {
                        GPIO_pinWrite(LED0_PIN, HIGH);
                        TASK_DELAY_MS(onTime_u32);
                    }
                    if (offTime_u32)
                    {
                        GPIO_pinWrite(LED0_PIN, LOW);
                        TASK_DELAY_MS(offTime_u32);
                    }
                }

                JOBS_updateStatus(gJobIdStr, JOB_STATUS_SUCCESSED);
            }
            else
            {
                print_error("Error: blink count %ld is greater than allowed blinks (%ld)", countValue_u32, MAX_LED_BLINK_COUNT);
                JOBS_updateStatus(gJobIdStr, JOB_STATUS_FAILED);
            }
        }
        else
        {
            print_error("Error: Invalid job document: %s", gJobDocumentStr);
        }
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
                    printf("\r\nDevice successfully provisioned");
                }
                else
                {
                    printf("\r\nDevice is not provisioned");
                }
            }
            break;

        case SYSTEM_MODE_NORMAL:
            if (AWS_isConnected())
            {
                app_jobCheck();
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
            .pHostNameStr = AWS_IOT_MQTT_HOST,
            .port_u16 = AWS_IOT_MQTT_PORT,
            .pRootCaStr = (char *)aws_root_ca_pem_start,
            .pClaimCertStr = (char *)claim_certificate_pem_crt_start,
            .pClaimPrivateKeyStr = (char *)claim_private_pem_key_start,
            .pClaimTemplateStr = AWS_PROVISION_TEMPLATE_NAME}};

    GPIO_pinMode(LED0_PIN, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE, NULL);
    GPIO_pinWrite(LED0_PIN, LOW);

    SYSTEM_init(&sysConfig);
    SYSTEM_start();
    if (SYSTEM_getMode() == SYSTEM_MODE_NORMAL)
    {
        if (JOBS_register("blink", 0, app_jobHandlerLed))
        {
            printf("\r\nblink job reg success");
        }
        else
        {
            printf("\r\nblink job reg failed");
        }
    }

    BaseType_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);
    if (pdPASS != err)
    {
        print_error("\r\n Error 0x%X in creating app_task \r\n restarting system\r\n\r\r\n\r", err);
        fflush(stdout);
        esp_restart();
    }
}
