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
#include "lib_jobs.h"
#include "lib_gpio.h"
#include "app_config.h"

/* Macros ------------------------------------------------------------------*/

#define thisModule APP_MODULE_MAIN
#define MAX_LED_BLINK_COUNT 5

#define STR_SHADOW_KEY_LED "LED"
#define STR_SHADOW_KEY_COLOR "COLOR"

#define DESIRED_LED_STATE 0
#define DESIRED_COLOR "RED"

/* Certificates ---------------------------------------------------------*/
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t thing_certificate_pem_crt_start[] asm("_binary_thing_certificate_pem_crt_start");
extern const uint8_t thing_private_pem_key_start[] asm("_binary_thing_private_pem_key_start");

/* Variables -----------------------------------------------------------------*/
int32_t gDesiredLedState_s32 = 0, gReportedLedState_s32 = 0;
char gDesiredColorStr[12] = DESIRED_COLOR, gReportedColorStr[12] = DESIRED_COLOR;

static char gJobIdStr[LENGTH_JOB_ID] = {0};
static char gJobDocumentStr[LENGTH_JOB_DOCUMENT] = {0};
static bool gJobReceived_b8 = FALSE;

void classicShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue);

#define CLASSIC_SHADOW 0
#define MAX_TYPES_OF_SHADOWS 1

const awsShadowElement_st classicShadowElements[] = {
    /* Shaodw Value type          Key         value                    needToPublish shadowUpdateType*/
    {SHADOW_VALUE_TYPE_STRING, "fw_ver", s_value : {pStr : APP_VERSION}, TRUE, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_INT, "LED", s_value : {val_i32 : 0}, TRUE, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_STRING, "COLOR", s_value : {pStr : DESIRED_COLOR}, TRUE, SHADOW_UPDATE_TYPE_ALL},
};

const shadowConfigTable_st shadowTable[MAX_TYPES_OF_SHADOWS] =
    {
        {
            // CLASSIC_SHADOW
            ptrShadowName : NULL,
            maxElementCount_u8 : (sizeof(classicShadowElements) / sizeof(classicShadowElements[0])),
            callbackHandler : classicShadowUpdateCallBack,
            pShadowElementsTable : classicShadowElements,
        },
};

void classicShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue)
{
    printf("\nClassic shadow: %s", pKeyStr);
    if (elementIndex_u8 == 1) // 0-FV, 1-LED, 2-COLOR
    {
        gDesiredLedState_s32 = *(uint8_t *)pValue;
        printf("\nDelta update desired Led : %ld", gDesiredLedState_s32);
    }
    else if (elementIndex_u8 == 2) // 0-FV, 1-LED, 2-COLOR
    {
        if ((strcmp((char *)pValue, "WHITE") == 0) ||
            (strcmp((char *)pValue, "RED") == 0) ||
            (strcmp((char *)pValue, "GREEN") == 0) ||
            (strcmp((char *)pValue, "BLUE") == 0))
        {
            strcpy(gDesiredColorStr, (char *)pValue);
            printf("\ngDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
        }
    }
}

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

    case EVENT_BLE_CONNECTED:
        printf("\nEVENT_BLE_CONNECTED");
        break;
    case EVENT_BLE_AUTHENTICATED:
        printf("\nEVENT_BLE_AUTHENTICATED");
        break;
    case EVENT_BLE_DISCONNECTED:
        printf("\nEVENT_BLE_DISCONNECTED");
        break;

    default:
        break;
    }
}

jobsStatus_et app_jobHandlerLed(const job_st *ps_job)
{
    printf("\n%s : %s", ps_job->idStr, ps_job->documentStr);

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

            printf("\nblink %ld times => ON:%ldms OFF:%ldms", countValue_u32, onTime_u32, offTime_u32);
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
                    printf("\nDevice successfully provisioned");
                }
                else
                {
                    printf("\nDevice is not provisioned");
                }
            }
            break;

        case SYSTEM_MODE_NORMAL:
            if (AWS_isConnected())
            {
                app_jobCheck();
                if (gDesiredLedState_s32 != gReportedLedState_s32)
                {
                    gReportedLedState_s32 = gDesiredLedState_s32;
                    GPIO_pinWrite(LED0_PIN, gDesiredLedState_s32);
                    printf("\ngDesiredLedState_s32:%ld gReportedLedState_s32:%ld", gDesiredLedState_s32, gReportedLedState_s32);
                    SHADOW_update(CLASSIC_SHADOW, STR_SHADOW_KEY_LED, &gReportedLedState_s32, SHADOW_UPDATE_TYPE_REPORTED);
                }
                else if (strcmp(gDesiredColorStr, gReportedColorStr) != 0)
                {
                    strcpy(gReportedColorStr, gDesiredColorStr);
                    printf("\ngDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
                    SHADOW_update(CLASSIC_SHADOW, STR_SHADOW_KEY_COLOR, gReportedColorStr, SHADOW_UPDATE_TYPE_REPORTED);
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
        // .pWifiSsidStr = TEST_WIFI_SSID,
        // .pWifiPwdStr = TEST_WIFI_PASSWORD,

        .s_mqttClientConfig = {
            .maxPubMsgToStore_u8 = 6,
            .maxSubMsgToStore_u8 = 4,
            .maxSubscribeTopics_u8 = 6,
            .maxJobs_u8 = 2,
            //  .pThingNameStr = MY_THING_NAME,
            // .pHostNameStr = AWS_IOT_MQTT_HOST,
            // .port_u16 = AWS_IOT_MQTT_PORT,
            .pRootCaStr = (char *)aws_root_ca_pem_start,
            // .pThingCertStr = (char *)thing_certificate_pem_crt_start,
            // .pThingPrivateKeyStr = (char *)thing_private_pem_key_start,
        }};

    GPIO_pinMode(LED0_PIN, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE, NULL);
    GPIO_pinWrite(LED0_PIN, LOW);

    if (SYSTEM_init(&sysConfig) == TRUE)
    {
        BLE_init();

        if (SYSTEM_getMode() == SYSTEM_MODE_NORMAL)
        {

            SHADOW_register(shadowTable, MAX_TYPES_OF_SHADOWS);
            if (JOBS_register("blink", 0, app_jobHandlerLed))
            {
                printf("\nblink job reg success");
            }
            else
            {
                printf("\nblink job reg failed");
            }
        }

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
