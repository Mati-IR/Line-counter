#include "line_counter.h"


int threadRoutine(fs::path mypath, LineCounter *app)//function passed to pool thread queue, later executed by thread
{
    if (app == NULL || !exists(mypath))
        return -1;
    do{}
    while (!app->sumAllLinesMutex.try_lock());//waiting for mutex
    app->sumAllLines += app->howManyLines(mypath);
    app->sumAllLinesMutex.unlock();
    return 0;
}


LineCounter::LineCounter()//set initial values of variables
{
    errorEncountered = false;
    this->sumAllLines = 0;
    set_targetPath();
    set_maxThreads();
    pool = new thread_pool(this->maxThreads);// create thread_pool from thread_pool.hpp
    clearCin();
}

LineCounter::LineCounter(int maxThreads, fs::path targetPath)
{
    if(set_maxThreads(maxThreads) != 0 || set_targetPath(targetPath) != 0 )
    {
        cout << "invalid arguments passed to constructor\n";
        cout << "Path passed to function: " << targetPath.string() << endl;
        cout << "fs::is_directory(targetPath) = " <<fs::is_directory(targetPath) << endl;
        cout << "set_targetPath(targetPath) = "  <<  set_targetPath(targetPath) << endl;
        exit(EXIT_FAILURE);
    }
    errorEncountered = false;
    this->sumAllLines = 0;
}

void LineCounter::clearCin()//clear contents and flags of cin
{
    cin.clear();//clear contents of cin
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');//clear cin flags
}



int LineCounter::searchDir(fs::path currentPath)//searches dir recursively and delegates counting lines of files to thread pool
{
    if (!is_directory(currentPath))//invalid path case
        return -1;

    auto iter = fs::recursive_directory_iterator(currentPath, fs::directory_options::skip_permission_denied);
    auto end_iter = fs::end(iter);//for end condition
    auto error = std::error_code();//used to detect errors
    cout << "App working...\n";
    for (; iter != end_iter; iter.increment(error))//iterate through directory
    {
        if (error)//skips problematic files, permission denied errors, etc.
        {
            errorEncountered = true;//set flag to true
            continue;
        }
        pool->submit(threadRoutine, iter->path(), this);//submit task to tasks queue
    }
    this->pool->wait_for_tasks();
    return 0;
}

long long int LineCounter::howManyLines(const fs::path& dir)//returns number of lines in a single file
{
    int linesCounter = 0;
    std::ifstream targetFile(extractFileName(dir));
    std::string buf;

    targetFile.open(dir.string());
    while (getline(targetFile, buf))//iterate through every line in file
    {
        if (!buf.empty())
            linesCounter++;
    }
    targetFile.close();
    return linesCounter;
}

std::string LineCounter::extractFileName(fs::path dir)//returns file name from path
{
    return dir.string().substr(dir.string().find_last_of("/\\") + 1);//returns part of path after last "\"
}

int LineCounter::set_maxThreads()//used in setup
{
    do
    {
        cout << "\rInput max. amount of threads (max. " << MAX_THREADS << "): ";
        cin >> this->maxThreads;
        if (!cin || this->maxThreads <= 0 || this->maxThreads > MAX_THREADS)//Incorrect input (eg.character) or incorrect amount of threads
        {
            cout << "Incorrect input\n";
            clearCin();
            if (this->maxThreads <= 0 || this->maxThreads > MAX_THREADS)//incorrect amount of threads
                cout << "\nInput number between 1 and " << MAX_THREADS << endl;
            continue;
        }else
            break;
    } while(this->maxThreads <= 0 || this->maxThreads > MAX_THREADS);
    return 0;
}
int LineCounter::set_maxThreads(int maxThreads)
{
    if (maxThreads <= 0 || maxThreads > MAX_THREADS)
        return -1;
    this->maxThreads = maxThreads;
    return 0;
}

int LineCounter::set_targetPath()//used in setup()
{
    do
    {
        cout << "Input COMPLETE target path (eg. C:\\Users\\John\\Desktop\\folder)\n";
        cin >> this->targetPath;
        if (!cin || !is_directory(this->targetPath)/*!dirExists(this->targetPath)*/)
        {
            cout << "Incorrect path\n";
            clearCin();//clear cin flags and contents
            continue;
        }else
            break;

    } while(!is_directory(this->targetPath));
    return 0;
}

int LineCounter::set_targetPath(fs::path targetPath)
{
    if(!is_directory(targetPath))
        return -1;
    this->targetPath = targetPath;
    return 0;
}

LineCounter::~LineCounter()
{
    delete pool;
}


int LineCounter::run()//run application
{
    auto start = high_resolution_clock::now();//start task timer
    if(searchDir(this->targetPath) != 0)
    {
        cout << "Path not found\n";
        return -1;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);//calculate time taken for the task

    cout << "\nsum of all lines is = " << this->sumAllLines << endl <<
         "Task took: " << (duration.count() / 1000000.0) << " s\n";//changing microseconds to seconds
    if (errorEncountered)
    {
        cout << "Some files might have been skipped due to errors\nRunning the app as an administrator might solve it but is not recommended due to security reasons\n";
    }

    system("pause");//Press anything to continue...
    return 0;
}

fs::path LineCounter::get_targetPath()
{
    return this->targetPath;
}

long long int LineCounter::get_sumAllLines()
{
    return this->sumAllLines;
}

bool LineCounter::get_errorEncountered()
{
    return this->errorEncountered;
}

