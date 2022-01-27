#include "pch.h"
#include "PmAgentContainer.h"

class ComponentTestPmAgent : public ::testing::Test
{
};

TEST_F(ComponentTestPmAgent, ContainerWillNotCrashDuringDestruction)
{
    std::wstring bsConfigFile = ( L"bs.json");
    std::wstring pmConfigFile = (L"config.json");

    PmAgentContainer *patient = new PmAgentContainer(bsConfigFile, pmConfigFile);
    ASSERT_NE(patient, nullptr);

    try {
        delete patient;
    }
    catch (std::exception& e) {
        printf("Exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}