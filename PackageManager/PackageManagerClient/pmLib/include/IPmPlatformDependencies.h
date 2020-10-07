#pragma once

class IPmPlatformConfiguration;
class IPmPlatformComponentManager;

/**
 * @file IPmPlaformDependencies
 *
 * @brief This inteface will need to be implemented by the integrator, the package manager will use this interface
 *   to invoke platform specific behavoirs. This interface has been split into confinguration and component management
 *   interfaces. See each interface for details
 *
 *  These interfaces will have to respect ABI boundaries
 */
class IPmPlatformDependencies
{
public:
    IPmPlatformDependencies() {}
    virtual ~IPmPlatformDependencies() {}

    virtual IPmPlatformConfiguration& Configuration() = 0;
    virtual IPmPlatformComponentManager& ComponentManager() = 0;
};