#pragma once
#include "json\json.h"
#include <filesystem>

static void ThrowAway_GenerateUcidConfig()
{
    std::wstring bsConfigFile;
    std::wstring ucidConfigFile;

    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureXYZ\\UnifiedConnector\\config", L"Bootstrapper", bsConfigFile ) )
    {
        throw std::exception( "Failed to read bootstrapper config path from registry" );
    }

    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureXYZ\\UnifiedConnector\\config", L"UCID", ucidConfigFile ) )
    {
        throw std::exception( "Failed to read ucid config path from registry" );
    }

    std::ifstream file( bsConfigFile );
    auto bsData = std::string( (std::istreambuf_iterator<char>( file )), std::istreambuf_iterator<char>() );

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root;
    std::string jsonError;
    jsonReader->parse( bsData.c_str(), bsData.c_str() + bsData.length(), &root, &jsonError );

    std::ofstream ofs( ucidConfigFile );
    ofs << "<identity_config><business_id>" <<
        root["id"]["business_id"].asString() <<
        "</business_id><installer_key>" <<
        root["id"]["installer_key"].asString() <<
        "</installer_key><url>" <<
        root["id"]["url"].asString() <<
        "</url></identity_config>";
    ofs.close();
}