/*******************************************************************************
 *
 * Filename:     app_main.c
 *
 * Description:
 *               main entry of the application .
 *******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include "lib_system.h"
#include "lib_aws.h"
#include "lib_gpio.h"
#include "lib_flash.h"
#include "lib_delay.h"
#include "lib_print.h"
#include "lib_utils.h"
#include "app_config.h"

/* Macros ------------------------------------------------------------------*/

#define thisModule APP_MODULE_MAIN

/* Thing Certificates ---------------------------------------------------------*/
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t claim_certificate_pem_crt_start[] asm("_binary_claim_certificate_pem_crt_start");
extern const uint8_t claim_private_pem_key_start[] asm("_binary_claim_private_pem_key_start");

/* Variables -----------------------------------------------------------------*/
static logLevels_et gDefaultLogLevels_ae[MODULES_MAX] = DEFAULT_LOG_LEVELS;

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

/**
 * @brief  This function publishes initial device status.
 * @retval None
 */
void app_publishDeviceStatus()
{
    awsThingShadow_st as_shadow[3] = {
        {.keyStr = "thing_name",
         .valType_e = SHADOW_VALUE_TYPE_STRING},
        {.keyStr = "firmware_ver",
         .valType_e = SHADOW_VALUE_TYPE_STRING,
         .value_e.str = APP_VERSION}};

    strcpy(as_shadow[0].value_e.str, AWS_getThingName());
    AWS_shadowDocumentUpdate(as_shadow, 3, SHADOW_UPDATE_TYPE_ALL);
}

void app_task(void *param)
{
    uint32_t nextMsgTime_u32 = 0;

    if (AWS_getThingName())
    {
        app_publishDeviceStatus();
    }

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
                GPIO_pinWrite(LED0_PIN, HIGH);
                TASK_DELAY_MS(250);
                GPIO_pinWrite(LED0_PIN, LOW);
                TASK_DELAY_MS(250);
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
    systemInitConfig_st s_sysConfig = {
        .pLogLevels_e = gDefaultLogLevels_ae,
        .logModulesCount_u8 = MODULES_MAX,
        .systemEventCallBack = app_eventsCallBackHandler,
        .pDeviceNamePrefixStr = DEVICE_NAME_PREFIX,
        .pAppVersionStr = APP_VERSION,

        .pWifiSsidStr = TEST_WIFI_SSID,
        .pWifiPwdStr = TEST_WIFI_PASSWORD,

        .s_awsConfig = {
            .hostNameStr = AWS_IOT_MQTT_HOST,
            .port_u16 = AWS_IOT_MQTT_PORT,
            .pRootCaStr = (char *)aws_root_ca_pem_start,
            .pClaimCertStr = (char *)claim_certificate_pem_crt_start,
            .pClaimPrivateKeyStr = (char *)claim_private_pem_key_start,
            .pClaimTemplateStr = AWS_PROVISION_TEMPLATE_NAME}};

    GPIO_pinMode(LED0_PIN, GPIO_MODE_OUTPUT, GPIO_PIN_INTR_DISABLE, NULL);
    GPIO_pinWrite(LED0_PIN, LOW);

    bool initSuccess = SYSTEM_init(&s_sysConfig);

    if (initSuccess)
    {
        SYSTEM_start();
    BaseType_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);
    if (pdPASS != err)
    {
        print_error("\n Error 0x%X in creating app_task \n restarting system\n\r\n\r", err);
        fflush(stdout);
        esp_restart();
    }
}
