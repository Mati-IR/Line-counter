#ifndef MAIN_CPP_LINE_COUNTER_H
#define MAIN_CPP_LINE_COUNTER_H
#define MAX_THREADS 50

#include <iostream>
#include <filesystem>
#include <limits>
#include <mutex>
#include <fstream>
#include "thread_pool.hpp"
#include <chrono>
#include <exception>

using std::cout, std::cin, std::endl;
using namespace std::chrono;//namespace for timer
namespace fs = std::filesystem;


class LineCounter
{

    fs::path targetPath;//path provided by user
    long long int sumAllLines;//used to store number of lines from files in targetPath
    std::mutex sumAllLinesMutex;//allows changing sumAllLines
    int maxThreads;//stores the highest allowed number of threads
    thread_pool *pool;
    bool errorEncountered;

    void clearCin();//clear contents and flags of cin
    friend int threadRoutine(fs::path mypath, LineCounter *app);
public:
    LineCounter();
    LineCounter(int maxThreads, fs::path targetPath);//TODO:Unstable, path is not processed correctly
    ~LineCounter();

    int searchDir(fs::path currentPath);//searches dir recursively and delegates counting lines of files to thread pool
    long long int howManyLines(const fs::path& dir);//returns number of lines in a single file
    int set_maxThreads();//used in default constructor
    int set_maxThreads(int maxThreads);
    int set_targetPath();//used in default constructor
    int set_targetPath(fs::path targetPath);
    bool get_errorEncountered();
    fs::path get_targetPath();
    std::string extractFileName(fs::path dir);//returns file name from path
    long long int get_sumAllLines();
    int run();//run application
};

int threadRoutine(fs::path mypath, LineCounter *app);//function passed to thread pool


#endif //MAIN_CPP_LINE_COUNTER_H
