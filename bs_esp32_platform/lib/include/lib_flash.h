/**
* \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
*
* \file lib_flash.h
* \brief Flash library header file.
*
* This file defines the structure and enums used by the Flash library.
*
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

#ifndef _LIB_MODULE_FLASH_H_
#define _LIB_MODULE_FLASH_H_

/* Includes ------------------------------------------------------------------*/
#include "lib_utils.h"

/*---------------------------structures-------------------------------------*/
/**
 * @enum flashStatus_t
 * An enum to represent the status of flash operations.
 */
typedef enum
{
    FLASH_STATUS_OK,              /*!< Status ok */
    FLASH_STATUS_BUSY,            /*!< Status busy */
    FLASH_STATUS_LOAD_DEFAULT,    /*!< Load default configuration values */
    FLASH_STATUS_READ_FAIL,       /*!< Read failed */
    FLASH_STATUS_WRITE_FAIL,      /*!< Write failed */
    FLASH_STATUS_INVALID_DATASET, /*!< Data is invalid */
    FLASH_STATUS_MAX              /*!< Total number of status for Flash library */
} flashStatus_t;

/**
 * @enum deviceConfigParams_et
 * An enum to represent Device Configuration parameters.
 */
typedef enum
{
    DEVICE_CONFIG_HOST,       /*!< AWS IoT Host */
    DEVICE_CONFIG_PORT,       /*!< AWS IoT Port number */
    DEVICE_CONFIG_WIFI,       /*!< WIFI configuration parameter */
    DEVICE_CONFIG_THING_NAME, /*!< Thing name parameter */
    DEVICE_CONFIG_CLEAR,      /*!< Clear configurations */
    DEVICE_CONFIG_MAX         /*!< Total number of available configuration parameters */
} deviceConfigParams_et;

/**
 * @enum deviceCertType_et
 * An enum to represent Device Certificate type.
 */
typedef enum
{
    FLASH_CERT_CERTIFICATE_PEM, /*!< Device certificate */
    FLASH_CERT_PRIVATE_KEY,     /*!< Device private key */
    FLAH_CERT_MAX               /*!< Total number of device certificate types */
} deviceCertType_et;

/**
 * @enum flashDataSet_et
 * An enum to represent available Flash Datasets.
 */
typedef enum
{
    FLASH_DATASET_DEVICE_CONFIG,       /*!< Device configuration dataset */
    FLASH_DATASET_DEVICE_CERTIFICATES, /*!< Device certificate dataset */
    FLASH_DATASET_DEVICE_PRIVATE_KEY,  /*!< Device private key dataset */
    FLASH_DATASET_APP,                 /*!< Application dataset */
    FLASH_DATASET_MAX,                 /*!< Total number of available datasets */
} flashDataSet_et;

/* Function declarations ----------------------------------------------------*/

/**
 * @brief Check if a dataset is updated.
 * @param none
 * @returns Dataset update status.
 * @retval true if a dataset is updated
 * @retval false if not updated
 */
bool FLASH_isBusy();

/**
 * @brief Check if the device is registered.
 * @param none
 * @returns Device registration status.
 * @retval true is device is registered
 * @retval false if device is not registered yet.
 */
bool FLASH_isDeviceRegistered();

/**
 * @brief Read the data from application dataset.
 * @param [out] pData Data buffer
 * @param [in] pData Length of data
 * @returns Read status
 * @retval true on successful read
 * @retval false on failure
 */
bool FLASH_appDataRead(void *pData, uint16_t size_u16);

/**
 * @brief Store the given data to application data set.
 * @param [in] pData Data to be written
 * @param [in] size_u16 Length of data
 * @returns Write status
 * @retval true on success
 * @retval false on failure
 */
bool FLASH_appDataWrite(void *pData, uint16_t size_u16);

/**
 * @brief Erase stored application dataset.
 * @param none
 * @return Erase status
 * @retval true on success
 * @retval false on failure
 */
bool FLASH_appDataEraseAll();

#endif //_LIB_MODULE_FLASH_H_
