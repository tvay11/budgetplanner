cmake_minimum_required(VERSION 3.20)
project(budgetplanner)

set(CMAKE_CXX_STANDARD 14)

add_executable(budgetplanner main.cpp sqlite3.c)
