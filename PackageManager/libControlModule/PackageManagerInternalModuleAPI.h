/*******************************************************************************
*       Copyright (c) 2020, Cisco Systems, All Rights Reserved
********************************************************************************/
/**
* @file PackageManagerInternalModuleAPI.h
*
* @brief Provides the interface for an internal package managed module (running in the same process as the package manager)
*
* @version 1
*
* An internal module that is to be managed by the package manger will expose two APIs:
* CreateModuleInstance() and ReleaseModuleInstance() which should be used to create and release the module instance respectively.
* A context returned by CreateModuleInstance() provides function pointers to APIs
* Init(), Deinit(), Start(), Stop(), SetOption(), ConfigUpdated().
*
* @section example Example
* Basic use:
*
*     //Create a module instance - REQUIRED
*     MODULE_CTX_T  modContext = {0};
*     modContext.nVersion = PM_MODULE_INTERFACE_VERSION;
*     CreateModuleInstance(&modContext);
*
*     //Initialize module - OPTIONAL
*     if ( modContext.fpInit )
*         modContext.fpInit();
*
*     //Set options - OPTIONAL
*     If ( modContext.fpSetOption )
*         modContext.fpSetOption();
*
*     //Start the module - REQUIRED
*     modContext.fpStart("<absolute path to binary directory>", "<absolute path to data directory>", "<absolute path to config directory>");
*
*     //Notify module of config update - OPTIONAL
*     If ( modContext.fpConfigUpdated )
*         modContext.fpConfigUpdated();
*
*     //Deinit the module - OPTIONAL
*     If ( modContext.fpDeinit )
*         modContext.fpDeinit();
*
*     //Stop the module - REQUIRED
*     modContext.fpStop();
*
*     //Destroy module instance - REQUIRED
*     ReleaseModuleInstance(modContext);
*
*/

#pragma once

#ifdef _WIN32
    #include <tchar.h>
#else
    #ifndef TCHAR
        #define TCHAR char
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Defines current interface version
*/
#define PM_MODULE_INTERFACE_VERSION 1u

#ifndef IN
    #define IN
#endif

#ifndef OUT
    #define OUT
#endif

#ifdef _WIN32
    #ifdef PM_MODULE_API_EXPORTS
        #define PM_MODULE_API __declspec(dllexport)
    #else
        #define PM_MODULE_API __declspec(dllimport)
    #endif
#else
    #ifdef PM_MODULE_API_EXPORTS
        #define PM_MODULE_API __attribute__((visibility("default")))
    #else
        #define PM_MODULE_API
    #endif
#endif

/**
* @brief Enumeration of errors returned by a package managed module
*/
typedef enum _PM_MODULE_RESULT_T
{
    PM_MODULE_SUCCESS = 0,
    PM_MODULE_GENERAL_ERROR = -1,
    PM_MODULE_NOT_IMPLEMENTED = -2,
    PM_MODULE_NOT_INITIALIZED = -3,
    PM_MODULE_ALREADY_INITIALIZED = -4,
    PM_MODULE_NOT_STARTED = -5,
    PM_MODULE_ALREADY_STARTED = -6,
    PM_MODULE_NO_MEMORY = -7,
    PM_MODULE_INVALID_PARAM = -8,
    PM_MODULE_INVALID_OPTION_ID = -9,
    PM_MODULE_UNSUPPORTED_VERSION = 10,
    PM_MODULE_ACCESS_DENIED = -11,
    PM_MODULE_UNSUPPORTED_PLATFORM_VERSION = -12,
}PM_MODULE_RESULT_T;


/**
* @brief Enumeration of options supported by a module
*/
typedef enum _PM_MODULE_OPTION_ID_T
{
    PM_MODULE_OPTION_UNKNOWN = 0,
    PM_MODULE_OPTION_LOG_LEVEL,
    PM_MODULE_OPTION_MAX
}PM_MODULE_OPTION_ID_T;

/**
* @brief Enumeration of accepted log levels by a module
*/
typedef enum _PM_MODULE_LOG_LEVEL_T
{
    PM_MODULE_LOG_LEVEL_ERROR = 0,
    PM_MODULE_LOG_LEVEL_DEBUG = 1
}PM_MODULE_LOG_LEVEL_T;

/**
* @brief Initializes the module - OPTIONAL
*/
typedef PM_MODULE_RESULT_T(*FP_PM_MODULE_INIT_T)();

/**
* @brief Uninitializes the module - OPTIONAL
*/
typedef PM_MODULE_RESULT_T(*FP_PM_MODULE_DEINIT_T)();

/**
* @brief Starts the module - REQUIRED
*/
typedef PM_MODULE_RESULT_T(*FP_PM_MODULE_START_T)(IN const TCHAR* pszBasePath, IN const TCHAR* pszDataPath, IN const TCHAR* pszConfigPath);

/**
* @brief Stops module - REQUIRED
*/
typedef PM_MODULE_RESULT_T(*FP_PM_MODULE_STOP_T)();

/**
* @brief Sets the configurable options of the module - OPTIONAL
*
* @param[in] nOptionID enumeration that specifies the option to set
* @param[in] pOption   pointer to an object or to a native type, inferred based on nOptionID
* @param[in] nSize     size of pOption
*
* @return PM_MODULE_SUCCESS in case of success, <br>
*         PM_MODULE_INVALID_OPTION_ID if nOptionID is not supported, <br>
*         other PM_MODULE_RESULT_T in case of errors. <br>
*/
typedef PM_MODULE_RESULT_T(*FP_PM_MODULE_SET_OPTION_T)(IN PM_MODULE_OPTION_ID_T nOptionID, IN void* pOption, IN size_t nSize);

/**
* @brief notifies the module of a config update - OPTIONAL
*/
typedef PM_MODULE_RESULT_T(*FP_PM_MODULE_CONFIG_UPDATED_T)();

/**
* @brief Structure to define module context
*/
typedef struct _PM_MODULE_CTX_T
{
    unsigned int nVersion;
    FP_PM_MODULE_INIT_T fpInit;
    FP_PM_MODULE_DEINIT_T fpDeinit;
    FP_PM_MODULE_START_T fpStart;
    FP_PM_MODULE_STOP_T fpStop;
    FP_PM_MODULE_SET_OPTION_T fpSetOption;
    FP_PM_MODULE_CONFIG_UPDATED_T fpConfigUpdated;
}PM_MODULE_CTX_T;

/**
* @brief Creates a module instance - REQUIRED
*
* @param[in,out] pPM_MODULE_CTX  pointer to module context object. Caller should set version field of the context
*                            to an expected interface version. If that version is supported by the module then the module will populate
*                            the remaining fields of the context based on that expected version.
*
* @return PM_MODULE_SUCCESS in case of success, <br>
*         PM_MODULE_UNSUPPORTED_VERSION if requested interface version is not supported by the module, <br>
*         PM_MODULE_UNSUPPORTED_PLATFORM_VERSION if current platform is not supported by the module, <br>
*         other PM_MODULE_RESULT_T in case of errors. <br>
*/
PM_MODULE_RESULT_T PM_MODULE_API CreateModuleInstance(IN OUT PM_MODULE_CTX_T* pPM_MODULE_CTX);

/**
* @brief Releases a module instance - REQUIRED
*
* This API should be called for each call of CreateModuleInstance
*
* @param[in,out] pPM_MODULE_CTX  pointer to module context object
*
* @return PM_MODULE_SUCCESS in case of success, <br>
*         other PM_MODULE_RESULT_T in case of errors. <br>
*/
PM_MODULE_RESULT_T PM_MODULE_API ReleaseModuleInstance(IN OUT PM_MODULE_CTX_T* pPM_MODULE_CTX);

#ifdef __cplusplus
}
#endif
