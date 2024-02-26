/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file lib_jobs.h
 * \brief AWS Jobs library header file.
 *
 * AWS Jobs library handles the job api's.
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

#ifndef LIB_JOBS_H_
#define LIB_JOBS_H_

#include "lib_utils.h"

#define LENGTH_JOB_DOCUMENT 256 // should be greaterr than LENGTH_OTA_URL
#define LENGTH_JOB_ID 32
#define LENGTH_JOB_CLIENT_TOKEN 32

/**
 * @enum jobsStatus_et
 * An enum to represent all possible Status of AWS Jobs.
 */
typedef enum
{
    JOB_STATUS_IN_PROGRESS,
    JOB_STATUS_SUCCESSED,
    JOB_STATUS_FAILED,
    JOB_STATUS_REJECTED,
    JOB_STATUS_MAX,
} jobsStatus_et;

/**
 * @brief Represents the structure used to handle AWS Jobs.
 */
typedef struct
{
    uint32_t queuedAt_u32;                 /*!< Timestamp when job was queued */
    uint32_t currentTimeStamp_u32;         /*!< Current timestamp */
    char idStr[LENGTH_JOB_ID];             /*!< Job Id */
    char documentStr[LENGTH_JOB_DOCUMENT]; /*!< Job document */
} job_st;

/**
 * @brief Job callback handler type. The application should provide
 * this handler to perform actions based on the Job created on AWS IoT.
 */
typedef jobsStatus_et (*jobCallback_t)(const job_st *ps_job);

/**
 * @brief Register an AWS IoT Job handler
 * @param [in] pJobActionStr A string to represent Job action
 * @param [in] timeoutMin_u8 The minimum timeout value when Job is running
 * @param [in] callbackHandler The callback handler for performing Job action
 */
bool JOBS_register(const char *pJobActionStr, uint8_t timeoutMin_u8, jobCallback_t callbackHandler);

/**
 * @brief Update the status of Job to AWS IoT.
 * @param [in] pJobIdStr Job ID as string
 * @param [in] status_e Status of the Job
 * @returns none
 */
void JOBS_updateStatus(char *pJobIdStr, jobsStatus_et status_e);

#endif /* LIB_JOBS_H_ */
