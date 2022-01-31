#include "pch.h"
#include "../App/line_counter.h"
#include "../App/line_counter.cpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using std::cout, std::cin, std::endl;

std::string testFileName = "test_files";
fs::path defaultDir(testFileName);
int maxThreads = 5;

struct TF : public testing::Test
{
    LineCounter* testApp;
    void SetUp() 
    { 
        testApp = new LineCounter(maxThreads, defaultDir);
    }

};

TEST_F(TF, TEST_get_targetPath)
{    
    EXPECT_EQ(-1, threadRoutine(testApp->get_targetPath(), NULL));
    EXPECT_EQ(-1, threadRoutine(testApp->get_targetPath(), nullptr));
    EXPECT_EQ(0, threadRoutine(testApp->get_targetPath(), testApp));
 }

TEST_F(TF, TEST_get_errorEncountered)
{
    EXPECT_FALSE(testApp->get_errorEncountered());
}
TEST_F(TF, TEST_threadRoutine)
{
    EXPECT_EQ(-1, threadRoutine(testApp->get_targetPath(), NULL));
    EXPECT_EQ(-1, threadRoutine(testApp->get_targetPath(), nullptr));
    EXPECT_EQ(0, threadRoutine(testApp->get_targetPath(), testApp));
    EXPECT_FALSE(testApp->get_errorEncountered());
}

TEST_F(TF, TEST_run_outcome)
{
    testApp->run();
    EXPECT_EQ(22, testApp->get_sumAllLines());
}

TEST_F(TF, TEST_howManyLines)
{
    EXPECT_EQ(22, testApp->howManyLines(defaultDir));
}