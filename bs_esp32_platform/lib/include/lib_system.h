/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_system.h
 * \brief System library header file.
 *
 * The system library initializes the system by managing core operations,
 * including WiFi, BLE, MQTT, and HTTP communication for data transfers.
 * It also manages flash storage operations, error handling, device logs,
 * and supports OTA firmware updates.
 *
 * The libraries have been tested on the ESP32 modules.
 * Buildstorm explicitly denies responsibility for any hardware failures
 * arising from the use of these libraries, whether directly or indirectly.

 *
 * EULA LICENSE:
 * This library is licensed under end user license EULA agreement.
 * The EULA is available at https://buildstorm.com/eula/
 * For any support contact us at hello@buildstorm.com
 *
 */

#ifndef _LIB_SYSTEM_H_
#define _LIB_SYSTEM_H_

#include "lib_aws.h"
#include "lib_ota.h"
#include "lib_utils.h"
#include "lib_config.h"
#include "lib_delay.h"
#include "lib_print.h"
#include "lib_flash.h"
#include "lib_ble.h"

/**
 * @enum systemMode_et
 * An enum that represents System modes. The system will always be in one of
 * the following mode.
 */
typedef enum
{
    SYSTEM_MODE_IDLE,          /*!< System idle state */
    SYSTEM_MODE_TEST,          /*!< Test mode */
    SYSTEM_MODE_DEVICE_CONFIG, /*!< Device is waiting for configuration */
    SYSTEM_MODE_WIFI_CONFIG,   /*!< Device is waiting for configuration */
    SYSTEM_MODE_NORMAL,        /*!< Device is configured/provisioned */
    SYSTEM_MODE_OTA,           /*!< System is in OTA mode to perform firmware update */
    SYSTEM_MODE_ABORT,         /*!< System abort mode */
    SYSTEM_MODE_MAX            /*!< Total number of system modes */
} systemMode_et;

/**
 * @enum systemEvents_et
 * An enum that represents System events. The system notifes the application
 * about these events using a callback of type systemEventCb_t.
 */
typedef enum
{
    EVENT_WIFI_CONNECTED,       /*!< WiFi is connected */
    EVENT_WIFI_DISCONNECTED,    /*!< WiFi is disconnected */
    EVENT_BLE_CONNECTED,        /*!< Device is connected to BLE central device */
    EVENT_BLE_AUTHENTICATED,    /*!< A BLE xentral device has authenticated successfully */
    EVENT_BLE_DISCONNECTED,     /*!< Device is disconnected from BLE central device */
    EVENT_BLE_SEND_SUCCESS,     /*!< Packet was successfully sent from BLE*/
    EVENT_MQTT_CONNECTED,       /*!< Device is connected to MQTT broker */
    EVENT_MQTT_DISCONNECTED,    /*!< Device is disconnected from MQTT broker */
    EVENT_MQTT_PUBLISH_SUCCESS, /*!< Message was successfully published from MQTT*/
    EVENT_THING_PROVISIONED,    /*!< Device Provision complete*/
    EVENT_MAX                   /*!< Total number of system events */
} systemEvents_et;

/**
 * @brief System events callback function type. The application should
 * define the callback function and initialize the system configuration
 * with the callback function to receive the system events.
 */
typedef void (*systemEventCb_t)(systemEvents_et event_e);

/**
 * @brief System configuration structure. The application should define the
 * system configuration variable and configure it before calling
 * @ref SYSTEM_init function.
 */
typedef struct
{
    const logLevels_et *pLogLevels_e;           /*!< Desired log levels */
    uint8_t logModulesCount_u8;                 /*!< Total number of logging modules */
    char *pWifiSsidStr;                         /*!< WiFi SSID */
    char *pWifiPwdStr;                          /*!< WiFi password */
    char *pDeviceNamePrefixStr;                 // max 15 bytes
    char *pAppVersionStr;                       /*!< Application version */
    systemEventCb_t systemEventCallbackHandler; /*!< System event callback handler */
    mqttClientConfig_st s_mqttClientConfig;     /*!< AWS configuration */
} systemInitConfig_st;

/**
 * @brief Initiliaze the system with given configuration.
 * @param [in] s_pConfig Pointer to system configuration structure.
 * @returns Initialization status
 * @retval true on success
 * @retval false on failure
 */
bool SYSTEM_init(systemInitConfig_st *s_pConfig);
void SYSTEM_start();

/**
 * @brief Restarts device.
 * @param [in] restartDelay_u32 Delay (ms) after which system should be restarted
 * @retval none
 */
void SYSTEM_restart(uint32_t restartDelay_u32);
bool SYSTEM_isWaitingForRestart();

/**
 * @brief Get the system mode.
 * @param none
 * @returns The current mode of the system
 */
systemMode_et SYSTEM_getMode();

/**
 * @brief Get the system mode as string.
 * @returns The current mode of the system as string
 * @retval "IDLE" string for SYSTEM_MODE_IDLE
 * @retval "TEST" string for SYSTEM_MODE_TEST
 * @retval "DEVICE CONFIG" string for SYSTEM_MODE_DEVICE_CONFIG
 * @retval "WIFI CONFIG" string for SYSTEM_MODE_WIFI_CONFIG
 * @retval "NORMAL" string for SYSTEM_MODE_NORMAL
 * @retval "OTA" string for SYSTEM_MODE_OTA
 * @retval "ABORT" string for SYSTEM_MODE_ABORT
 */
const char *SYSTEM_getModeString();

/**
 * @brief Get the device name as registered.
 * @param none
 * @returns Thing name as string
 */
const char *SYSTEM_getDeviceName();

/**
 * @brief Get the MAC address of the device.
 * @param none
 * @returns Device's MAC address as string
 */
const char *SYSTEM_getMacAddr();

#endif //_LIB_SYSTEM_H_
