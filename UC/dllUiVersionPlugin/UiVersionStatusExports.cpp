#include <Windows.h>
#include <string>

#include "UiVersionStatus.h"
#include "ComponentStatusPluginCommon/ComponentStatusPluginCbasedMethods.h"


struct AVAILABLE_INTERFACE g_AvailableInterfaces[] =
{
    {
        COMPONENT_STATUS,
        COMPONENT_STATUS_VERSION
    },
    {
        C_COMPONENT_STATUS,
        C_COMPONENT_STATUS_VERSION
    }
};


CSC_PLUGIN_API MODULE_INTERFACE_LIST GetAvailableInterfaces()
{
    MODULE_INTERFACE_LIST supportedInterfaces =
    {
        &g_AvailableInterfaces[ 0 ],
        sizeof( g_AvailableInterfaces ) / sizeof( g_AvailableInterfaces[ 0 ] )
    };

    return supportedInterfaces;
}

CSC_PLUGIN_API Plugin* CreatePlugin( const char* pszInterfaceName )
{
    if( NULL == pszInterfaceName ) {
        return NULL;
    }

    Plugin* pNewPlugin = NULL;
    if( NULL != pszInterfaceName ) {
        std::string interfaceName = pszInterfaceName;
        bool bComponentPlugin = ( interfaceName == COMPONENT_STATUS );
        if( bComponentPlugin ) {
            pNewPlugin = new UiVersionStatus();
        }
    }

    // Return new plugin, or NULL if specified interface cannot be instantiated.
    return pNewPlugin;
}

CSC_PLUGIN_API bool DisposePlugin( Plugin* pPlugin )
{
    delete pPlugin;

    return true;
}

/*
* CSC_PLUGIN_API void* CreateCbasedPlugin
*   A Plugin DLL exported entry point that creates and returns a C based plugin API in the form of
*   a C structure containing function pointers.
*   This particular implementation creates the Presentation Plugin API C structure for the
*   Presentation Plugin Shim.
*
*   IN const char* szInterfaceName - Name of the Plugin Interface API C structure being requested.
*/
CSC_PLUGIN_API void* CreateCbasedPlugin(
    const char* pszInterfaceName )
{
    if( NULL == pszInterfaceName ) {
        return NULL;
    }

    std::string strInterfaceName = pszInterfaceName;
    void* pPluginCstruct = NULL;

    if( C_COMPONENT_STATUS == strInterfaceName ) {
        pPluginCstruct = CreateCbasedComponentStatusPlugin();
    }
    // other interfaces added here as "else if".
    else {
        return NULL;
    }

    return pPluginCstruct;
}


/*
* CSC_PLUGIN_API bool DisposeCbasedPlugin
*   A Plugin DLL exported entry point that disposes of a Plugin API C structure instance returned
*   by CreateCbasedPlugin. Memory allocated by a Plugin must be released by that same Plugin. The
*   code using the Plugin must therefore invoke the Plugin's DisposeCbasedPlugin method to free a
*   Plugin API C structure instance returned by CreateCbasedPlugin.
*   This particular implementation frees the Presentation Plugin API C structure instance for the
*   Presentation Plugin Shim.
*
*   IN void* pPluginCstruct - The Plugin API C structure instance, returned by CreateCbasedPlugin,
*           that is to be freed.
*/
CSC_PLUGIN_API bool DisposeCbasedPlugin(
    void* pPluginCstruct )
{
    bool rtn = DisposeCbasedComponentStatusPlugin( pPluginCstruct );

    return rtn;
}