/**
 * \copyright Copyright (c) 2021, Buildstorm Pvt Ltd
 *
 * \file lib_aws.h
 * \brief An AWS library file
 *
 *  AWS library handles AWS IoT events like Publish, Subscribe,
 *  Shadow updates, Connection events, Device Provisioning... etc.
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
    SHADOW_VALUE_TYPE_FLOAT, /*!< Shadow value type as float */
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
    SHADOW_UPDATE_TYPE_ALL,      /*!< Update both Desired & Reported
                                    states */
    SHADOW_UPDATE_TYPE_MAX,      /*!< Total number for update types */
} shadowUpdateType_et;


/**
 * @struct value_st
 * This is a shadow value type.
 */
typedef struct
{
    int32_t val_i32;                    /*!< Value of type int32_t */
    float val_f32;                      /*!< Value of type float */
    char str[LENGTH_AWS_SHADOW_BUFFER]; /*!< Value of type string */
} value_st;


/**
 * @brief AWS Shadow structure used by the library to update classic shadow.
 * This will update the shadow for one element without needing to register
 * No callbacks will be sent represent only one shadow element within a thing shadow.
 */
typedef struct
{
    char keyStr[LENGTH_AWS_SHADOW_KEY]; /*!< A key of the shadow element */
    value_st value_e;                   /*!< A value of the shadow element */
    shadowValueType_et valType_e;       /*!< A type of value required for this shadow element */
} awsThingShadow_st;




/**
 * @brief Shadow update callback function type. The application should
 * define the callback function and intialize it in the @ref awsShadow_st
 * configuration.
 */
typedef void (*awsShadowUpdateCallBack_t)(const char *pKeyStr, const void *pValue);

/**
 * @brief AWS Shadow structure used by the library to handle shadow updates.
 * You should initialize the callback handler and register this awsShadow_st
 * by calling @ref SHADOW_deltaRegister function to receive callbacks
 * whenever the shadow element is updated.
 *
 * This structure represent only one shadow element within a thing shadow.
 */
typedef struct
{
    shadowValueType_et valType_e;              /*!< A type of value required for this shadow element */
    char keyStr[LENGTH_AWS_SHADOW_KEY];        /*!< A key of the shadow element */
    value_st value_e;                          /*!< A value of the shadow element */
    uint8_t isUpdated_b8;                      /*!< Flag to indicate update */
    uint8_t needToPublish_b8;                  /*!< Flag to indicate data needs to be published */
    shadowUpdateType_et shadowUpdateType_e;    /*!< Flag to indicate update type desired/reported */
} awsShadow_st;
#define MAX_ELEMENTS_PER_SHADOW 10

typedef struct{
  char *ptrShadowName;
  uint8_t maxElementCount_u8;
  awsShadowUpdateCallBack_t callbackHandler; /*!< Callback handler to be called when shadow element is updated */
  awsShadow_st *pShadowElementsTable;
}shadowConfigTable_st;


typedef struct{
    char* shadowTypePtr;
    char* pshadowValueMemoryTable;
    tagStructure_st *ps_tagForShadowElements;
    awsShadow_st *pas_shadowTable;
    awsShadowUpdateCallBack_t shadowUpdateCallbackHandler; /*!< Callback handler to be called when shadow element is updated */
    uint8_t maxElementCountPerShadow_u8;
}shadowTagHandler_st;


bool SHADOW_register(shadowConfigTable_st *pShadowtable, uint8_t maxShadows);

/**
 * @brief Register a shadow element to receive callbacks whenever the shadow
 * element is updated with a new value.
 * @param [in] awsShadow_st Pointer to shadow element configuration
 * @returns Status to indicate if the shadow element is registered
 * @retval true registered successfully
 * @retval false when failed
 */
bool SHADOW_deltaRegister(awsShadow_st *ps_shadow);

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
bool SHADOW_update(uint8_t shadowIndex, char *pKeyStr, void *pValue, shadowUpdateType_et updateType_e);
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


#endif