/**
* \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
*
* \file lib_wifi.h
* \brief Wifi library header file.
*
*
* The Wifi library handles the Wifi operations.
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

#ifndef _LIB_WIFI_H_
#define _LIB_WIFI_H_

/* Includes ------------------------------------------------------------------*/

#include "lib_config.h"
#include "lib_utils.h"

/* Macros -----------------------------------------------------------------*/

#define ENABLE_WIFI_VERIFICATION TRUE

/**
 * @enum wifiStates_et
 * An enum to represnet WiFi states
 */
typedef enum
{
    STATE_WIFI_IDLE,                    /*!< Idle state */
    STATE_WIFI_SCAN,                    /*!< Scanning for SSIDs */
    STATE_WIFI_START,                   /*!< Start WiFi */
    STATE_WIFI_START_SERVER,            /*!< Start WiFi, AP Server */
    STATE_WIFI_DISCONNECTED,            /*!< WiFi disconnected */
    STATE_WIFI_CONNECTED,               /*!< WiFi connected */
    STATE_WIFI_SERVER_STA_CONNECTED,    /*!< WiFi connected */
    STATE_WIFI_SERVER_STA_DISCONNECTED, /*!< WiFi connected */
    STATE_WIFI_MAX,                     /*!< Total number of states */
} wifiStates_et;

/**
 * @brief WiFi credentials
 */
typedef struct
{
    char ssid[LENGTH_WIFI_CRED_SSID];         /*!< WiFi SSID */
    char password[LENGTH_WIFI_CRED_PASSWORD]; /*!< WiFi Password */
} wifiCred_st;

/* Function declarations ----------------------------------------------------*/

/**
 * @brief Check if WiFi is connected.
 * @param none
 * @returns WiFi connection status
 * @retval true if connected
 * @retval false if not connected
 */
bool WIFI_isConnected();

/**
 * @brief Get the IP address.
 * @param none
 * @returns IP address as string
 */
char *WIFI_getIpAddress();

/**
 * @brief Get WiFi signal strength.
 * @param none
 * @returns WiFi signal strength
 */
uint8_t WIFI_getSignalStrength();

/**
 * @brief Get WiFi RSSI.
 * @param none
 * @returns RSSI value
 */
int8_t WIFI_getRssi();

/**
 * @brief Get WiFi state as string.
 * @param none
 * @returns WiFi state as string.
 * @retval "IDLE" - for STATE_WIFI_IDLE
 * @retval "SCAN" - for STAT_WIFI_SCAN
 * @retval "START" - for STATE_WIFI_START
 * @retval "DISCONNECTED" - for STATE_WIFI_DISCONNECTED
 * @retval "CONNECTED" - for STATE_WIFI_CONNECTED
 */
const char *WIFI_getStateString();
wifiStates_et WIFI_getState();
const char *WIFI_getSSID();

#endif //_LIB_WIFI_H_