#include "pch.h"
#include "IPmPlatformComponentManager.h"
#include "WindowsComponentManager.h"
#include "IUcLogger.h"
#include "PmTypes.h"
#include <memory>

class ComponentTestWindowsPackageManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_patient = std::make_unique<WindowsComponentManager>();
    }

    void TearDown()
    {
        m_patient.reset();
    }

    std::unique_ptr<WindowsComponentManager> m_patient;
};

TEST_F( ComponentTestWindowsPackageManager, CanRunUpdate )
{
    //mock out windows calls and test all paths
    //pass interface to WindowsComponentManager
    PmPackage p;
    p.Name = "lol";
    m_patient->UpdateComponent( p );
}
