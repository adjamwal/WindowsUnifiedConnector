#include "pch.h"
#include "WindowsUtilities.h"
#include <memory>
#include <ShlObj.h>
#include <iostream>
#include <filesystem>
#include <fstream>

typedef std::pair<std::string, std::string> KNOWNFOLDERPAIR;
class ComponentTestWindowsUtilites : public ::testing::TestWithParam<KNOWNFOLDERPAIR>
{
protected:
    void SetUp()
    {

    }

    void TearDown()
    {
        std::filesystem::remove_all( m_baseTestPath );
    }

    void CreateFiles( std::vector<std::filesystem::path>& files )
    {
        for ( const auto& file : files )
        {
            auto p = file.parent_path();
            std::filesystem::create_directories( p );

            std::ofstream ofs( file.string() );
            ofs << "This is a test file.\n";
            ofs.close();
        }
    }

    void ExecuteTestSearch(
        std::filesystem::path path,
        std::vector<std::filesystem::path>& files,
        std::vector<std::filesystem::path>& expected )
    {
        std::vector<std::filesystem::path> discoveredFiles;

        CreateFiles( files );

        int32_t rtn = WindowsUtilities::FileSearchWithWildCard( path, discoveredFiles );

        EXPECT_EQ( rtn, 0 );
        EXPECT_EQ( expected.size(), discoveredFiles.size() );

        for ( auto& discoveredFile : discoveredFiles )
        {
            EXPECT_TRUE( std::find( expected.begin(), expected.end(), discoveredFile ) != expected.end() );
        }
    }

    std::filesystem::path m_baseTestPath = "C:/ProgramData/Test/";
};

TEST_P( ComponentTestWindowsUtilites, VerifyKnownFolderId )
{
    KNOWNFOLDERPAIR data = GetParam();

    std::string knownFolder = WindowsUtilities::ResolveKnownFolderIdForDefaultUser( data.first );
    EXPECT_STRCASEEQ( knownFolder.c_str(), data.second.c_str() );
}

#ifdef _WIN64
INSTANTIATE_TEST_SUITE_P(
    VerifyKnownFolderId,
    ComponentTestWindowsUtilites,
    Values(
        std::pair<std::string, std::string>( "FOLDERID_AccountPictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AddNewPrograms", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AdminTools", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataDesktop", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataDocuments", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataFavorites", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataProgramData", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppUpdates", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ApplicationShortcuts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppsFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CDBurning", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CameraRoll", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ChangeRemovePrograms", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonAdminTools", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Administrative Tools" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonOEMLinks", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonPrograms", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonStartMenu", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonStartup", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonTemplates", "C:\\ProgramData\\Microsoft\\Windows\\Templates" ),
        std::pair<std::string, std::string>( "FOLDERID_ComputerFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ConflictFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ConnectionsFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Contacts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ControlPanelFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Cookies", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\INetCookies" ),
        std::pair<std::string, std::string>( "FOLDERID_Desktop", "C:\\Users\\Default\\Desktop" ),
        std::pair<std::string, std::string>( "FOLDERID_DeviceMetadataStore", "C:\\ProgramData\\Microsoft\\Windows\\DeviceMetadataStore" ),
        std::pair<std::string, std::string>( "FOLDERID_Documents", "C:\\Users\\Default\\Documents" ),
        std::pair<std::string, std::string>( "FOLDERID_DocumentsLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Downloads", "C:\\Users\\Default\\Downloads" ),
        std::pair<std::string, std::string>( "FOLDERID_Favorites", "C:\\Users\\Default\\Favorites" ),
        std::pair<std::string, std::string>( "FOLDERID_Fonts", "C:\\WINDOWS\\Fonts" ),
        std::pair<std::string, std::string>( "FOLDERID_GameTasks", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\GameExplorer" ),
        std::pair<std::string, std::string>( "FOLDERID_Games", "" ),
        std::pair<std::string, std::string>( "FOLDERID_History", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\History" ),
        std::pair<std::string, std::string>( "FOLDERID_HomeGroup", "" ),
        std::pair<std::string, std::string>( "FOLDERID_HomeGroupCurrentUser", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ImplicitAppShortcuts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_InternetCache", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\INetCache" ),
        std::pair<std::string, std::string>( "FOLDERID_InternetFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Libraries", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Links", "C:\\Users\\Default\\Links" ),
        std::pair<std::string, std::string>( "FOLDERID_LocalAppData", "C:\\Users\\Default\\AppData\\Local" ),
        std::pair<std::string, std::string>( "FOLDERID_LocalAppDataLow", "" ),
        std::pair<std::string, std::string>( "FOLDERID_LocalizedResourcesDir", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Music", "C:\\Users\\Default\\Music" ),
        std::pair<std::string, std::string>( "FOLDERID_MusicLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_NetHood", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Network Shortcuts" ),
        std::pair<std::string, std::string>( "FOLDERID_NetworkFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Objects3D", "" ),
        std::pair<std::string, std::string>( "FOLDERID_OriginalImages", "" ),
        std::pair<std::string, std::string>( "FOLDERID_PhotoAlbums", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Pictures", "C:\\Users\\Default\\Pictures" ),
        std::pair<std::string, std::string>( "FOLDERID_PicturesLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Playlists", "" ),
        std::pair<std::string, std::string>( "FOLDERID_PrintersFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Profile", "C:\\Users\\Default" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramData", "C:\\ProgramData" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFiles", "C:\\Program Files" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesCommon", "C:\\Program Files\\Common Files" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesCommonX64", "C:\\Program Files\\Common Files" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesCommonX86", "C:\\Program Files (x86)\\Common Files" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesX64", "C:\\Program Files" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesX86", "C:\\Program Files (x86)" ),
        std::pair<std::string, std::string>( "FOLDERID_Programs", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs" ),
        std::pair<std::string, std::string>( "FOLDERID_Public", "C:\\Users\\Public" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicDocuments", "C:\\Users\\Public\\Documents" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicDownloads", "C:\\Users\\Public\\Downloads" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicGameTasks", "C:\\ProgramData\\Microsoft\\Windows\\GameExplorer" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicLibraries", "C:\\Users\\Public\\Libraries" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicMusic", "C:\\Users\\Public\\Music" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicPictures", "C:\\Users\\Public\\Pictures" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicRingtones", "C:\\ProgramData\\Microsoft\\Windows\\Ringtones" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicUserTiles", "C:\\Users\\Public\\AccountPictures" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicVideos", "C:\\Users\\Public\\Videos" ),
        std::pair<std::string, std::string>( "FOLDERID_QuickLaunch", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Internet Explorer\\Quick Launch" ),
        std::pair<std::string, std::string>( "FOLDERID_Recent", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Recent" ),
        std::pair<std::string, std::string>( "FOLDERID_RecordedTVLibrary", "C:\\Users\\Public\\Libraries\\RecordedTV.library-ms" ),
        std::pair<std::string, std::string>( "FOLDERID_RecycleBinFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ResourceDir", "C:\\WINDOWS\\resources" ),
        std::pair<std::string, std::string>( "FOLDERID_Ringtones", "" ),
        std::pair<std::string, std::string>( "FOLDERID_RoamedTileImages", "" ),
        std::pair<std::string, std::string>( "FOLDERID_RoamingAppData", "C:\\Users\\Default\\AppData\\Roaming" ),
        std::pair<std::string, std::string>( "FOLDERID_RoamingTiles", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SEARCH_CSC", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SEARCH_MAPI", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SampleMusic", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SamplePictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SamplePlaylists", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SampleVideos", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedGames", "C:\\Users\\Default\\Saved Games" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedPictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedPicturesLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedSearches", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Screenshots", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SearchHistory", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SearchHome", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SearchTemplates", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SendTo", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\SendTo" ),
        std::pair<std::string, std::string>( "FOLDERID_SidebarDefaultParts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SidebarParts", "" ),
        //std::pair<std::string, std::string>( "FOLDERID_SkyDrive", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SkyDriveCameraRoll", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SkyDriveDocuments", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SkyDrivePictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_StartMenu", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu" ),
        std::pair<std::string, std::string>( "FOLDERID_Startup", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SyncManagerFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SyncResultsFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SyncSetupFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_System", "C:\\WINDOWS\\system32" ),
        std::pair<std::string, std::string>( "FOLDERID_SystemX86", "C:\\WINDOWS\\SysWOW64" ),
        std::pair<std::string, std::string>( "FOLDERID_Templates", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Templates" ),
        std::pair<std::string, std::string>( "FOLDERID_UserPinned", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UserProfiles", "C:\\Users" ),
        std::pair<std::string, std::string>( "FOLDERID_UserProgramFiles", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UserProgramFilesCommon", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UsersFiles", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UsersLibraries", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Videos", "C:\\Users\\Default\\Videos" ),
        std::pair<std::string, std::string>( "FOLDERID_VideosLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Windows", "C:\\WINDOWS" )
        )
);
#else
// NOTE: These win 32 tests are still crafted to run on a 64 bit machine. Of a real 32 bit machine, some if these tests "Program Files (x86)" will fail
INSTANTIATE_TEST_SUITE_P(
    VerifyKnownFolderId,
    ComponentTestWindowsUtilites,
    Values(
        std::pair<std::string, std::string>( "FOLDERID_AccountPictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AddNewPrograms", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AdminTools", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataDesktop", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataDocuments", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataFavorites", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppDataProgramData", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppUpdates", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ApplicationShortcuts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_AppsFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CDBurning", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CameraRoll", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ChangeRemovePrograms", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonAdminTools", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Administrative Tools" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonOEMLinks", "" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonPrograms", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonStartMenu", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonStartup", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup" ),
        std::pair<std::string, std::string>( "FOLDERID_CommonTemplates", "C:\\ProgramData\\Microsoft\\Windows\\Templates" ),
        std::pair<std::string, std::string>( "FOLDERID_ComputerFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ConflictFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ConnectionsFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Contacts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ControlPanelFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Cookies", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\INetCookies" ),
        std::pair<std::string, std::string>( "FOLDERID_Desktop", "C:\\Users\\Default\\Desktop" ),
        std::pair<std::string, std::string>( "FOLDERID_DeviceMetadataStore", "C:\\ProgramData\\Microsoft\\Windows\\DeviceMetadataStore" ),
        std::pair<std::string, std::string>( "FOLDERID_Documents", "C:\\Users\\Default\\Documents" ),
        std::pair<std::string, std::string>( "FOLDERID_DocumentsLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Downloads", "C:\\Users\\Default\\Downloads" ),
        std::pair<std::string, std::string>( "FOLDERID_Favorites", "C:\\Users\\Default\\Favorites" ),
        std::pair<std::string, std::string>( "FOLDERID_Fonts", "C:\\WINDOWS\\Fonts" ),
        std::pair<std::string, std::string>( "FOLDERID_GameTasks", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\GameExplorer" ),
        std::pair<std::string, std::string>( "FOLDERID_Games", "" ),
        std::pair<std::string, std::string>( "FOLDERID_History", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\History" ),
        std::pair<std::string, std::string>( "FOLDERID_HomeGroup", "" ),
        std::pair<std::string, std::string>( "FOLDERID_HomeGroupCurrentUser", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ImplicitAppShortcuts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_InternetCache", "C:\\Users\\Default\\AppData\\Local\\Microsoft\\Windows\\INetCache" ),
        std::pair<std::string, std::string>( "FOLDERID_InternetFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Libraries", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Links", "C:\\Users\\Default\\Links" ),
        std::pair<std::string, std::string>( "FOLDERID_LocalAppData", "C:\\Users\\Default\\AppData\\Local" ),
        std::pair<std::string, std::string>( "FOLDERID_LocalAppDataLow", "" ),
        std::pair<std::string, std::string>( "FOLDERID_LocalizedResourcesDir", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Music", "C:\\Users\\Default\\Music" ),
        std::pair<std::string, std::string>( "FOLDERID_MusicLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_NetHood", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Network Shortcuts" ),
        std::pair<std::string, std::string>( "FOLDERID_NetworkFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Objects3D", "" ),
        std::pair<std::string, std::string>( "FOLDERID_OriginalImages", "" ),
        std::pair<std::string, std::string>( "FOLDERID_PhotoAlbums", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Pictures", "C:\\Users\\Default\\Pictures" ),
        std::pair<std::string, std::string>( "FOLDERID_PicturesLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Playlists", "" ),
        std::pair<std::string, std::string>( "FOLDERID_PrintersFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Profile", "C:\\Users\\Default" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramData", "C:\\ProgramData" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFiles", "C:\\Program Files (x86)" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesCommon", "C:\\Program Files (x86)\\Common Files" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesCommonX64", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesCommonX86", "C:\\Program Files (x86)\\Common Files" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesX64", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ProgramFilesX86", "C:\\Program Files (x86)" ),
        std::pair<std::string, std::string>( "FOLDERID_Programs", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs" ),
        std::pair<std::string, std::string>( "FOLDERID_Public", "C:\\Users\\Public" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicDocuments", "C:\\Users\\Public\\Documents" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicDownloads", "C:\\Users\\Public\\Downloads" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicGameTasks", "C:\\ProgramData\\Microsoft\\Windows\\GameExplorer" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicLibraries", "C:\\Users\\Public\\Libraries" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicMusic", "C:\\Users\\Public\\Music" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicPictures", "C:\\Users\\Public\\Pictures" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicRingtones", "C:\\ProgramData\\Microsoft\\Windows\\Ringtones" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicUserTiles", "C:\\Users\\Public\\AccountPictures" ),
        std::pair<std::string, std::string>( "FOLDERID_PublicVideos", "C:\\Users\\Public\\Videos" ),
        std::pair<std::string, std::string>( "FOLDERID_QuickLaunch", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Internet Explorer\\Quick Launch" ),
        std::pair<std::string, std::string>( "FOLDERID_Recent", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Recent" ),
        std::pair<std::string, std::string>( "FOLDERID_RecordedTVLibrary", "C:\\Users\\Public\\Libraries\\RecordedTV.library-ms" ),
        std::pair<std::string, std::string>( "FOLDERID_RecycleBinFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_ResourceDir", "C:\\WINDOWS\\resources" ),
        std::pair<std::string, std::string>( "FOLDERID_Ringtones", "" ),
        std::pair<std::string, std::string>( "FOLDERID_RoamedTileImages", "" ),
        std::pair<std::string, std::string>( "FOLDERID_RoamingAppData", "C:\\Users\\Default\\AppData\\Roaming" ),
        std::pair<std::string, std::string>( "FOLDERID_RoamingTiles", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SEARCH_CSC", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SEARCH_MAPI", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SampleMusic", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SamplePictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SamplePlaylists", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SampleVideos", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedGames", "C:\\Users\\Default\\Saved Games" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedPictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedPicturesLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SavedSearches", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Screenshots", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SearchHistory", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SearchHome", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SearchTemplates", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SendTo", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\SendTo" ),
        std::pair<std::string, std::string>( "FOLDERID_SidebarDefaultParts", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SidebarParts", "" ),
        //std::pair<std::string, std::string>( "FOLDERID_SkyDrive", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SkyDriveCameraRoll", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SkyDriveDocuments", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SkyDrivePictures", "" ),
        std::pair<std::string, std::string>( "FOLDERID_StartMenu", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu" ),
        std::pair<std::string, std::string>( "FOLDERID_Startup", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SyncManagerFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SyncResultsFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_SyncSetupFolder", "" ),
        std::pair<std::string, std::string>( "FOLDERID_System", "C:\\WINDOWS\\system32" ),
        std::pair<std::string, std::string>( "FOLDERID_SystemX86", "C:\\WINDOWS\\SysWOW64" ),
        std::pair<std::string, std::string>( "FOLDERID_Templates", "C:\\Users\\Default\\AppData\\Roaming\\Microsoft\\Windows\\Templates" ),
        std::pair<std::string, std::string>( "FOLDERID_UserPinned", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UserProfiles", "C:\\Users" ),
        std::pair<std::string, std::string>( "FOLDERID_UserProgramFiles", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UserProgramFilesCommon", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UsersFiles", "" ),
        std::pair<std::string, std::string>( "FOLDERID_UsersLibraries", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Videos", "C:\\Users\\Default\\Videos" ),
        std::pair<std::string, std::string>( "FOLDERID_VideosLibrary", "" ),
        std::pair<std::string, std::string>( "FOLDERID_Windows", "C:\\WINDOWS" )
    )
);
#endif

TEST_F( ComponentTestWindowsUtilites, NoFilesToFind )
{
    std::filesystem::path path( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> files;

    std::vector<std::filesystem::path> expected;

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, NoFilesToFindWildcard )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;

    std::vector<std::filesystem::path> expected;

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, OneFileToFind )
{
    std::filesystem::path path( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, OneFileToFindWildcard )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, NoFilesFindWildcardMultipleExistsingFiles )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.txt" );
    files.push_back( m_baseTestPath / "test.json" );

    std::vector<std::filesystem::path> expected;

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, OneFileToFindWildcardMultipleExistingFiles )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.xml" );
    files.push_back( m_baseTestPath / "test.txt" );
    files.push_back( m_baseTestPath / "test.json" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, SingleCharacterWildcardSearch )
{
    std::filesystem::path path( m_baseTestPath / "policy.???" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "policy.xml" );
    files.push_back( m_baseTestPath / "policy.txt" );
    files.push_back( m_baseTestPath / "policy.json" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "policy.xml" );
    expected.push_back( m_baseTestPath / "policy.txt" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, SearchWithDirectoryWildcard )
{
    std::filesystem::path path( m_baseTestPath / "Temp*Path/test.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "TempPath/test.xml" );
    files.push_back( m_baseTestPath / "TempPath/other.xml" );
    files.push_back( m_baseTestPath / "TempNewPath/test.xml" );
    files.push_back( m_baseTestPath / "TempNewPath/other.xml" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "TempPath/test.xml" );
    expected.push_back( m_baseTestPath / "TempNewPath/test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, SearchWithDirectoryWildcardAndFileWildcard )
{
    std::filesystem::path path( m_baseTestPath / "Temp*Path/*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "TempPath/test.xml" );
    files.push_back( m_baseTestPath / "TempPath/test2.xml" );
    files.push_back( m_baseTestPath / "TempPath/test.txt" );
    files.push_back( m_baseTestPath / "TempNewPath/test.xml" );
    files.push_back( m_baseTestPath / "TempNewPath/test2.xml" );
    files.push_back( m_baseTestPath / "TempNewPath/test.txt" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "TempPath/test.xml" );
    expected.push_back( m_baseTestPath / "TempPath/test2.xml" );
    expected.push_back( m_baseTestPath / "TempNewPath/test.xml" );
    expected.push_back( m_baseTestPath / "TempNewPath/test2.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsUtilites, WillResolveKnownFolderID )
{
    std::string knownFolderString = "C:\\ProgramData";
    auto rtn = WindowsUtilities::ResolvePath( "<FOLDERID_ProgramData>" );

    EXPECT_EQ( rtn, knownFolderString );
}

TEST_F( ComponentTestWindowsUtilites, WillResolveKnownFolderIDWithPrefix )
{
    std::string prefix = "prefix";
    std::string knownFolderString = "C:\\ProgramData";
    std::string suffix = "suffix";

    auto rtn = WindowsUtilities::ResolvePath( prefix + "<FOLDERID_ProgramData>" + suffix );

    EXPECT_EQ( rtn, prefix + knownFolderString + suffix );
}

TEST_F( ComponentTestWindowsUtilites, WillNotModifyPathWhenKnownFolderIsEmpty )
{
    std::string folder = "prefix<FOLDERID_SomeKnownFolder>suffix";

    auto rtn = WindowsUtilities::ResolvePath( folder );

    EXPECT_EQ( rtn, folder );
}

TEST_F( ComponentTestWindowsUtilites, WillNotResolveKnownFolderWhenTagNotFound )
{
    auto rtn = WindowsUtilities::ResolvePath( "C:/Windows/Somthing" );

    EXPECT_EQ( rtn, "C:/Windows/Somthing" );
}

TEST_F( ComponentTestWindowsUtilites, WillResolveProgramArguments )
{
    std::string arg = "/arg2 <FOLDERID_ProgramData>/Cisco/policy.xml";

    EXPECT_EQ( WindowsUtilities::ResolvePath( arg ), "/arg2 C:\\ProgramData/Cisco/policy.xml" );
}
