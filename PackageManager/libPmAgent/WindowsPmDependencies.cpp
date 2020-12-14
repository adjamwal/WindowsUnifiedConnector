#include "pch.h"

#include "WindowsPmDependencies.h"
#include "WindowsConfiguration.h"
#include "WindowsComponentManager.h"

WindowsPmDependencies::WindowsPmDependencies( WindowsConfiguration& configuration, WindowsComponentManager& componentManager ) :
    m_configuration( configuration )
    , m_componentMgr( componentManager )
{

}

WindowsPmDependencies::~WindowsPmDependencies()
{

}

IPmPlatformConfiguration& WindowsPmDependencies::Configuration()
{
    return m_configuration;
}

IPmPlatformComponentManager& WindowsPmDependencies::ComponentManager()
{
    return m_componentMgr;
}