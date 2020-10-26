#include "ISslUtil.h"

class SslUtil : public ISslUtil
{
public:
    SslUtil();
    ~SslUtil();

    int32_t DecodeBase64( const std::string& base64Str, std::vector<uint8_t>& output ) override;

private:
    size_t CalculateDecodeLength( const char* b64input );
};