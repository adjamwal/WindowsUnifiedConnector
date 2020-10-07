#include "MocksCommon.h"
#include "IFileUtil.h"

class MockFileUtil : public IFileUtil
{
public:
    MockFileUtil();
    ~MockFileUtil();

    MOCK_METHOD1( ReadFile, std::string( const std::string& filename ));
    void MakeReadFileReturn( std::string value );
    void ExpectReadFileNotCalled();

private:
    std::string m_defaultString;
};