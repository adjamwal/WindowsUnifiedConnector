#pragma once

#include "IPmPlatformDependencies.h"

class WindowsConfiguration;
class WindowsComponentManager;

class WindowsPmDependencies : public IPmPlatformDependencies
{
public:
    WindowsPmDependencies( WindowsConfiguration& configuration, WindowsComponentManager& componentManager );
    ~WindowsPmDependencies();

    IPmPlatformConfiguration& Configuration() override;
    IPmPlatformComponentManager& ComponentManager() override;

private:
    WindowsConfiguration& m_configuration;
    WindowsComponentManager& m_componentMgr;
};

