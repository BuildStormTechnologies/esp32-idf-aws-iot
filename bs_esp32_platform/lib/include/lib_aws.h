/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_aws.h
 * \brief An AWS library file
 *
 *  AWS library handles AWS IoT events like Publish, Subscribe,
 *  Shadow updates, Connection events, Device Provisioning... etc.
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

#ifndef _LIB_AWS_H_
#define _LIB_AWS_H_

#include "lib_config.h"
#include "lib_msg.h"
#include "lib_utils.h"
#include "lib_shadow.h"

/**
 * @enum awsIotStates_et
 * An enum that represents states of AWS module.
 * It will be in one of the following modes.
 */
typedef enum
{
    STATE_AWS_IDLE,             /*!< AWS idle state */
    STATE_AWS_INIT,             /*!< AWS init state */
    STATE_AWS_START,            /*!< AWS start state */
    STATE_AWS_PROVISIONING,     /*!< AWS device provisioning in progress */
    STATE_AWS_CONNECTED,        /*!< Device is connected to AWS */
    STATE_AWS_DISCONNECTED,     /*!< Device is disconnected from AWS */
    STATE_AWS_SUSPENDED,        /*!< AWS module is in suspended state */
    STATE_AWS_RESUME,           /*!< Resumes AWS module from suspended state */
    STATE_AWS_PROVISION_FAILED, /*!< AWS device provisioning has failed */
    STATE_AWS_MAX               /*!< Total number of AWS states */
} awsIotStates_et;

/**
 * @brief Subscribe message callback function type. The application should
 * define the callback function
 */
typedef void (*awsSubscribeCallback_t)(const char *pTopic, const char *pPayload);

/**
 * @brief AWS configuration structure used by the library.
 */
typedef struct
{
    char *pHostNameStr;                              /*!< AWS IoT Endpoint */
    char *pRootCaStr;                                /*!< Root CA certificate string */
    char *pThingCertStr;                             /*!< Thing ceritificate string */
    char *pThingPrivateKeyStr;                       /*!< Thing Private key string */
    char *pClaimCertStr;                             /*!< Claim certificate string */
    char *pClaimPrivateKeyStr;                       /*!< Claim Private key string */
    char *pClaimTemplateStr;                         /*!< Provisioning template name */
    const char *pThingNameStr;                       /*!< AWS IoT thing name */
    uint16_t port_u16;                               /*!< AWS IoT port number */
    uint8_t maxPubMsgToStore_u8;                     /*!< Number of publish messaged stored in Ringbuffer */
    uint8_t maxSubMsgToStore_u8;                     /*!< Number of Subscribe messages stored in Ringbuffer */
    uint8_t maxSubscribeTopics_u8;                   /*!< Number of Subscribe topics supported */
    uint8_t maxJobs_u8;                              /*!< Number of jobs supported */
    awsSubscribeCallback_t subscribeCallbackHandler; /*!< Callback function for subscribe messages */
} mqttClientConfig_st;

/**
 * @brief Check if the device is in connected state.
 * @param none
 * @returns AWS connection status
 * @retval true when connected
 * @retval false when not connected
 */
bool AWS_isConnected();

/**
 * @brief Get the current state of AWS module.
 * @param none
 * @returns Current state of AWS module @ref awsIotStates_et
 */
awsIotStates_et AWS_getState();

/**
 * @brief Get current state of AWS module as string.
 * @param none
 * @returns String version of AWS states.
 * @retval "IDLE" - for STATE_AWS_IDLE
 * @retval "INIT" - for STATE_AWS_INIT
 * @retval "START" - for STATE_AWS_START
 * @retval "PROVISIONING" - for STATE_AWS_PROVISIONING
 * @retval "CONNECTED" - for STATE_AWS_CONNECTED
 * @retval "DISCONNECTED" - for STATE_AWS_DISCONNECTED
 * @retval "SUSPENDED" - for STATE_AWS_SUSPENDED
 * @retval "RESUME" - for STATE_AWS_RESUME
 * @retval "PROVISION_FAILED" - for STATE_AWS_PROVISION_FAILED
 */
const char *AWS_getStateString();

/**
 * @brief Get thing name.
 * @param none
 * @returns The thing name as registed with AWS IoT.
 */
const char *AWS_getThingName();

/**
 * @brief Suspend AWS module.
 * @param none
 * @returns none
 */
void AWS_suspend();

/**
 * @brief Resume AWS module from suspended state.
 * @param none
 * @returns none
 */
void AWS_resume();

/**
 * @brief Disconnect and close the AWS IoT connection.
 * @param none
 * @returns none
 */
void AWS_close();

/**
 * @brief Close the AWS connection and Start new connection.
 * @param none
 * @returns none
 */
void AWS_restart();

/**
 * @brief Subscribe to a given Topic on AWS IoT.
 * @param [in] pTopicStr topic as string
 * @param [in] qos_e QOS level
 * @returns Subscription status
 * @retval true on success
 * @retval false on failure
 */
bool AWS_subscribe(char *pTopicStr, uint8_t qos_e);

/**
 * @brief Get number messages available in the subscription message buffer
 * @param none
 * @returns Number of messages available.
 */
uint16_t AWS_subMsgAvailable();

/**
 * @brief Read the available message from the subscription message buffer.
 * @param [out] mqttMsg_st MQTT message buffer to return a message
 * @returns Message read status
 * @retval true when message is read successfully from the buffer.
 * @retval false on errors
 */
bool AWS_subMsgRead(mqttMsg_st *ps_msg);

/**
 * @brief Publish given message to AWS IoT. The message is queued in a Publish
 * buffer, then gets published.
 * @param [in] mqttMsg_st MQTT message for publishing
 * @returns Publish status
 * @retval true when message is queued for publishing
 * @retval false on errors
 */
bool AWS_publish(mqttMsg_st *ps_msg);

/**
 * @brief  Get number messages available to publish in the Publish queue.
 * @param none
 * @returns  Number messages available for publishing
 */
uint16_t AWS_pubMsgAvailable();

/**
 * @brief Check if library is still publishing messages.
 * @param none
 * @returns Status to indicate whether publish is in progress or not
 * @retval true when library is publishing messages
 * @retval false when it's not publishing
 */
bool AWS_publishInProgress();

void AWS_getShadow();
void AWS_clearPubMsg();

#endif //_LIB_AWS_H_