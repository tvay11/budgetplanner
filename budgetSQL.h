#ifndef BUDGETSQL_H
#define BUDGETSQL_H

#include <iostream>
#include <sqlite3.h>
#include <limits>
#include <string>

using namespace std;

void createTable(sqlite3 *db);
void dropTable(sqlite3 *db);
void addTransaction(sqlite3 *db);
void insertIntoBudgetDB(sqlite3 *db, const pair<string, string>& choice, const string& title, float amount);
pair<string, float> getUserInput(const pair<string, string>& choice);
static int callbackCSV(void *data, int argc, char **argv, char **azColName);
static int callback(void *NotUsed, int argc, char **argv, char **azColName);
static int callbackTable(void *NotUsed, int argc, char **argv, char **azColName);
string center(const string &str, int width);
pair<string, string> selectCategory();
bool getValidID(sqlite3 *db, int &id);
void viewTransactions(sqlite3 *db);
void deleteTransactionUI(sqlite3 *db);
bool deleteTransactionByID(sqlite3 *db, int id);
bool idExists(sqlite3 *db, int id);
void filterByCategory(sqlite3 *db);
bool updateDatabase(sqlite3 *db, const string &sqlUpdate);
int getUserChoice();
void updateTransactions(sqlite3 *db);
void searchTransactions(sqlite3 *db);
void exportToCSV(sqlite3 *db);
#endif // BUDGETSQL_H
