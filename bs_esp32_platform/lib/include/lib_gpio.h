/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_gpio.h
 * \brief GPIO library header file.
 *
 * The gpio library handles gpio pin operations.
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

#ifndef _LIB_GPIO_H_
#define _LIB_GPIO_H_

/* Includes ------------------------------------------------------------------*/

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "lib_utils.h"

/* Macros -----------------------------------------------------------------*/
#define GPIO_pinWrite(gpioNum, state) gpio_set_level(gpioNum, state)
#define GPIO_pinRead(gpioNum) gpio_get_level(gpioNum)
/* User Defined Data Types ------------------------------------------------*/

/**
 * @brief GPIO interrupt handler callback function. The application should
 * define it and provide it in @ref GPIO_pinMode as parameter.
 */
typedef void (*gpio_intr_handler)(void *arg);

/* Function declarations ----------------------------------------------------*/

/**
 * @brief Configure GPIO pin, mode, interrupt & interrupt handler.
 * @param gpioNum GPIO number
 * @param mode GPIO mode
 * @param intrType GPIO interrupt type
 * @param handler GPIO interrupt callback handler
 * @returns none
 */
void GPIO_pinMode(gpio_num_t, gpio_mode_t, gpio_int_type_t, gpio_intr_handler);

#endif //_LIB_GPIO_H_