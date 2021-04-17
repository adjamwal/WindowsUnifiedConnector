#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

struct PmComponent;
struct PackageConfigInfo;
struct PackageInventory;
struct PmDiscoveryComponent;
struct PmProductDiscoveryRules;

/**
 * @file IPmPlatformComponentManager.h
 *
 * @brief This interface will provide the plaform specific functionality to package installs and updates
 */
class IPmPlatformComponentManager
{
public:
    IPmPlatformComponentManager() {}
    virtual ~IPmPlatformComponentManager() {}

    /**
     * @brief This API is used to retrieve the list of all installed packages on the client. The package manager
     *   will provide a list of attributes to lookup for each package
     *
     * @return 0 if the packages have been successfully retrieved. -1 otherwise
     */
    virtual int32_t GetInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules, PackageInventory& packagesDiscovered ) = 0;

    /**
     * @brief This API is used to retrieve the cached list of packages installed on the client, as detected during the last discovery operation.
     *
     * @return 0 if the packages have been successfully retrieved. -1 otherwise
     */
    virtual int32_t GetCachedInventory( PackageInventory& cachedInventory ) = 0;

    /**
     * @brief This API will be used to install a package. The package will provide the following:
     *   - Installation binary
     *   - Installation location
     *   - Installation command line
     *   This API should rollback the installation if the package could not be installed
     *
     * @param[in] package - The package details
     * @return 0 if the package was installed. -1 otherwise
     */
    virtual int32_t InstallComponent( const PmComponent& package ) = 0;

    /**
     * @brief This API will be used to update a package. The package will provide the following:
     *   - Installation binary
     *   - Installation location
     *   - Installation command line
     *   This API should rollback to the previous version if the package could not be updated. The package should be
     *   left in a good working state
     *
     * @param[in] package - The package details
     * @return 0 if the package was updated. -1 otherwise
     */
    virtual int32_t UpdateComponent( const PmComponent& package, std::string& error ) = 0;

    /**
     * @brief This API will be used to remove a package. The package will provide the following:
     *   - Un-installation binary
     *   - Un-installation command line
     *   This API should rollback to the uninstallation if the package could not be removed. The package should be
     *   left in a good working state
     *
     * @param[in] package - The package details
     * @return 0 if the package was removed. -1 otherwise
     */
    virtual int32_t UninstallComponent( const PmComponent& package ) = 0;

    /**
     * @brief This API will be used to deploy a configuration file for a package. The configuration will provide the 
     *   following:
     *   - Configuration file path
     *   - Configuration contents
     *   - commandline to validate the configuration file
     *
     * @return 0 if the configuration was deployed. -1 otherwise
     */
    virtual int32_t DeployConfiguration( const PackageConfigInfo& config ) = 0;

    /**
     * @brief This API will be used to resolve a platform specific path
     *   The config path could contain platform specific content ( Windows KNOWN_FOLDER_ID )
     *
     * @return string contaning the resolved
     */
    virtual std::string ResolvePath( const std::string& basePath ) = 0;
};