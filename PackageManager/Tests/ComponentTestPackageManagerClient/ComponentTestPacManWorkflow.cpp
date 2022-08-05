#include "ComponentTestPacManBase.h"
#include <filesystem>

extern std::string _ucReponseNoPackages;

class ComponentTestPacManWorkflow : public ComponentTestPacManBase
{
public:
    virtual ~ComponentTestPacManWorkflow() {}
};

TEST_F( ComponentTestPacManWorkflow, PacManWillExportPackageList )
{
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoPackages, { 200, 0 } );

    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockFileUtil->MakeFileSizeReturn( 100 );
    m_mockFileUtil->MakeEraseFileReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" );

    EXPECT_CALL( *m_mockFileUtil, WriteLine( _, StrEq( _ucReponseNoPackages ) ) ).Times( 1 );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();
}
