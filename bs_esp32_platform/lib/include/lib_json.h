/**
* \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
*
* \file lib_json.h
* \brief Json library header file.
*
* The JSON library provides the API's form JSON parsing
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

#ifndef _LIB_JSON_H
#define _LIB_JSON_H

#include "lib_utils.h"
#include "lib_config.h"

#define LENGTH_KEY_SIZE 32 // Max 3-char keyStr ex:"typ", "dat", "cmd", "sts" etc

/**
 * @brief A structure to represent key-value pairs in a JSON string.
 */
typedef struct
{
    char *keyStr;    /*!< A key */
    char *pValueStr; /*!< Value associated with the key */
} tagStructure_st;

#define MAX_JSON_ARRAY_OBJS 10
#define MAX_JSON_ARRAY_INTEGERS 10

/**
 * @brief A structure to represent array of JSON objects.
 */
typedef struct
{
    uint8_t numOfJosnObjs_u8;                /*!< Number of JSON objects in array */
    char jsonObjs[MAX_JSON_ARRAY_OBJS][150]; /*!< Array of objects */
} jsonArray_st;

/**
 * @brief A structure to represent array of integers
 */
typedef struct
{
    uint32_t arrayElements[MAX_JSON_ARRAY_INTEGERS]; /*!< Array of integaers */
} jsonArrayList_st;

/**
 * @brief Extract the key-value pairs sepcified by tags from a give JSON string.
 * @param [in] pJsonStr The JSON string
 * @param [inout] tags An array of key-value pairs to be extracted
 * @param [in] maxKeys_u8 The maximum number of keys to be extracted
 * @param [in] clearvalueFlag_u8 Clear the value buffers
 * @returns Status of extraction
 * @retval true on success
 * @retval false on failure
 */
uint8_t JSON_processString(const char *pJsonStr, const tagStructure_st tags[], uint8_t maxKeys_u8, uint8_t clearvalueFlag_u8);

/**
 * @brief Extract an array of JSON objects from given JSON string.
 * @param [in] pJsonStr The JSON strong
 * @param [inout] jsnArrPtr An array of JSON objects
 * @returns Status of extraction
 * @retval true on success
 * @retval false on failure
 */
uint8_t JSON_processArrayObjs(char *pJsonStr, jsonArray_st *jsnArrPtr);

/**
 * @brief Extract an array of JSON objects from given JSON string.
 * @param [in] pJsonStr The JSON strong
 * @param [inout] jsnArrPtr Pointer of structure of Array of uint32 member
 * @returns Status of extraction
 * @retval Number of Array elements found
 * @retval false on failure
 */
uint8_t JSON_processArrayOfIntegers(char *pJsonStr, jsonArrayList_st *jsnArrStr);

#endif //_LIB_JSON_H
