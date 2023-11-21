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
#include "lib_ble.h"
#include "lib_wifi.h"
#include "lib_aws.h"
#include "lib_jobs.h"
#include "lib_gpio.h"
#include "lib_flash.h"
#include "lib_delay.h"
#include "lib_print.h"
#include "lib_utils.h"

#include "app_config.h"

/* Macros ------------------------------------------------------------------*/

#define thisModule APP_MODULE_MAIN

#define STR_SHADOW_KEY_LED "LED"
#define STR_SHADOW_KEY_COLOR "COLOR"
#define STR_AWS_TOPIC_PUBLISH "devicePubTopic"
#define STR_AWS_TOPIC_SUBSCRIBE "deviceSubTopic"

/* User Data Types -----------------------------------------------------------------*/

typedef struct
{
    uint16_t counter_u16;
    char messageStr[32];
} userData_st;

/* Thing Certificates ---------------------------------------------------------*/
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");

#if (!USE_BLE_FOR_PROVISION)
#if USE_CLAIM_CERTIFICATES
extern const uint8_t claim_certificate_pem_crt_start[] asm("_binary_claim_certificate_pem_crt_start");
extern const uint8_t claim_private_pem_key_start[] asm("_binary_claim_private_pem_key_start");
#else
extern const uint8_t thing_certificate_pem_crt_start[] asm("_binary_thing_certificate_pem_crt_start");
extern const uint8_t thing_private_pem_key_start[] asm("_binary_thing_private_pem_key_start");
#endif
#endif

/* Variables -----------------------------------------------------------------*/

static logLevels_et gDefaultLogLevels_ae[MODULES_MAX] = DEFAULT_LOG_LEVELS;

static mqttMsg_st s_pubMsg = {0};
static mqttMsg_st s_subMsg = {0};
static uint32_t gDesiredLedState_u32 = LOW, gReportedLedState_u32 = LOW;
static char gDesiredColorStr[LENGTH_AWS_SHADOW_BUFFER] = "GREY", gReportedColorStr[LENGTH_AWS_SHADOW_BUFFER] = "GREY";
static char gJobId1_str[LENGTH_JOB_ID] = {0}, gJobId2_str[LENGTH_JOB_ID] = {0};
static char gJobId3_str[LENGTH_JOB_ID] = {0}, gJobId4_str[LENGTH_JOB_ID] = {0};

void app_ledBlink()
{
    static uint32_t nextLedChangeTime_u32 = 0;
    static bool pinState_b8 = LOW;

    if (millis() > nextLedChangeTime_u32)
    {
        nextLedChangeTime_u32 = millis() + 400;
        GPIO_pinWrite(LED0_PIN, pinState_b8);
        pinState_b8 = !pinState_b8;
    }
}

jobsStatus_et app_testJob1_handler(const job_st *ps_job)
{
    print_info("%s : %s", ps_job->idStr, ps_job->documentStr);
    print_info("q:%u  c:%u", ps_job->queuedAt_u32, ps_job->currentTimeStamp_u32);
    strcpy(gJobId1_str, ps_job->idStr);
    return JOB_STATUS_IN_PROGRESS; // job accepted
}

jobsStatus_et app_testJob2_handler(const job_st *ps_job)
{
    print_info("%s : %s", ps_job->idStr, ps_job->documentStr);
    print_info("q:%u  c:%u", ps_job->queuedAt_u32, ps_job->currentTimeStamp_u32);
    // don't copy job id
    // reject the job
    //    strcpy(gJobId2_str, ps_job->idStr);
    return JOB_STATUS_REJECTED; // job rejected
}

jobsStatus_et app_testJob3_handler(const job_st *ps_job)
{
    print_info("%s : %s", ps_job->idStr, ps_job->documentStr);
    print_info("q:%u  c:%u", ps_job->queuedAt_u32, ps_job->currentTimeStamp_u32);
    strcpy(gJobId3_str, ps_job->idStr);
    return JOB_STATUS_IN_PROGRESS; // job accepted
}

jobsStatus_et app_testJob4_handler(const job_st *ps_job)
{
    print_info("%s : %s", ps_job->idStr, ps_job->documentStr);
    print_info("q:%u  c:%u", ps_job->queuedAt_u32, ps_job->currentTimeStamp_u32);
    strcpy(gJobId4_str, ps_job->idStr);
    return JOB_STATUS_IN_PROGRESS; // job accepted
}

void app_jobCheck()
{
    if (gJobId1_str[0])
    {
        print_info("%s", gJobId1_str);
        TASK_DELAY_MS(10000);
        JOBS_updateStatus(gJobId1_str, JOB_STATUS_SUCCESSED);
        gJobId1_str[0] = 0;
    }
    else if (gJobId2_str[0])
    {
        print_info("%s", gJobId2_str);
        TASK_DELAY_MS(10000);
        JOBS_updateStatus(gJobId2_str, JOB_STATUS_FAILED);
        gJobId2_str[0] = 0;
    }
    else if (gJobId3_str[0])
    {
        print_info("%s", gJobId3_str);
        // don't update status. Let it timeout
        gJobId3_str[0] = 0;
    }
    else if (gJobId4_str[0])
    {
        print_info("%s", gJobId4_str);
        TASK_DELAY_MS(10000);
        JOBS_updateStatus(gJobId4_str, JOB_STATUS_FAILED);
        gJobId4_str[0] = 0;
    }
}

void app_ledShadowHandler(const char *pkey, const void *value)
{
    gDesiredLedState_u32 = *(uint8_t *)value;
    print_info("Delta update desired Led : %d", gDesiredLedState_u32);
}

void app_colorShadowHandler(const char *pkey, const void *value)
{
    if ((strcmp((char *)value, "WHITE") == 0) ||
        (strcmp((char *)value, "RED") == 0) ||
        (strcmp((char *)value, "GREEN") == 0) ||
        (strcmp((char *)value, "BLUE") == 0))
    {
        strcpy(gDesiredColorStr, (char *)value);
        print_info("gDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
    }
}

void app_publishData()
{
    static uint32_t nextPublishTime_u32 = 0;
    static uint8_t publishCount_u8 = 0;

    if ((millis() > nextPublishTime_u32) && (publishCount_u8 < 20))
    {
        nextPublishTime_u32 = millis() + 10000;

        s_pubMsg.payloadLen_u16 = sprintf(s_pubMsg.payloadStr, "Hello World %03d", publishCount_u8++);
        s_pubMsg.topicLen_u8 = sprintf(s_pubMsg.topicStr, STR_AWS_TOPIC_PUBLISH);
        print_info("%s", s_pubMsg.payloadStr);
        AWS_publish(&s_pubMsg);
    }
}

void app_awsInit()
{
    awsShadow_st led_Shadow = {
        .keyStr = STR_SHADOW_KEY_LED,
        .valType_e = SHADOW_VALUE_TYPE_INT,
        .callbackHandler = app_ledShadowHandler,
        .value_e.val_i32 = gDesiredLedState_u32};

    awsShadow_st color_Shadow = {
        .keyStr = STR_SHADOW_KEY_COLOR,
        .valType_e = SHADOW_VALUE_TYPE_STRING,
        .callbackHandler = app_colorShadowHandler,
        .value_e.str = ""};

    strcpy(color_Shadow.value_e.str, gDesiredColorStr);

    if (AWS_shadowDeltaRegister(&led_Shadow))
    {
        print_info("led_Shadow reg success");
    }
    else
    {
        print_error("led_Shadow reg failed");
    }

    if (AWS_shadowDeltaRegister(&color_Shadow))
    {
        print_info("color_Shadow reg success");
    }
    else
    {
        print_error("color_Shadow reg failed");
    }

    if (AWS_subscribe(STR_AWS_TOPIC_SUBSCRIBE, QOS0_AT_MOST_ONCE))
    {
        print_info("aws subscribe success");
    }
    else
    {
        print_error("aws subscribe failed");
    }

    if (JOBS_register("TEST_JOB_1", 2, app_testJob1_handler))
    {
        print_info("TEST_JOB_1 reg success");
    }
    else
    {
        print_error("TEST_JOB_1 reg failed");
    }

    if (JOBS_register("TEST_JOB_2", 0, app_testJob2_handler)) // timeout = 0 means no timeout
    {
        print_info("TEST_JOB_2 reg success");
    }
    else
    {
        print_error("TEST_JOB_2 reg failed");
    }

    if (JOBS_register("TEST_JOB_3", 1, app_testJob3_handler))
    {
        print_info("TEST_JOB_3 reg success");
    }
    else
    {
        print_error("TEST_JOB_3 reg failed");
    }

    if (JOBS_register("TEST_JOB_4", 3, app_testJob4_handler))
    {
        print_info("TEST_JOB_4 reg success");
    }
    else
    {
        print_error("TEST_JOB_4 reg failed");
    }
}

void app_userDataTest()
{
    userData_st appData = {0};

    // read operation
    if (FLASH_appDataRead((void *)&appData, sizeof(userData_st)))
    {
        print_info("Data read successfully => counter: %d, Msg: %s",
                   appData.counter_u16,
                   appData.messageStr);
    }
    else
    {
        print_error("Failed to read from flash");
    }

    print_info("Updating data");

    // update variables
    appData.counter_u16++;
    if (appData.counter_u16 % 2)
    {
        strcpy(appData.messageStr, "Counter is odd");
    }
    else
    {
        strcpy(appData.messageStr, "Counter is even");
    }

    // write  operation
    if (FLASH_appDataWrite((void *)&appData, sizeof(userData_st)))
    {
        print_info("Data written successfully => Counter: %d, Msg: %s",
                   appData.counter_u16, appData.messageStr);
    }
    else
    {
        print_error("Failed to write to flash");
    }
}

void app_bleCallback(const char *pPacketStr, const char *pDataStr)
{
    print_info("%s:%s", pPacketStr, pDataStr);
}

void app_eventsCallbackHandler(systemEvents_et event_e)
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

/**
 * @brief  This function publishes initial device status.
 * @retval None
 */
void app_publishDeviceStatus()
{
    awsThingShadow_st as_shadow[4] = {
        {.keyStr = "thing_name",
         .valType_e = SHADOW_VALUE_TYPE_STRING},
        {.keyStr = "firmware_ver",
         .valType_e = SHADOW_VALUE_TYPE_STRING,
         .value_e.str = APP_VERSION},
        {.keyStr = STR_SHADOW_KEY_LED,
         .valType_e = SHADOW_VALUE_TYPE_INT,
         .value_e.val_i32 = gDesiredLedState_u32},
        {.keyStr = STR_SHADOW_KEY_COLOR,
         .valType_e = SHADOW_VALUE_TYPE_STRING}};

    strcpy(as_shadow[0].value_e.str, AWS_getThingName());
    strcpy(as_shadow[3].value_e.str, gDesiredColorStr);
    AWS_shadowDocumentUpdate(as_shadow, 4, SHADOW_UPDATE_TYPE_ALL);
    print_verbose("Done");
}

void app_task(void *param)
{
    uint32_t nextMsgTime_u32 = 0;

    BLE_packetRegister("tes", app_bleCallback);
    app_userDataTest();

    if (AWS_getThingName())
    {
        app_publishDeviceStatus();
    }

    while (1)
    {
        switch (SYSTEM_getMode())
        {
        case SYSTEM_MODE_DEVICE_CONFIG:
            app_ledBlink();
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
                if (AWS_subMsgRead(&s_subMsg))
                {
                    print_info("  SUB Message =>  topic:%s  payload:%s", s_subMsg.topicStr, s_subMsg.payloadStr);
                }
                else if (gDesiredLedState_u32 != gReportedLedState_u32)
                {
                    gReportedLedState_u32 = gDesiredLedState_u32;
                    GPIO_pinWrite(LED0_PIN, gDesiredLedState_u32);
                    print_info("gDesiredLedState_u32:%d gReportedLedState_u32:%d", gDesiredLedState_u32, gReportedLedState_u32);
                    AWS_shadowUpdate(STR_SHADOW_KEY_LED, &gReportedLedState_u32, SHADOW_UPDATE_TYPE_REPORTED);
                }
                else if (strcmp(gDesiredColorStr, gReportedColorStr) != 0)
                {
                    strcpy(gReportedColorStr, gDesiredColorStr);
                    print_info("gDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
                    AWS_shadowUpdate(STR_SHADOW_KEY_COLOR, gReportedColorStr, SHADOW_UPDATE_TYPE_REPORTED);
                }
                else
                {
                    app_publishData();
                    app_jobCheck();
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
    systemInitConfig_st s_sysConfig =
    {.pLogLevels_e = gDefaultLogLevels_ae,
     .logModulesCount_u8 = MODULES_MAX,
     .systemEventCallbackHandler = app_eventsCallbackHandler,
     .pDeviceNamePrefixStr = DEVICE_NAME_PREFIX,
     .pAppVersionStr = APP_VERSION,
#if (!USE_BLE_FOR_PROVISION)
     .pWifiSsidStr = TEST_WIFI_SSID,
     .pWifiPwdStr = TEST_WIFI_PASSWORD,
#endif

#if USE_BLE_FOR_PROVISION
     .s_awsConfig =
         {.pRootCaStr = (char *)aws_root_ca_pem_start}
#else
#if USE_CLAIM_CERTIFICATES
     .s_awsConfig = {
         .hostNameStr = AWS_IOT_MQTT_HOST,
         .port_u16 = AWS_IOT_MQTT_PORT,
         .maxShadowCount_u8 = 6,
         .maxPublishMessagesToStore_u8 = 4,
         .maxSubscribeMessagesToStore_u8 = 4,
         .maxSubscribeTopics_u8 = 4,
         .maxJobs_u8 = 5,
         .pRootCaStr = (char *)aws_root_ca_pem_start,
         .pClaimCertStr = (char *)claim_certificate_pem_crt_start,
         .pClaimPrivateKeyStr = (char *)claim_private_pem_key_start,
         .pClaimTemplateStr = AWS_PROVISION_TEMPLATE_NAME}
#else
     .s_awsConfig =
         {
             .pThingNameStr = MY_THING_NAME,
             .hostNameStr = AWS_IOT_MQTT_HOST,
             .port_u16 = AWS_IOT_MQTT_PORT,
             .maxShadowCount_u8 = 6,
             .maxPublishMessagesToStore_u8 = 4,
             .maxSubscribeMessagesToStore_u8 = 4,
             .maxSubscribeTopics_u8 = 4,
             .maxJobs_u8 = 5,
             .pRootCaStr = (char *)aws_root_ca_pem_start,
             .pThingCertStr = (char *)thing_certificate_pem_crt_start,
             .pThingPrivateKeyStr = (char *)thing_private_pem_key_start}
#endif // USE_CLAIM_CERTIFICATES
#endif // USE_BLE_FOR_PROVISION
    };

    SYSTEM_init(&s_sysConfig);
    BLE_init();
    SYSTEM_start();
    GPIO_pinMode(LED0_PIN, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE, NULL);
    GPIO_pinWrite(LED0_PIN, gDesiredLedState_u32);

    if (SYSTEM_getMode() == SYSTEM_MODE_NORMAL)
    {
        app_awsInit();
    }

    BaseType_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);
    if (pdPASS != err)
    {
        print_error("\n Error 0x%X in creating app_task \n restarting system\n\r\n\r", err);
        fflush(stdout);
        esp_restart();
    }
}
