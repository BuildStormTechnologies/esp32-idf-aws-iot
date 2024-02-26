/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_ota.h
 * \brief OTA library header file.
 *
 * The ota library provides the API's for ota operations
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

#ifndef _LIB_OTA_H_
#define _LIB_OTA_H_

/* Includes ------------------------------------------------------------------*/
#include "esp_err.h"
#include "lib_utils.h"

// TODO: update the library using new apis. refer esp-idf ota example

/**
 * @enum firmUpdateType_et
 * Types of supported firmware updates.
 */
typedef enum
{
    OTA_UPDATE_TYPE_NONE,  /*!< None */
    OTA_UPDATE_TYPE_LOCAL, /*!< Perform local update */
    OTA_UPDATE_TYPE_WEB,   /*!< Perform web update */
    OTA_UPDATE_TYPE_MAX    /*!< Total number of firmware update types */
} firmUpdateType_et;

/* Function declarations ----------------------------------------------------*/

/**
 * @brief Get OTA progress in percentage.
 * @param none
 * @returns Progress value in percentage.
 */
uint8_t OTA_getProgressPercentage();

/**
 * @brief Check if OTA is in progress.
 * @param none
 * @returns Status to indicate if OTA is in progress
 * @retval true if in progress
 * @retval false if not in progress
 */
bool OTA_inProgress();

#endif //_LIB_OTA_H_