#include "stdafx.h"
#include "ProxyDiscoveryEngine.h"
#include "MockWinHttpWrapper.h"
#include "ProxyStringParser.h"

ProxyInfoModel CreateProxyInfo(LPWSTR server, USHORT port, DWORD discoveryMethod)
{
    ProxyInfoModel proxyInfo;
    proxyInfo.SetProxyPort(port);
    proxyInfo.SetProxyServer(server);
    proxyInfo.SetProxyDiscoveryMode(discoveryMethod);

    return proxyInfo;
}

class TestProxyStringParser : public ::testing::Test
{
protected:
    virtual void SetUp()
    {

    };

    virtual void TearDown()
    {
        
    }
protected:
    ProxyStringParser m_patient;
};

TEST_F(TestProxyStringParser, WillFailToParseProxyStringWhenProxyStringIsNull)
{  
    PROXY_INFO_LIST list;
    LPWSTR proxyInfo = NULL;

    BOOL status = m_patient.ParseProxyString(proxyInfo, list, PROXY_FIND_REG);

    EXPECT_EQ(status, FALSE);
}

TEST_F(TestProxyStringParser, WillFailToParseProxyStringWhenProxyStringIsEmpty)
{
    PROXY_INFO_LIST list;
    LPWSTR proxyInfo = L"";

    BOOL status = m_patient.ParseProxyString(proxyInfo, list, PROXY_FIND_REG);

    EXPECT_EQ(status, FALSE);
}

TEST_F(TestProxyStringParser, WillParseProxyStringWithOneProxy)
{
    PROXY_INFO_LIST actualList;
    PROXY_INFO_LIST expectedList;
    expectedList.emplace_back(CreateProxyInfo(L"127.0.0.1", 80, NULL));

    BOOL status = m_patient.ParseProxyString(L"127.0.0.1:80", actualList, NULL);

    EXPECT_EQ(actualList, expectedList);
}

TEST_F(TestProxyStringParser, WillParseProxyStringWithTwoParametersAndSemicolonSeparator)
{
    PROXY_INFO_LIST actualList;
    PROXY_INFO_LIST expectedList;
    expectedList.emplace_back(CreateProxyInfo(L"127.0.0.1", 80, PROXY_FIND_REG));
    expectedList.emplace_back(CreateProxyInfo(L"192.168.1.254", 81, PROXY_FIND_REG));

    BOOL status = m_patient.ParseProxyString(L"127.0.0.1:80;192.168.1.254:81", actualList, PROXY_FIND_REG);
    EXPECT_EQ(status, TRUE);

    EXPECT_EQ(actualList, expectedList);
}

TEST_F(TestProxyStringParser, WillParseProxyStringWithTwoParametersAndSpaceSeparator)
{
    PROXY_INFO_LIST actualList;
    PROXY_INFO_LIST expectedList;
    expectedList.emplace_back(CreateProxyInfo(L"127.0.0.1", 80, PROXY_FIND_REG));
    expectedList.emplace_back(CreateProxyInfo(L"192.168.1.254", 81, PROXY_FIND_REG));

    BOOL status = m_patient.ParseProxyString(L"127.0.0.1:80 192.168.1.254:81", actualList, PROXY_FIND_REG);
    EXPECT_EQ(status, TRUE);
    
    EXPECT_EQ(actualList, expectedList);
}