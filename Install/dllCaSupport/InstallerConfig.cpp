#include "pch.h"
#include "InstallerConfig.h"
#include <PmConfig.h>

InstallerConfig::InstallerConfig()
{
    m_configData.log_level = PM_CONFIG_LOGLEVEL_DEFAULT;
}

InstallerConfig::~InstallerConfig()
{
}

int32_t InstallerConfig::LoadBsConfig( const std::string& bsConfig )
{
    return int32_t();
}

int32_t InstallerConfig::LoadPmConfig( const std::string& pmConfig )
{
    return int32_t();
}

bool InstallerConfig::PmConfigFileChanged( const std::string& pmConfig )
{
    return false;
}

int32_t InstallerConfig::VerifyBsFileIntegrity( const std::string& bsConfig )
{
    return int32_t();
}

int32_t InstallerConfig::VerifyPmFileIntegrity( const std::string& pmConfig )
{
    return int32_t();
}

const std::string& InstallerConfig::GetCloudIdentifyUri()
{
    return std::string();
}

const std::string& InstallerConfig::GetCloudCheckinUri()
{
    return std::string();
}

const std::string& InstallerConfig::GetCloudEventUri()
{
    return m_configData.eventUri;
}

const std::string& InstallerConfig::GetCloudCatalogUri()
{
    return std::string();
}

uint32_t InstallerConfig::GetCloudCheckinIntervalMs()
{
    return uint32_t();
}

uint32_t InstallerConfig::GetLogLevel()
{
    return uint32_t();
}

const std::vector<PmComponent>& InstallerConfig::GetSupportedComponentList()
{
    return m_ComponentList;
}

uint32_t InstallerConfig::GetMaxFileCacheAge()
{
    return uint32_t();
}

bool InstallerConfig::AllowPostInstallReboots()
{
    return false;
}

void InstallerConfig::SetCloudEventUri( std::string& uri )
{
    m_configData.eventUri = uri;
}
