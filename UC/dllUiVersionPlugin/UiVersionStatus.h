#pragma once

#include "ComponentStatusPlugin.h"

class UiVersionStatus : public IComponentStatus
{
public: 
    virtual bool SupportsInterface( const char* pszInterfaceName,
        unsigned int uiVersion = 0 ) override;
    virtual PluginResult IsEnabled(bool& rbIsEnabled) override;
    virtual PluginResult GetDisplayName(char* utf8DisplayString, size_t& nBufferLen) override;
    virtual PluginResult GetDisplayVersion(char* utfDisplayVersionString, size_t& nBufferLen) override;
};

