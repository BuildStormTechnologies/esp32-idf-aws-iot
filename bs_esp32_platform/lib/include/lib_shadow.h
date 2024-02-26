/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_shadow.h
 * \brief Shadow library file
 *
 *  Shadow library handles AWS IoT events like Shadow updates
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

#ifndef _LIB_SHADOW_H_
#define _LIB_SHADOW_H_

#include "lib_config.h"
#include "lib_msg.h"
#include "lib_utils.h"
#include "lib_json.h"

/**
 * @enum shadowValueType_et
 * An enum that represents all possible shadow value types.
 */
typedef enum
{
    SHADOW_VALUE_TYPE_INT,    /*!< Shadow value type as integer */
    SHADOW_VALUE_TYPE_FLOAT,  /*!< Shadow value type as float */
    SHADOW_VALUE_TYPE_STRING, /*!< Shadow value type as string */
    SHADOW_VALUE_TYPE_MAX,    /*!< Total number of shadow value types */
} shadowValueType_et;

/**
 * @enum shadowUpdateType_et
 * An enum that represents all possible shadow update types.
 */
typedef enum
{
    SHADOW_UPDATE_TYPE_DESIRED,  /*!< Update Desired state */
    SHADOW_UPDATE_TYPE_REPORTED, /*!< Update Reported state */
    SHADOW_UPDATE_TYPE_ALL,      /*!< Update both Desired & Reported states */
    SHADOW_UPDATE_TYPE_MAX,      /*!< Total number for update types */
} shadowUpdateType_et;

/**
 * @struct value_st
 * This is a shadow value type.
 */
typedef struct
{
    int32_t val_i32;  /*!< Value of type int32_t */
    float val_f32;    /*!< Value of type float */
    const char *pStr; /*!< Value of type string */
} value_st;

/**
 * @brief AWS Shadow structure used by the library to update classic shadow.
 * This will update the shadow for one element without needing to register
 * No callbacks will be sent represent only one shadow element within a thing shadow.
 */
typedef struct
{
    char keyStr[LENGTH_AWS_SHADOW_KEY]; /*!< A key of the shadow element */
    value_st s_value;                   /*!< A value of the shadow element */
    shadowValueType_et valType_e;       /*!< A type of value required for this shadow element */
} awsThingShadow_st;

/**
 * @brief Shadow update callback function type. The application should
 * define the callback function and intialize it in the @ref awsShadowElement_st
 * configuration.
 */
typedef void (*awsShadowUpdateCallBack_t)(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue);

/**
 * @brief AWS Shadow structure used by the library to handle shadow updates.
 * You should initialize the callback handler and register this awsShadowElement_st
 * by calling @ref SHADOW_deltaRegister function to receive callbacks
 * whenever the shadow element is updated.
 *
 * This structure represent only one shadow element within a thing shadow.
 */
typedef struct
{
    const shadowValueType_et valType_e;           /*!< A type of value required for this shadow element */
    const char *keyStr;                           /*!< A key of the shadow element */
    const value_st s_value;                       /*!< A value of the shadow element */
    const uint8_t needToPublish_b8;               /*!< Flag to indicate data needs to be published */
    const shadowUpdateType_et shadowUpdateType_e; /*!< Flag to indicate update type desired/reported */
} awsShadowElement_st;

typedef struct
{
    const char *ptrShadowName;
    const uint8_t maxElementCount_u8;
    const awsShadowUpdateCallBack_t callbackHandler; /*!< Callback handler to be called when shadow element is updated */
    const awsShadowElement_st *pShadowElementsTable;
} shadowConfigTable_st;

/**
 * @brief Register a shadow elements to receive callbacks whenever the shadow
 * element is updated with a new value.
 * @param [in] shadowConfigTable_st Pointer to shadow configuration
 * @returns Status to indicate if the shadow element is registered
 * @retval true registered successfully
 * @retval false when failed
 */
bool SHADOW_register(const shadowConfigTable_st *pShadowtable, uint8_t maxShadows);

/**
 * @brief Update the shadow element.
 * @param [in] shadowIndex (0-Classic, 1--n Named Shadows)
 * @param [in] pKeyStr A key representing shadow element
 * @param [in] pValue New value of the shadow element
 * @param [in] shadowUpdateType_et Type of shadow update
 * @returns Shadow update status
 * @retval true on success
 * @retval false on failure
 */
bool SHADOW_update(uint8_t shadowIndex, const char *pKeyStr, void *pValue, shadowUpdateType_et updateType_e);
/**
 * @brief Print the status of AWS IoT.
 * @param none
 * @returns none
 */

/**
 * @brief Update device shadow.
 * @param [in] shadowIndex (0-Classic, 1--n Named Shadows)
 * @param [in] awsThingShadow_st The Device shadow
 * @param [in] maxKeys_u8 Max keys to update
 * @param [in] shadowUpdateType_et Type of shadow update
 * @returns Shadow update status
 * @retval true on success
 * @retval false on failure
 */
bool SHADOW_documentUpdate(uint8_t shadowIndex, awsThingShadow_st as_thingShadow[], uint8_t maxKeys_u8, shadowUpdateType_et updateType_e);

#endif //_LIB_SHADOW_H_