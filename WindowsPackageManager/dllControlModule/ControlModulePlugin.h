#pragma once

#include "PackageManagerInternalModuleAPI.h"

extern "C" PM_MODULE_API PM_MODULE_RESULT_T CreateModuleInstance( IN OUT PM_MODULE_CTX_T * pPM_MODULE_CTX );

extern "C" PM_MODULE_API PM_MODULE_RESULT_T ReleaseModuleInstance( IN OUT PM_MODULE_CTX_T * pPM_MODULE_CTX );