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

int32_t InstallerConfig::LoadPmConfig( const std::string& pmConfig )
{
    return int32_t();
}

bool InstallerConfig::PmConfigFileChanged( const std::string& pmConfig )
{
    return false;
}

int32_t InstallerConfig::VerifyPmFileIntegrity( const std::string& pmConfig )
{
    return int32_t();
}

std::string InstallerConfig::GetCloudCheckinUri()
{
    return "";
}

std::string InstallerConfig::GetCloudEventUri()
{
    return m_configData.eventUri;
}

std::string InstallerConfig::GetCloudCatalogUri()
{
    return "";
}

uint32_t InstallerConfig::GetCloudCheckinIntervalMs()
{
    return uint32_t();
}

uint32_t InstallerConfig::GetLogLevel()
{
    return uint32_t();
}

uint32_t InstallerConfig::GetMaxFileCacheAgeS()
{
    return uint32_t();
}

uint32_t InstallerConfig::GetMaxEventTtlS()
{
    return uint32_t();
}

bool InstallerConfig::AllowPostInstallReboots()
{
    return false;
}

uint32_t InstallerConfig::GetRebootThrottleS()
{
    return uint32_t();
}

uint32_t InstallerConfig::GetWatchdogTimeoutMs()
{
    return uint32_t();
}

void InstallerConfig::SetCloudEventUri( std::string& uri )
{
    m_configData.eventUri = uri;
}

