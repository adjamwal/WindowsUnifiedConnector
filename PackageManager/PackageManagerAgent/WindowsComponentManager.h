#pragma once

#include "IPmPlatformComponentManager.h"
#include "IUcLogger.h"
#include <string>

class WindowsComponentManager : public IPmPlatformComponentManager
{
public:
    WindowsComponentManager();
    virtual ~WindowsComponentManager();

    /**
     * @brief This API is used to retrieve the list of all installed packages on the client. The package manager
     *   will provide a list of attributes to lookup for each package
     *
     * @return 0 if the packages have been successfully retrieved. -1 otherwise
     */
    int32_t GetInstalledPackages( PmInstalledPackage* packages, size_t& packagesLen ) override;

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
    int32_t InstallComponent( const PmPackage& package ) override;

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
    int32_t UpdateComponent( const PmPackage& package ) override;

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
    int32_t UninstallComponent( const PmPackage& package ) override;

    /**
     * @brief This API will be used to deploy a configuration file for a package. The configuration will provide the
     *   following:
     *   - Configuration file path
     *   - Configuration contents
     *   - commandline to validate the configuration file
     *
     * @return 0 if the configuration was deployed. -1 otherwise
     */
    int32_t DeployConfiguration( const PmPackageConfigration& config ) override;

private:
    int32_t RunPackage( std::string executable, std::string cmdline, std::string& error );
};

