/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_delay.h
 * \brief Header file for delay.
 *
 * The delay library handles the delay api's.
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

#ifndef LIB_DELAY_H_
#define LIB_DELAY_H_

#include "freertos/FreeRTOS.h"
#include "lib_utils.h"

/* Macros -----------------------------------------------------------------*/
#define MS_TO_RTOS_TICKS(x) ((x) >= portTICK_PERIOD_MS ? ((x) / portTICK_PERIOD_MS) : portTICK_PERIOD_MS)
#define TASK_DELAY_MS(x) vTaskDelay(MS_TO_RTOS_TICKS(x))

/**
 * @brief Get time elapsed in milli seconds since start-up.
 * @param none
 * @returns Number of milli seconds
 */
uint32_t millis();

/**
 * @brief Get time elapsed in micro seconds since start-up.
 * @param none
 * @returns Number of micro seconds
 */
int64_t micros();

/**
 * @brief Delay, wait until given time period.
 * @param [in] waitTime_u32 Number of milli-seconds to wait
 * @returns none
 */
void delay(uint32_t waitTime_u32);

#endif // LIB_DELAY_H_
