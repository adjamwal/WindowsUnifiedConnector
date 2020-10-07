/**
 * @file ucidapi.h
 *
 * UCIDAPI has functions for use by clients to get the UCID and
 * its associated token. It is also possible to request for a refresh
 * of the token associated with UCID.
 *
 * The UCID and token are printable ASCII character sequences.
 *
 * The contents of UCID and token are opaque, and applications
 * should not infer any meaning from their contents.
 *
 * Memory Management:
 * \li The caller of the API is responsible for allocating and freeing
 *     memory passed into the API.
 *
 * Example usage:
 *
 * \code
 *
 * #include "ucidapi.h"
 *
 * void func_that_needs_ucid()
 * {
 *     int bufsz = 0;
 *
 *     // Get required size
 *     ucid_result_t res = ucid_get_id(NULL, &bufsz);
 *
 *     if (res == UCID_RES_INSUFFICIENT_LEN) {
 *         // Allocate memory of bufsz bytes
 *         char* myucid = (char*) malloc(bufsz);
 *         // make sure myucid is not NULL
 *         ...
 *
 *         // Get UCID
 *         res = ucid_get_id(myucid, &bufsz);
 *     }
 *
 *     if (res != UCID_RES_SUCCESS) {
 *         // Handle failure
 *         return;
 *     }
 *
 *     // Use myucid
 *     // NOTE: bufsz includes 1 for terminating NUL
 * }
 *
 * \endcode

 * Copyright (c) 2020 Cisco Systems, Inc. All rights reserved.
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#   ifdef UCID_CAPI_EXPORTS
#       define UCID_CAPI __declspec(dllexport)
#   else
#       define UCID_CAPI __declspec(dllimport)
#   endif
#else
#   ifdef UCID_CAPI_EXPORTS
#       define UCID_CAPI __attribute__((visibility("default")))
#   else
#       define UCID_CAPI
#   endif
#endif

#ifndef IN
#   define IN
#endif

#ifndef OUT
#   define OUT
#endif

/**
 * An enumeration to indicate the result of a UCID API call.
 */
typedef enum ucid_result_e {
    UCID_RES_SUCCESS = 0,            ///< the call succeeded
    UCID_RES_GENERAL_ERROR = -1,     ///< the call failed due to an error other
                                     ///  than these listed
    UCID_RES_NOT_INITED = -2,        ///< the API is not ready
    UCID_RES_INVALID_ARG = -3,       ///< an argument passed to the API
                                     ///    is invalid
    UCID_RES_INSUFFICIENT_LEN = -4,  ///< the length of the memory block
                                     ///    is not sufficient
    UCID_RES_AGENT_ERROR = -5,       ///< problem when communicating
                                     ///    with the agent
    UCID_RES_CLOUD_ERROR = -6,       ///< problem in agent's communication
                                     ///    with the cloud
    UCID_RES_CLOUD_FAILURE = -7,     ///< the cloud returned a failure response
} ucid_result_t;

/**
 * Get the UCID.
 *
 * Copies UCID to the memory pointed to by \c p_id. The terminating NUL
 * character is also copied.
 *
 * If \c p_buflen is NULL, then returns \c UCID_RES_INVALID_ARG.
 * If \c p_id is NULL, then updates \c *p_buflen with the size in bytes (including
 *               1 for the terminating NUL) needed to store UCID.
 * If both \c p_id and \c p_buflen are not NULL, then \c *p_buflen should contain the
 *               size in bytes pointed to by \c p_id. The UCID
 *               (including the terminating NUL) is copied to \c p_id
 *               and \c *p_buflen is updated with the size of the UCID
 *               (including 1 for terminating NUL).
 *
 * @param[in,out] p_id pointer to memory that can store the UCID
 * @param[in,out] p_buflen a non-NULL pointer to an integer
 *
 * @return \li UCID_RES_SUCCESS if the call is successful.
 *         \li UCID_RES_INVALID_ARG if \c p_buflen is NULL.
 *         \li UCID_RES_NOT_INITED if UCID is not yet available.
 *         \li UCID_RES_INSUFFICIENT_LEN if \c p_id is NULL, OR
 *                                       if \c p_id is not NULL and \c *p_buflen
 *                                       does not have a value
 *                                           >= (size of UCID in bytes
 *                                               +1 for terminating NUL);
 *                                 \c *p_buflen is updated with the required size
 *                                 (including 1 for the terminating NUL).
 *         \li UCID_RES_GENERAL_ERROR if any other error occurs.
 */
UCID_CAPI
ucid_result_t
ucid_get_id(IN OUT char* p_id, IN OUT int* p_buflen);

/**
 * Get the token that is associated with the UCID.
 *
 * Copies the token associated with the UCID to the memory pointed to by
 * \c p_token. The terminating NUL character is also copied.
 *
 * If \c p_buflen is NULL, then returns \c UCID_RES_INVALID_ARG.
 * If \c p_token is NULL, then updates \c *p_buflen with the size in bytes
 *               (including 1 for the terminating NUL) needed
 *               to store the token.
 * If both \c p_token and \c p_buflen are not NULL, then \c *p_buflen should contain
 *               the size in bytes pointed to by \c p_token. The token
 *               (including the terminating NUL) is copied to \c p_token
 *               and \c *p_buflen is updated with the size of the token
 *               (including 1 for the terminating NUL).
 *
 * @param[in,out] p_token pointer to memory that can store the token
 * @param[in,out] p_buflen a non-NULL pointer to an integer
 *
 * @return \li UCID_RES_SUCCESS if the call is successful.
 *         \li UCID_RES_INVALID_ARG if \c p_buflen is NULL.
 *         \li UCID_RES_NOT_INITED if token is not yet available.
 *         \li UCID_RES_INSUFFICIENT_LEN if \c p_token is NULL, OR
 *                                       if \c p_token is not NULL and \c *p_buflen
 *                                       does not have a value
 *                                           >= (size of token in bytes
 *                                               +1 for terminating NUL);
 *                                 \c *p_buflen is updated with the required size
 *                                 (including 1 for the terminating NUL).
 *         \li UCID_RES_GENERAL_ERROR if any other error occurs.
 */
UCID_CAPI
ucid_result_t
ucid_get_token(IN OUT char* p_token, IN OUT int* p_buflen);

/**
 * Refresh the token that is associated with the UCID.
 *
 * This call blocks until it gets a response.
 *
 * @return \li UCID_RES_SUCCESS if the call is successful.
 *         \li UCID_RES_AGENT_ERROR if there is a problem when communicating
 *                                    with the agent.
 *         \li UCID_RES_CLOUD_ERROR if there is a problem in agent's
 *                                    communication with the cloud.
 *         \li UCID_RES_CLOUD_FAILURE if the cloud returned a failure response.
 *         \li UCID_RES_GENERAL_ERROR if any other error occurs.
 */
UCID_CAPI
ucid_result_t
ucid_refresh_token();

#ifdef __cplusplus
}
#endif
