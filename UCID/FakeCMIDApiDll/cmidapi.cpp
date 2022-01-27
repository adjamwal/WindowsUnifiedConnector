#include "pch.h"
#include "cmidapi.h"

cmid_result_t copy_data(IN const char* str, IN OUT char* p_data, IN OUT int* p_buflen)
{
    cmid_result_t result = cmid_result_t::CMID_RES_GENERAL_ERROR;

    if (p_data == NULL && p_buflen != NULL)
    {
        *p_buflen = static_cast<int>(strlen(str));

        result = cmid_result_t::CMID_RES_INSUFFICIENT_LEN;
    }
    else if (p_data != NULL && p_buflen != NULL)
    {
        if (*p_buflen >= static_cast<int>(strlen(str)))
        {
            strncpy_s(p_data, *p_buflen + 1, str, static_cast<int>(strlen(str)));

            result = cmid_result_t::CMID_RES_SUCCESS;
        }
        else
        {
            *p_buflen = static_cast<int>(strlen(str));

            result = cmid_result_t::CMID_RES_INSUFFICIENT_LEN;
        }
    }
    else
    {
        result = cmid_result_t::CMID_RES_NOT_INITED;
    }

    return result;
}

CMID_CAPI cmid_result_t cmid_get_id(IN OUT char* p_id, IN OUT int* p_buflen)
{
    const char* str = "tempid";

    cmid_result_t result = copy_data(str, p_id, p_buflen);

    return result;
}

CMID_CAPI cmid_result_t cmid_get_token(IN OUT char* p_token, IN OUT int* p_buflen)
{
    const char* str = "temptoken";

    cmid_result_t result = copy_data(str, p_token, p_buflen);

    return result;
}

CMID_CAPI cmid_result_t cmid_refresh_token()
{
    return cmid_result_t::CMID_RES_SUCCESS;
}

CMID_CAPI cmid_result_t cmid_get_business_id(IN OUT char* p_bid, IN OUT int* p_buflen)
{
    const char* str = "tempbusinessid";

    cmid_result_t result = copy_data(str, p_bid, p_buflen);

    return result;
}

CMID_CAPI cmid_result_t cmid_get_url(IN cmid_url_type_t urlType, IN OUT char* p_url, IN OUT int* p_buflen)
{
    cmid_result_t result = cmid_result_t::CMID_RES_GENERAL_ERROR;

    const char* catalog_url = "http://localhost:9201/catalog";
    const char* checkin_url = "http://localhost:9201/checkin";
    const char* event_url = "http://localhost:9201/event";

    if (urlType == cmid_url_type_t::CMID_CATALOG_URL)
    {
        result = copy_data(catalog_url, p_url, p_buflen);
    }
    else if (urlType == cmid_url_type_t::CMID_CHECKIN_URL)
    {
        result = copy_data(checkin_url, p_url, p_buflen);
    }
    else if (urlType == cmid_url_type_t::CMID_EVENT_URL)
    {
        result = copy_data(event_url, p_url, p_buflen);
    }
    else
    {
        result = cmid_result_t::CMID_RES_INVALID_ARG;
    }

    return result;
}
