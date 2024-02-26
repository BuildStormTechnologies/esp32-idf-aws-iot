/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_print.h
 * \brief Print library header file.
 *
 * The print library provides the API's for print operations
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

#ifndef _LIB_PRINT_H_
#define _LIB_PRINT_H_

#include "esp_system.h"
#include "esp_types.h"
#include "lib_config.h"
#include "lib_utils.h"

/***************************************************************************************************
                             Function Prototypes
 ***************************************************************************************************/

/**
 * @brief Set the log level of a given library file.
 * @param module_e The library file for which log level must be set
 * @param logLevel_e Desired log level
 * @returns none
 */
void print_setLogLevel(menusLibModule_et module_e, logLevels_et logLevel_e);

/**
 * @brief Set the given log levels as default.
 * @param maxModules_u8 Maximum number of modules
 * @param pLogLevels_e Pointer to Log levels
 * @returns none
 */
void print_setDefaultLogLevel(uint8_t maxModules_u8, const logLevels_et *pLogLevels_e);

/**
 * @brief Print the log message on serial terminal.
 * @param module_e Library module
 * @param logLevel_e Log level
 * @param pArgListStr List of arguments
 * @returns none
 */
void print_serial(menusLibModule_et module_e, logLevels_et logLevel_e, const char *pFunNameStr, const char *pArgListStr, ...);

/**
 * @brief Check if logs are enabled
 * @param none
 * @returns Status of logs is enabled or not
 * @retval true if logs are enabled
 * @retval false if logs are not enabled
 */
bool print_isLogsEnabled();

/**
 * @brief Enable logs
 * @param none
 * @returns none
 */
void print_enableLogs();

/**
 * @brief Disable logs
 * @param none
 * @returns none
 */
void print_disableLogs();

/**
 * @brief Print fail message
 * @param pFunNameStr : Name of the calling function using the attribute __func__
 * @param failMsg : failure msg to print
 * @returns none
 */
void print_failedMsg(const char *pFunNameStr, const char *failMsg);

/**
 * @brief Print malloc failed message
 * @param pFunNameStr : Name of the calling function using the attribute __func__
 * @param failMsg : failure msg to print
 * @returns none
 */
void print_mallocFailedMsg(const char *pFunNameStr, const char *failMsg);

#define print_verbose(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_VERBOSE, __func__, var_arg_list)
#define print_error(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_ERROR, __func__, var_arg_list)
#define print_info(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_INFO, __func__, var_arg_list)
#define print_debug(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_DEBUG, __func__, var_arg_list)

#define print_verbose_raw(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_VERBOSE, NULL, var_arg_list)
#define print_error_raw(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_ERROR, NULL, var_arg_list)
#define print_info_raw(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_INFO, NULL, var_arg_list)
#define print_debug_raw(var_arg_list...) print_serial(thisModule, PRINT_LEVEL_DEBUG, NULL, var_arg_list)

#define print_failed(failMsg) print_failedMsg(__func__, failMsg)
#define print_mallocFailed(failMsg) print_mallocFailedMsg(__func__, failMsg)

#define PRINTF printf

/**************************************************************************************************/

#endif //_LIB_PRINT_H_
