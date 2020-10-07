#pragma once

#include <cstdint>

class IPmPlatformDependencies;

/**
 * @file IPackageManager.h
 *
 * @brief This contains the package manager interface
 */
class IPackageManager
{
public:
    IPackageManager() {}
    virtual ~IPackageManager() {}

    /**
     * @brief Starts the package manager. This starts are thread that will periodically check for updates from the 
     *   cloud
     * @param[in] configFile - Path to the configuration file. This is a json file that will provide the PM cloud
     *   URL and the thread interval. The file should be in the form:
     *   {
     *       "cloud": {
     *           "CheckinUri": "https://packagemanager.cisco.com/checkin",
     *           "CheckinInterval": 1000
     *       }
     *   }
     *
     * @return 0 on success, -1 otherwise
     */
    virtual int32_t Start( const char* configFile ) = 0;

    /**
     * @brief Stops the package manager
     * @return 0 on success, -1 otherwise
     */
    virtual int32_t Stop() = 0;

    /**
     * @brief An integrator will need to supply an interface to perform platform specific functionality. This
     *   API will be used to pass that interface into the package manager. The platform dependencies must be
     *   supplied before starting the package manager
     */
    virtual void SetPlatformDependencies( IPmPlatformDependencies* dependecies ) = 0;
};

/**
 * @brief Initializes the package manager module. This must be called before the package manager instance can
 *   be retrived
 * @return 0 on success, -1 otherwise
 */
int32_t InitPackageManager();

/**
 * @brief Deinitializes the package manager module. The package manager interface should not be used after
 *   it has been deinitialized
 * @return 0 on success, -1 otherwise
 */
int32_t DeinitPackageManager();

/**
 * @brief Retrieves the package manager interface
 * @return the interface pointer. NULL on failure
 */
IPackageManager* GetPackageManagerInstance();
