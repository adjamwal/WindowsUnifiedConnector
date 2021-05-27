#pragma once

#ifdef CA_SUPPORT_API_EXPORTS
#define CA_SUPPORT_API __declspec(dllexport)
#else
#define CA_SUPPORT_API __declspec(dllimport)
#endif