#include "pch.h"
#include "WindowsUtilities.h"
#include <locale>
#include <codecvt>
#include <fstream>
#include <ShlObj.h>
#include <Msi.h>
#include <unordered_map>

#define UC_REG_KEY L"SOFTWARE\\Cisco\\SecureClient\\UnifiedConnector"

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> _g_converter;
// TOOD: Do we acutally need all of thses or just a subset?
static std::unordered_map< std::string, const GUID> _knownFolderMap = {
    { "FOLDERID_AccountPictures", FOLDERID_AccountPictures },
    { "FOLDERID_AddNewPrograms", FOLDERID_AddNewPrograms },
    { "FOLDERID_AdminTools", FOLDERID_AdminTools },
    { "FOLDERID_AppDataDesktop", FOLDERID_AppDataDesktop },
    { "FOLDERID_AppDataDocuments", FOLDERID_AppDataDocuments },
    { "FOLDERID_AppDataFavorites", FOLDERID_AppDataFavorites },
    { "FOLDERID_AppDataProgramData", FOLDERID_AppDataProgramData },
    { "FOLDERID_ApplicationShortcuts", FOLDERID_ApplicationShortcuts },
    { "FOLDERID_AppsFolder", FOLDERID_AppsFolder },
    { "FOLDERID_AppUpdates", FOLDERID_AppUpdates },
    { "FOLDERID_CameraRoll", FOLDERID_CameraRoll },
    { "FOLDERID_CDBurning", FOLDERID_CDBurning },
    { "FOLDERID_ChangeRemovePrograms", FOLDERID_ChangeRemovePrograms },
    { "FOLDERID_CommonAdminTools", FOLDERID_CommonAdminTools },
    { "FOLDERID_CommonOEMLinks", FOLDERID_CommonOEMLinks },
    { "FOLDERID_CommonPrograms", FOLDERID_CommonPrograms },
    { "FOLDERID_CommonStartMenu", FOLDERID_CommonStartMenu },
    { "FOLDERID_CommonStartup", FOLDERID_CommonStartup },
    { "FOLDERID_CommonTemplates", FOLDERID_CommonTemplates },
    { "FOLDERID_ComputerFolder", FOLDERID_ComputerFolder },
    { "FOLDERID_ConflictFolder", FOLDERID_ConflictFolder },
    { "FOLDERID_ConnectionsFolder", FOLDERID_ConnectionsFolder },
    { "FOLDERID_Contacts", FOLDERID_Contacts },
    { "FOLDERID_ControlPanelFolder", FOLDERID_ControlPanelFolder },
    { "FOLDERID_Cookies", FOLDERID_Cookies },
    { "FOLDERID_Desktop", FOLDERID_Desktop },
    { "FOLDERID_DeviceMetadataStore", FOLDERID_DeviceMetadataStore },
    { "FOLDERID_Documents", FOLDERID_Documents },
    { "FOLDERID_DocumentsLibrary", FOLDERID_DocumentsLibrary },
    { "FOLDERID_Downloads", FOLDERID_Downloads },
    { "FOLDERID_Favorites", FOLDERID_Favorites },
    { "FOLDERID_Fonts", FOLDERID_Fonts },
    { "FOLDERID_Games", FOLDERID_Games },
    { "FOLDERID_GameTasks", FOLDERID_GameTasks },
    { "FOLDERID_History", FOLDERID_History },
    { "FOLDERID_HomeGroup", FOLDERID_HomeGroup },
    { "FOLDERID_HomeGroupCurrentUser", FOLDERID_HomeGroupCurrentUser },
    { "FOLDERID_ImplicitAppShortcuts", FOLDERID_ImplicitAppShortcuts },
    { "FOLDERID_InternetCache", FOLDERID_InternetCache },
    { "FOLDERID_InternetFolder", FOLDERID_InternetFolder },
    { "FOLDERID_Libraries", FOLDERID_Libraries },
    { "FOLDERID_Links", FOLDERID_Links },
    { "FOLDERID_LocalAppData", FOLDERID_LocalAppData },
    { "FOLDERID_LocalAppDataLow", FOLDERID_LocalAppDataLow },
    { "FOLDERID_LocalizedResourcesDir", FOLDERID_LocalizedResourcesDir },
    { "FOLDERID_Music", FOLDERID_Music },
    { "FOLDERID_MusicLibrary", FOLDERID_MusicLibrary },
    { "FOLDERID_NetHood", FOLDERID_NetHood },
    { "FOLDERID_NetworkFolder", FOLDERID_NetworkFolder },
    { "FOLDERID_Objects3D", FOLDERID_Objects3D },
    { "FOLDERID_OriginalImages", FOLDERID_OriginalImages },
    { "FOLDERID_PhotoAlbums", FOLDERID_PhotoAlbums },
    { "FOLDERID_PicturesLibrary", FOLDERID_PicturesLibrary },
    { "FOLDERID_Pictures", FOLDERID_Pictures },
    { "FOLDERID_Playlists", FOLDERID_Playlists },
    { "FOLDERID_PrintersFolder", FOLDERID_PrintersFolder },
    { "FOLDERID_Profile", FOLDERID_Profile },
    { "FOLDERID_ProgramData", FOLDERID_ProgramData },
    { "FOLDERID_ProgramFiles", FOLDERID_ProgramFiles },
    { "FOLDERID_ProgramFilesX64", FOLDERID_ProgramFilesX64 },
    { "FOLDERID_ProgramFilesX86", FOLDERID_ProgramFilesX86 },
    { "FOLDERID_ProgramFilesCommon", FOLDERID_ProgramFilesCommon },
    { "FOLDERID_ProgramFilesCommonX64", FOLDERID_ProgramFilesCommonX64 },
    { "FOLDERID_ProgramFilesCommonX86", FOLDERID_ProgramFilesCommonX86 },
    { "FOLDERID_Programs", FOLDERID_Programs },
    { "FOLDERID_Public", FOLDERID_Public },
    { "FOLDERID_PublicDocuments", FOLDERID_PublicDocuments },
    { "FOLDERID_PublicDownloads", FOLDERID_PublicDownloads },
    { "FOLDERID_PublicGameTasks", FOLDERID_PublicGameTasks },
    { "FOLDERID_PublicLibraries", FOLDERID_PublicLibraries },
    { "FOLDERID_PublicMusic", FOLDERID_PublicMusic },
    { "FOLDERID_PublicPictures", FOLDERID_PublicPictures },
    { "FOLDERID_PublicRingtones", FOLDERID_PublicRingtones },
    { "FOLDERID_PublicUserTiles", FOLDERID_PublicUserTiles },
    { "FOLDERID_PublicVideos", FOLDERID_PublicVideos },
    { "FOLDERID_QuickLaunch", FOLDERID_QuickLaunch },
    { "FOLDERID_Recent", FOLDERID_Recent },
    { "FOLDERID_RecordedTVLibrary", FOLDERID_RecordedTVLibrary },
    { "FOLDERID_RecycleBinFolder", FOLDERID_RecycleBinFolder },
    { "FOLDERID_ResourceDir", FOLDERID_ResourceDir },
    { "FOLDERID_Ringtones", FOLDERID_Ringtones },
    { "FOLDERID_RoamingAppData", FOLDERID_RoamingAppData },
    { "FOLDERID_RoamedTileImages", FOLDERID_RoamedTileImages },
    { "FOLDERID_RoamingTiles", FOLDERID_RoamingTiles },
    { "FOLDERID_SampleMusic", FOLDERID_SampleMusic },
    { "FOLDERID_SamplePictures", FOLDERID_SamplePictures },
    { "FOLDERID_SamplePlaylists", FOLDERID_SamplePlaylists },
    { "FOLDERID_SampleVideos", FOLDERID_SampleVideos },
    { "FOLDERID_SavedGames", FOLDERID_SavedGames },
    { "FOLDERID_SavedPictures", FOLDERID_SavedPictures },
    { "FOLDERID_SavedPicturesLibrary", FOLDERID_SavedPicturesLibrary },
    { "FOLDERID_SavedSearches", FOLDERID_SavedSearches },
    { "FOLDERID_Screenshots", FOLDERID_Screenshots },
    { "FOLDERID_SEARCH_CSC", FOLDERID_SEARCH_CSC },
    { "FOLDERID_SearchHistory", FOLDERID_SearchHistory },
    { "FOLDERID_SearchHome", FOLDERID_SearchHome },
    { "FOLDERID_SEARCH_MAPI", FOLDERID_SEARCH_MAPI },
    { "FOLDERID_SearchTemplates", FOLDERID_SearchTemplates },
    { "FOLDERID_SendTo", FOLDERID_SendTo },
    { "FOLDERID_SidebarDefaultParts", FOLDERID_SidebarDefaultParts },
    { "FOLDERID_SidebarParts", FOLDERID_SidebarParts },
    { "FOLDERID_SkyDrive", FOLDERID_SkyDrive },
    { "FOLDERID_SkyDriveCameraRoll", FOLDERID_SkyDriveCameraRoll },
    { "FOLDERID_SkyDriveDocuments", FOLDERID_SkyDriveDocuments },
    { "FOLDERID_SkyDrivePictures", FOLDERID_SkyDrivePictures },
    { "FOLDERID_StartMenu", FOLDERID_StartMenu },
    { "FOLDERID_Startup", FOLDERID_Startup },
    { "FOLDERID_SyncManagerFolder", FOLDERID_SyncManagerFolder },
    { "FOLDERID_SyncResultsFolder", FOLDERID_SyncResultsFolder },
    { "FOLDERID_SyncSetupFolder", FOLDERID_SyncSetupFolder },
    { "FOLDERID_System", FOLDERID_System },
    { "FOLDERID_SystemX86", FOLDERID_SystemX86 },
    { "FOLDERID_Templates", FOLDERID_Templates },
    { "FOLDERID_UserPinned", FOLDERID_UserPinned },
    { "FOLDERID_UserProfiles", FOLDERID_UserProfiles },
    { "FOLDERID_UserProgramFiles", FOLDERID_UserProgramFiles },
    { "FOLDERID_UserProgramFilesCommon", FOLDERID_UserProgramFilesCommon },
    { "FOLDERID_UsersFiles", FOLDERID_UsersFiles },
    { "FOLDERID_UsersLibraries", FOLDERID_UsersLibraries },
    { "FOLDERID_Videos", FOLDERID_Videos },
    { "FOLDERID_VideosLibrary", FOLDERID_VideosLibrary },
    { "FOLDERID_Windows", FOLDERID_Windows }
};

bool WindowsUtilities::FileExists(const WCHAR* filename)
{
    struct _stat stFileInfo;
    return (_wstat(filename, &stFileInfo) == 0);
}

std::string WindowsUtilities::ReadFileContents( const WCHAR* filename )
{
    std::string contents;

    if( FileExists( filename ) ) {
        std::ifstream file( filename );

        if( file.is_open() ) {
            file.seekg( 0, std::ios::end );
            contents.resize( file.tellg() );
            file.seekg( 0, std::ios::beg );

            file.read( &contents[ 0 ], contents.size() );
        }

    }

    return contents;
}

uint32_t WindowsUtilities::GetFileModifyTime( const WCHAR* filename )
{
    struct _stat stFileInfo = { 0 };
    _wstat( filename, &stFileInfo );

    return ( uint32_t )stFileInfo.st_mtime;
}

bool WindowsUtilities::DirectoryExists(const WCHAR* dirname)
{
    DWORD ftyp = GetFileAttributes(dirname);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }

    return false;
}

std::wstring WindowsUtilities::GetExePath()
{
    WCHAR buffer[MAX_PATH] = { 0 };

    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"/\\");

    return std::wstring(buffer).substr(0, pos);
}

std::wstring WindowsUtilities::GetDirPath(const std::wstring& path)
{
    std::wstring::size_type pos = std::wstring(path).find_last_of(L"/\\");

    return path.substr(0, pos);
}

bool WindowsUtilities::ReadRegistryString(_In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data)
{
    DWORD dataSize{};
    LONG retCode = ::RegGetValue(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &dataSize);

    if (retCode != ERROR_SUCCESS)
    {
        return false;
    }

    data.resize(dataSize / sizeof(wchar_t));

    retCode = ::RegGetValue(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, &data[0], &dataSize);
    if (retCode != ERROR_SUCCESS)
    {
        return false;
    }

    DWORD stringLengthInWchars = dataSize / sizeof(wchar_t);
    stringLengthInWchars--; // Exclude the NUL written by the Win32 API
    data.resize(stringLengthInWchars);

    return true;
}

bool WindowsUtilities::ReadRegistryStringA( _In_ HKEY hKey, _In_ const std::string& subKey, _In_ const std::string& valueName, _In_ DWORD flags, _Out_ std::string& data )
{
    DWORD dataSize {};
    LONG retCode = ::RegGetValueA( hKey, subKey.c_str(), valueName.c_str(), flags, nullptr, nullptr, &dataSize );

    if( retCode != ERROR_SUCCESS )
    {
        return false;
    }

    data.resize( dataSize / sizeof( char ) );

    retCode = ::RegGetValueA( hKey, subKey.c_str(), valueName.c_str(), flags, nullptr, &data[ 0 ], &dataSize );
    if( retCode != ERROR_SUCCESS )
    {
        return false;
    }

    DWORD stringLengthInChars = dataSize / sizeof( char );
    stringLengthInChars--; // Exclude the NUL written by the Win32 API
    data.resize( stringLengthInChars );

    return true;
}

bool WindowsUtilities::Is64BitWindows()
{
#if defined(_WIN64)
    return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows so we must sniff
    BOOL f64 = FALSE;
    return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
    return FALSE; // Win64 does not support Win16
#endif
}

bool WindowsUtilities::GetSysDirectory( std::string& path )
{
    bool ret = false;
    PWSTR tmpPath = nullptr;

    HRESULT result = ::SHGetKnownFolderPath( FOLDERID_System, KF_FLAG_DEFAULT, NULL, &tmpPath );

    if ( SUCCEEDED( result ) )
    {
        if ( tmpPath != nullptr )
        {
            path = _g_converter.to_bytes( tmpPath );
        }
        
        CoTaskMemFree( tmpPath );
        ret = true;
    }
    
    return ret;
}

#define GUID_SIZE 39
std::vector<WindowsUtilities::WindowsInstallProgram> WindowsUtilities::GetInstalledPrograms()
{
    DWORD dwIndex = 0;
    DWORD dwStatus = ERROR_SUCCESS;
    static const DWORD max_size = 1024;
    DWORD cchdata = max_size;
    wchar_t data[ max_size ] = { 0 };
    wchar_t szProductCode[ GUID_SIZE ] = { 0 };
    std::vector<WindowsInstallProgram> list;

    do {
        WindowsInstallProgram item;
        memset( szProductCode, 0, sizeof( szProductCode ) );
        dwStatus = MsiEnumProducts( dwIndex, szProductCode );
        if( dwStatus != ERROR_SUCCESS ) {
            break;
        }

        cchdata = max_size;
        if( MsiGetProductInfo( szProductCode, INSTALLPROPERTY_PRODUCTNAME, data, &cchdata ) == 0 ) {
            item.name = _g_converter.to_bytes( data );
            cchdata = max_size;
            if( MsiGetProductInfoW( szProductCode, INSTALLPROPERTY_VERSIONSTRING, data, &cchdata ) == 0 ) {
                item.version = _g_converter.to_bytes( data );
                list.push_back( item );
            }
        }
        dwIndex++;
    } while( dwStatus == ERROR_SUCCESS );

    return list;
}

std::string WindowsUtilities::ResolveKnownFolderId( const std::string& knownFolderId )
{
    std::string knownFolder;

    if( _knownFolderMap.find( knownFolderId ) != _knownFolderMap.end() ) {
        PWSTR wpath = NULL;
        if( SUCCEEDED( SHGetKnownFolderPath( _knownFolderMap[ knownFolderId ], KF_FLAG_DEFAULT, ( HANDLE )-1, &wpath ) ) ) {
            knownFolder = _g_converter.to_bytes( wpath );
            CoTaskMemFree( wpath );
        }
    }

    return knownFolder;
}

std::wstring WindowsUtilities::GetLogDir()
{
    PWSTR path = NULL;
    std::wstring logDir;

    ReadRegistryString( HKEY_LOCAL_MACHINE, UC_REG_KEY, L"LogDir", logDir );

    return logDir;
}



struct ENUM_CONTEXT : WIN32_FIND_DATA
{
    PCWSTR _szMask;
    PCWSTR* _pszMask;
    ULONG _MaskCount;
    ULONG _MaxLevel;
    ULONG _nFiles;
    ULONG _nFolders;
    WCHAR _FileName[MAXSHORT + 1];

    void StartEnum( PCWSTR pcszRoot, PCWSTR pszMask[], ULONG MaskCount, PCWSTR szMask, ULONG MaxLevel, PSTR prefix )
    {
        SIZE_T len = wcslen( pcszRoot );

        if ( len < RTL_NUMBER_OF( _FileName ) )
        {
            memcpy( _FileName, pcszRoot, len * sizeof( WCHAR ) );

            _szMask = szMask, _pszMask = pszMask, _MaskCount = MaskCount;
            _MaxLevel = szMask ? MaxLevel : MaskCount;
            _nFolders = 0, _nFolders = 0;

            Enum( _FileName + len, 0, prefix );
        }
    }

    void Enum( PWSTR pszEnd, ULONG nLevel, PSTR prefix );
};

//void ENUM_CONTEXT::Enum( PWSTR pszEnd, ULONG nLevel, PSTR prefix )
//{
//    if ( nLevel > _MaxLevel )
//    {
//        return;
//    }
//
//    PCWSTR lpFileName = _FileName;
//
//    SIZE_T cb = lpFileName + RTL_NUMBER_OF( _FileName ) - pszEnd;
//
//    PCWSTR szMask = nLevel < _MaskCount ? _pszMask[nLevel] : _szMask;
//
//    SIZE_T cchMask = wcslen( szMask ) + 1;
//
//    if ( cb < cchMask + 1 )
//    {
//        return;
//    }
//
//    *pszEnd++ = L'\\', cb--;
//
//    DbgPrint( "%s[<%.*S>]\n", prefix, pszEnd - lpFileName, lpFileName );
//
//    memcpy( pszEnd, szMask, cchMask * sizeof( WCHAR ) );
//
//    ULONG dwError;
//
//    HANDLE hFindFile = FindFirstFileEx( lpFileName, FindExInfoBasic, this, FindExSearchNameMatch, 0, FIND_FIRST_EX_LARGE_FETCH );
//
//    if ( hFindFile != INVALID_HANDLE_VALUE )
//    {
//        PWSTR FileName = cFileName;
//
//        do
//        {
//            SIZE_T FileNameLength = wcslen( FileName );
//
//            switch ( FileNameLength )
//            {
//            case 2:
//                if ( FileName[1] != '.' ) break;
//            case 1:
//                if ( FileName[0] == '.' ) continue;
//            }
//
//            if ( dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
//            {
//                _nFolders++;
//
//                if ( !(dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) )
//                {
//                    if ( cb < FileNameLength )
//                    {
//                        __debugbreak();
//                    }
//                    else
//                    {
//                        memcpy( pszEnd, FileName, FileNameLength * sizeof( WCHAR ) );
//                        Enum( pszEnd + FileNameLength, nLevel + 1, prefix - 1 );
//                    }
//                }
//            }
//            else if ( nLevel >= _MaskCount || (!_szMask && nLevel == _MaskCount - 1) )
//            {
//                _nFiles++;
//
//                DbgPrint( "%s%u%u <%.*S>\n", prefix, nFileSizeLow, nFileSizeHigh, FileNameLength, FileName );
//            }
//
//        } while ( FindNextFile( hFindFile, this ) );
//
//        if ( (dwError = GetLastError()) == ERROR_NO_MORE_FILES )
//        {
//            dwError = NOERROR;
//        }
//
//        FindClose( hFindFile );
//    }
//    else
//    {
//        dwError = GetLastError();
//    }
//
//    if ( dwError && dwError != ERROR_FILE_NOT_FOUND )
//    {
//        DbgPrint( "%s[<%.*S>] err = %u\n", prefix, pszEnd - lpFileName, lpFileName, dwError );
//    }
//}
//
//void Test( PCWSTR pcszRoot )
//{
//    char prefix[MAXUCHAR + 1];
//    memset( prefix, '\t', RTL_NUMBER_OF( prefix ) - 1 );
//    prefix[RTL_NUMBER_OF( prefix ) - 1] = 0;
//
//    ENUM_CONTEXT ectx;
//
//    static PCWSTR Masks[] = { L"Program*", L"*", L"*.txt" };
//    static PCWSTR Masks2[] = { L"Program*", L"*" };
//    static PCWSTR Masks3[] = { L"Program Files*", L"Internet Explorer" };
//
//    // search Program*\*\*.txt with fixed deep level
//    ectx.StartEnum( pcszRoot, Masks, RTL_NUMBER_OF( Masks ), 0, RTL_NUMBER_OF( prefix ) - 1, prefix + RTL_NUMBER_OF( prefix ) - 1 );
//    // search *.txt files from Program*\*\ - any deep level
//    ectx.StartEnum( pcszRoot, Masks2, RTL_NUMBER_OF( Masks2 ), L"*.txt", RTL_NUMBER_OF( prefix ) - 1, prefix + RTL_NUMBER_OF( prefix ) - 1 );
//    // search all files (*) from Program Files*\Internet Explorer\ 
//    ectx.StartEnum( pcszRoot, Masks3, RTL_NUMBER_OF( Masks3 ), L"*", RTL_NUMBER_OF( prefix ) - 1, prefix + RTL_NUMBER_OF( prefix ) - 1 );
//
//}