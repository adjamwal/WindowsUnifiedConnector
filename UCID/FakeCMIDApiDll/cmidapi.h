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
        CMID_RES_GENERAL_ERROR = -1,
        CMID_RES_NOT_INITED = -2,        ///< the API is not ready
        CMID_RES_INVALID_ARG = -3,       ///< an argument passed to the API
        CMID_RES_INSUFFICIENT_LEN = -4,  ///< the length of the memory block
        CMID_RES_AGENT_ERROR = -5,       ///< problem when communicating
        CMID_RES_CLOUD_ERROR = -6,       ///< problem in agent's communication
        CMID_RES_CLOUD_FAILURE = -7,     ///< the cloud returned a failure response
    } cmid_result_t;

    typedef enum cmid_url_type_e {
        CMID_EVENT_URL = 1,         ///< indicates the un-versioned event url
        CMID_CHECKIN_URL = 2,       ///< indicates the check-in url
        CMID_CATALOG_URL = 3,       ///< indicates the catalog url
    } cmid_url_type_t;

    CMID_CAPI
        cmid_result_t
        cmid_get_id(IN OUT char* p_id, IN OUT int* p_buflen);

    CMID_CAPI
        cmid_result_t
        cmid_get_token(IN OUT char* p_token, IN OUT int* p_buflen);

    CMID_CAPI
        cmid_result_t
        cmid_refresh_token();

    CMID_CAPI
        cmid_result_t
        cmid_get_business_id(IN OUT char* p_bid, IN OUT int* p_buflen);

    CMID_CAPI
        cmid_result_t
        cmid_get_url(IN cmid_url_type_t urlType, IN OUT char* p_url, IN OUT int* p_buflen);

#ifdef __cplusplus
}
#endif
