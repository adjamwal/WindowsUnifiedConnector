#include "pch.h"

#include <filesystem>
#include <stdio.h>
#include "UcLogFile.h"

class ComponentTestLibLogger : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_tempPathName = std::filesystem::temp_directory_path();
        m_tempPathName += L"testLog.log";
        std::filesystem::path tempPath(m_tempPathName);
        if (!tempPath.empty()) {
            _wremove(m_tempPathName.c_str());
        }

        m_logFile = new UcLogFile();
        m_logFile->Init(m_tempPathName.c_str());
    }

    void TearDown()
    {
        delete m_logFile;
    }

    std::wstring m_tempPathName;
    UcLogFile* m_logFile;
    inline static const std::string s_testLevel = "TEST";
    inline static const std::string s_testMessage = "This is a test";
};
TEST_F(ComponentTestLibLogger, LogToFile )
{
    m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
    m_logFile->~UcLogFile();

    FILE* hFile = _wfopen(m_tempPathName.c_str(), L"r");
    char* content = new char[100];
    fgets(content, 99, hFile);
    fclose(hFile);
    _wremove(m_tempPathName.c_str());

    std::string result(content);
    delete[] content;

    std::string expected(s_testLevel + ": " + s_testMessage);
    EXPECT_TRUE(result.find(expected) != std::string::npos);
}

TEST_F(ComponentTestLibLogger, FileWillRotate)
{
    m_logFile->SetLogConfig(70, 3); // The three lines below are 65 bytes each in the file
    m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
    m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
    m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
    m_logFile->~UcLogFile();

    std::set<std::wstring> logFileSet;
    m_tempPathName = m_tempPathName.substr(0, m_tempPathName.length() - 4); // strip off .log
    m_tempPathName.append(L"*");

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(m_tempPathName.c_str(), &FindFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            logFileSet.insert(FindFileData.cFileName);
            _wremove(FindFileData.cFileName);
        } while (FindNextFile(hFind, &FindFileData) != 0);

        if (hFind) FindClose(hFind);
    }

    EXPECT_TRUE(logFileSet.size() == 3);
}
TEST_F(ComponentTestLibLogger, NoCrashOnMultipleThreads)
{
    std::thread thread1([this]() {
        m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
        });
    std::thread thread2([this]() {
        m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
        });
    std::thread thread3([this]() {
        m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
        });
    std::thread thread4([this]() {
        m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
        });
    std::thread thread5([this]() {
        m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
        });
    std::thread thread6([this]() {
        m_logFile->WriteLogLine(s_testLevel.c_str(), s_testMessage.c_str());
        });

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    thread6.join();

    FILE* hFile = _wfopen(m_tempPathName.c_str(), L"r");
    fseek(hFile, 0, SEEK_END);
    size_t size = ftell(hFile);
    rewind(hFile);

    char* content = new char[size + 1];
    fread(content, sizeof(char), size, hFile);
    std::string expected(s_testLevel + ": " + s_testMessage);
    std::string result(content);
    delete[] content;

    fclose(hFile);
    _wremove(m_tempPathName.c_str());

    size_t idx = result.find(expected);
    unsigned occurances = 0;
    while (idx != std::string::npos)
    {
        idx = result.find(expected, idx + 1);
        occurances++;
    }

    EXPECT_TRUE(occurances == 6);
}

