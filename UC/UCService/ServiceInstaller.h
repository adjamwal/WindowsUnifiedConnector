#pragma once

#include "stdafx.h"
#include <winnt.h>
#include <wtypes.h>

void InstallService(PWSTR pszServiceName,
    PWSTR pszDisplayName,
    DWORD dwStartType,
    PWSTR pszDependencies,
    PWSTR pszAccount,
    PWSTR pszPassword);

void UninstallService(PWSTR pszServiceName);
