#pragma once

#include <string>
#include <vector>
#include <map>

#define IDS_UPDATE_REQUIRES_REBOOT 0
#define IDS_ELEVATION_REQUIRED 1
#define DEFAULT_LANG_ID 1033

struct TranlationSet 
{
    int languageCode;
    std::vector<std::wstring> strings;
};

static TranlationSet _englishTranslationSet = {
    1033,
    {
        L"A Cisco software update requires a reboot to complete.",
        L"Administrative privileges are required to run correctly. Please run again from an Administrator account or with UAC enabled" 
    }
};

static TranlationSet _frenchTranslationSet = {
    1036,
    {
        L"A Cisco software update requires a reboot to complete.",
        L"Administrative privileges are required to run correctly. Please run again from an Administrator account or with UAC enabled"
    }
};

static std::map <int, std::vector<std::wstring>> _translationMap = {
    { _englishTranslationSet.languageCode, _englishTranslationSet.strings },
    { _frenchTranslationSet.languageCode, _frenchTranslationSet.strings }
};
