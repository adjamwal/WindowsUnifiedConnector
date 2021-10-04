/**
 * @file cmidapi.h
 *
 * CMIDAPI has functions for use by clients to get the Business ID, CMID and
 * its associated token. It is also possible to request for a refresh
 * of the token associated with CMID. Additionaly, the event, catalog and checkin
 * URLs can also be requested.
 *
 * The CMID, Business ID and token are printable ASCII character sequences.
 *
 * The contents of CMID, Business ID and token are opaque, and applications
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
 * #include "CMIDAPI.h"
 *
 * void func_that_needs_cmid()
 * {
 *     int bufsz = 0;
 *
 *     // Get required size
 *     cmid_result_t res = cmid_get_id(NULL, &bufsz);
 *
 *     if (res == CMID_RES_INSUFFICIENT_LEN) {
 *         // Allocate memory of bufsz bytes
 *         char* mycmid = (char*) malloc(bufsz);
 *         // make sure mycmid is not NULL
 *         ...
 *
 *         // Get CMID
 *         res = cmid_get_id(mycmid, &bufsz);
 *     }
 *
 *     if (res != CMID_RES_SUCCESS) {
 *         // Handle failure
 *         return;
 *     }
 *
 *     // Use mycmid
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
#   ifdef CMID_CAPI_EXPORTS
#       define CMID_CAPI __declspec(dllexport)
#   else
#       define CMID_CAPI __declspec(dllimport)
#   endif
#else
#   ifdef CMID_CAPI_EXPORTS
#       define CMID_CAPI __attribute__((visibility("default")))
#   else
#       define CMID_CAPI
#   endif
#endif

#ifndef IN
#   define IN
#endif

#ifndef OUT
#   define OUT
#endif

/**
 * An enumeration to indicate the result of a CMID API call.
 */
typedef enum cmid_result_e {
    CMID_RES_SUCCESS = 0,            ///< the call succeeded
    CMID_RES_GENERAL_ERROR = -1,     ///< the call failed due to an error other
                                     ///  than these listed
    CMID_RES_NOT_INITED = -2,        ///< the API is not ready
    CMID_RES_INVALID_ARG = -3,       ///< an argument passed to the API
                                     ///    is invalid
    CMID_RES_INSUFFICIENT_LEN = -4,  ///< the length of the memory block
                                     ///    is not sufficient
    CMID_RES_AGENT_ERROR = -5,       ///< problem when communicating
                                     ///    with the agent
    CMID_RES_CLOUD_ERROR = -6,       ///< problem in agent's communication
                                     ///    with the cloud
    CMID_RES_CLOUD_FAILURE = -7,     ///< the cloud returned a failure response
} cmid_result_t;

/**
 * An enumeration to indicate the URL types returned by cmid_get_url.
 */
typedef enum cmid_url_type_e {
    CMID_EVENT_URL = 1,         ///< indicates the un-versioned event url
    CMID_CHECKIN_URL = 2,       ///< indicates the check-in url
    CMID_CATALOG_URL = 3,       ///< indicates the catalog url
} cmid_url_type_t; 

/**
 * Get the CMID.
 *
 * Copies CMID to the memory pointed to by \c p_id. The terminating NUL
 * character is also copied.
 *
 * If \c p_buflen is NULL, then returns \c CMID_RES_INVALID_ARG.
 * If \c p_id is NULL, then updates \c *p_buflen with the size in bytes (including
 *               1 for the terminating NUL) needed to store CMID.
 * If both \c p_id and \c p_buflen are not NULL, then \c *p_buflen should contain the
 *               size in bytes pointed to by \c p_id. The CMID
 *               (including the terminating NUL) is copied to \c p_id
 *               and \c *p_buflen is updated with the size of the CMID
 *               (including 1 for terminating NUL).
 *
 * @param[in,out] p_id pointer to memory that can store the CMID
 * @param[in,out] p_buflen a non-NULL pointer to an integer
 *
 * @return \li CMID_RES_SUCCESS if the call is successful.
 *         \li CMID_RES_INVALID_ARG if \c p_buflen is NULL.
 *         \li CMID_RES_NOT_INITED if CMID is not yet available.
 *         \li CMID_RES_INSUFFICIENT_LEN if \c p_id is NULL, OR
 *                                       if \c p_id is not NULL and \c *p_buflen
 *                                       does not have a value
 *                                           >= (size of CMID in bytes
 *                                               +1 for terminating NUL);
 *                                 \c *p_buflen is updated with the required size
 *                                 (including 1 for the terminating NUL).
 *         \li CMID_RES_GENERAL_ERROR if any other error occurs.
 */
CMID_CAPI
cmid_result_t
cmid_get_id(IN OUT char* p_id, IN OUT int* p_buflen);

/**
 * Get the token that is associated with the CMID.
 *
 * Copies the token associated with the CMID to the memory pointed to by
 * \c p_token. The terminating NUL character is also copied.
 *
 * If \c p_buflen is NULL, then returns \c CMID_RES_INVALID_ARG.
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
 * @return \li CMID_RES_SUCCESS if the call is successful.
 *         \li CMID_RES_INVALID_ARG if \c p_buflen is NULL.
 *         \li CMID_RES_NOT_INITED if token is not yet available.
 *         \li CMID_RES_INSUFFICIENT_LEN if \c p_token is NULL, OR
 *                                       if \c p_token is not NULL and \c *p_buflen
 *                                       does not have a value
 *                                           >= (size of token in bytes
 *                                               +1 for terminating NUL);
 *                                 \c *p_buflen is updated with the required size
 *                                 (including 1 for the terminating NUL).
 *         \li CMID_RES_GENERAL_ERROR if any other error occurs.
 */
CMID_CAPI
cmid_result_t 
cmid_get_token(IN OUT char* p_token, IN OUT int* p_buflen);

/**
 * Refresh the token that is associated with the CMID.
 *
 * This call blocks until it gets a response.
 *
 * @return \li CMID_RES_SUCCESS if the call is successful.
 *         \li CMID_RES_AGENT_ERROR if there is a problem when communicating
 *                                    with the agent.
 *         \li CMID_RES_CLOUD_ERROR if there is a problem in agent's
 *                                    communication with the cloud.
 *         \li CMID_RES_CLOUD_FAILURE if the cloud returned a failure response.
 *         \li CMID_RES_GENERAL_ERROR if any other error occurs.
 */
CMID_CAPI
cmid_result_t
cmid_refresh_token();

/**
* Get the Business ID.
*
* Copies Business ID to the memory pointed to by \c p_bid. The terminating NUL
* character is also copied.
*
* If \c p_buflen is NULL, then returns \c CMID_RES_INVALID_ARG.
* If \c p_bid is NULL, then updates \c *p_buflen with the size in bytes (including
* 1 for the terminating NUL) needed to store business ID.
* If both \c p_bid and \c p_buflen are not NULL, then \c *p_buflen should contain the
* size in bytes pointed to by \c p_bid. The business ID
* (including the terminating NUL) is copied to \c p_bid
* and \c *p_buflen is updated with the size of the business ID
* (including 1 for terminating NUL).
*
* @param[in,out] p_bid pointer to memory that can store the business ID
* @param[in,out] p_buflen a non-NULL pointer to an integer
*
* @return \li CMID_RES_SUCCESS if the call is successful.
* \li CMID_RES_INVALID_ARG if \c p_buflen is NULL.
* \li CMID_RES_NOT_INITED if business id is not available.
* \li CMID_RES_INSUFFICIENT_LEN if \c p_bid is NULL, OR
* if \c p_bid is not NULL and \c *p_buflen
* does not have a value
* >= (size of business ID in bytes
* +1 for terminating NUL);
* \c *p_buflen is updated with the required size
* (including 1 for the terminating NUL).
* \li CMID_RES_GENERAL_ERROR if any other error occurs.
*/
CMID_CAPI
cmid_result_t
cmid_get_business_id(IN OUT char* p_bid, IN OUT int* p_buflen);

/**
* Get the event, catalog or checkin url based on the urlType param.
*
* Copies the requested URL to the memory pointed to by \c p_url. The terminating NUL
* character is also copied.
*
* If \c p_buflen is NULL, then returns \c CMID_RES_INVALID_ARG.
* If \c p_url is NULL, then updates \c *p_buflen with the size in bytes (including
* 1 for the terminating NUL) needed to store the url.
* If both \c p_url and \c p_buflen are not NULL, then \c *p_buflen should contain the
* size in bytes pointed to by \c p_url. The requested url
* (including the terminating NUL) is copied to \c p_url
* and \c *p_buflen is updated with the size of the url
* (including 1 for terminating NUL).
* If \c urlType is not supported, then returns CMID_RES_INVALID_ARG
*
* @param[in] urlType enum depicting the requested url type
* @param[in,out] p_url pointer to memory that can store the requested url
* @param[in,out] p_buflen a non-NULL pointer to an integer
*
* @return \li CMID_RES_SUCCESS if the call is successful.
* \li CMID_RES_INVALID_ARG if \c p_buflen is NULL, OR
* if \c urlType is invalid or not supported
* \li CMID_RES_NOT_INITED if the url is not available.
* \li CMID_RES_INSUFFICIENT_LEN if \c p_url is NULL, OR
* if \c p_url is not NULL and \c *p_buflen
* does not have a value
* >= (size of requested url in bytes
* +1 for terminating NUL);
* \c *p_buflen is updated with the required size
* (including 1 for the terminating NUL).
* \li CMID_RES_GENERAL_ERROR if any other error occurs.
*/
CMID_CAPI
cmid_result_t
cmid_get_url(IN cmid_url_type_t urlType, IN OUT char* p_url, IN OUT int* p_buflen);

#ifdef __cplusplus
}
#endif
