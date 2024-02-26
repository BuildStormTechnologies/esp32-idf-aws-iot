/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file app_config.h
 * \brief app_config header file.
 *
 * The app_config Contains macros for the application.
 *
 * The libraries have been tested on the ESP32 modules.
 * Buildstorm explicitly denies responsibility for any hardware failures
 * arising from the use of these libraries, whether directly or indirectly.
 * Please note that files are subject to change without prior notice.
 *
 * EULA LICENSE:
 * This library is licensed under end user license EULA agreement.
 * The EULA is available at https://buildstorm.com/eula/
 * For any support contact us at hello@buildstorm.com
 *
 */

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#include "stdbool.h"
#include "stdint.h"
#include "lib_config.h"

#define APP_MODULE_MAIN LIB_MODULE_MAX
//-------------------------APP CONFIG--------------------------/

#define TASK_APP_PRIORITY 4
#define TASK_APP_STACK_SIZE (5 * 1024) // reduce it to 4096

#define TEST_WIFI_SSID "Wifi SSID"
#define TEST_WIFI_PASSWORD "Wifi Password"

#define AWS_IOT_MQTT_HOST "Your AWS host"
#define AWS_IOT_MQTT_PORT 8883

#define APP_VERSION "1.0.0"

#define TEST_AWS_TOPIC_PUBLISH "testPub/ESP32"
#define TEST_AWS_TOPIC_SUBSCRIBE "testSub/ESP32"

#endif //_APP_CONFIG_H_
