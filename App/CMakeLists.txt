cmake_minimum_required(VERSION 3.21)
project(RecruitmentTask)

set(CMAKE_CXX_STANDARD 17)

add_executable(func main.cpp thread_pool.hpp line_counter.cpp line_counter.h)
