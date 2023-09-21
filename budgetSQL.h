#ifndef BUDGETSQL_H
#define BUDGETSQL_H

#include <iostream>
#include <sqlite3.h>
#include <limits>
#include <string>

void createTable(sqlite3 *db);
void dropTable(sqlite3 *db);
void addTransaction(sqlite3 *db);
void viewTransactions(sqlite3 *db);
void deleteTransaction(sqlite3 *db);
void filterByCategory(sqlite3 *db);
void updateTransactions(sqlite3 *db);
void searchTransactions(sqlite3 *db);
void exportToCSV(sqlite3 *db);
#endif // BUDGETSQL_H
