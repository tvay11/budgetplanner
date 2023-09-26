cmake_minimum_required(VERSION 3.20)
project(budgetplanner)

set(CMAKE_CXX_STANDARD 14)

#GTEST
add_subdirectory(googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Create main executable
add_executable(budgetplanner main.cpp sqlite3.c budgetSQL.cpp budgetSQL.h)

# Create test executable
add_executable(runBudgetSQLTests budgetSQLtest.cpp budgetSQL.cpp sqlite3.c)

# Link Google Test to test executable
target_link_libraries(runBudgetSQLTests gtest gtest_main)
